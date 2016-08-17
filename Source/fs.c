#include <stdio.h>
#include <stdint.h>
#include "fs.h"
#include "vfs.h"

#define PAGE_ALIGN	0x1000
#define DEV_SIZE	(1 * 1024 * 1024)

static struct ext2_super_block *ext2_sb;
static struct ext2_blk_grp_des *ext2_bgd;
static struct ext2_dir_entry_2 *ext2_dentry;
static struct ext2_disk_inode (*ext2_inode)[BLOCK_SIZE * BITS_PER_BYTE];

static void * ext2_inode_bit_map;
static void * ext2_data_block_bit_map;
static void *first_data_block;
static void *last_data_block;
unsigned int first_data_blk_no;

void *dev_mem;

void print_sizes ()
{
	printf ("Size of ext2_super_block %d \n", (int)sizeof (struct ext2_super_block));
	printf ("Size of ext2_blk_grp_des %d \n", (int)sizeof (struct ext2_blk_grp_des));
	printf ("Size of ext2_inode %d \n", (int)sizeof (struct ext2_disk_inode));

	return;
}

void print_addresses(void)
{
	printf ("Address of dev_mem = 0x%08x \n", (unsigned int)((int *)dev_mem));
	printf ("Address of ext2_sb = 0x%08x \n", (unsigned int)ext2_sb);
	printf ("Address of ext2_bgd = 0x%08x \n",(unsigned int)ext2_bgd);
	printf ("Address of ext2_inode_bit_map = 0x%08x \n", (unsigned int)ext2_inode_bit_map);
	printf ("Address of ext2_data_block_bit_map = 0x%08x \n", (unsigned int)ext2_data_block_bit_map);
	printf ("Address of ext2_inode = 0x%08x \n", (unsigned int)ext2_inode);
	printf ("Start of data blocks =  0x%08x \n", (unsigned int)first_data_block);
	printf ("Last data block = 0x%08x \n", (unsigned int)last_data_block);

	return;
}

#if 0
void print_addresses(void)
{
	printf ("Address of dev_mem = 0x%08x \n", (unsigned int)((int *)dev_mem));
	printf ("Address of ext2_sb = 0x%08x \n", ext2_sb);
	printf ("Address of ext2_bgd = 0x%08x \n", ext2_bgd);
	printf ("Address of ext2_inode_bit_map = 0x%08x \n", ext2_inode_bit_map);
	printf ("Address of ext2_data_block_bit_map = 0x%08x \n", ext2_data_block_bit_map);
	printf ("Address of ext2_inode = 0x%08x \n", ext2_inode);
	printf ("Start of data blocks = 0x%08x \n", first_data_block);
	printf ("Last data block = 0x%08x \n", last_data_block);

	return;
}
#endif
static int dev_alloc(void)
{
	return posix_memalign(&dev_mem, PAGE_ALIGN, DEV_SIZE);
}

#if 0
static init_data_block(void)
{
	unsigned int *bmap = first_data_block;
	int i = 0;
	
	first_data_blk_no = (first_data_block - dev_mem) / BLOCK_SIZE;
	printf ("First data block = %u \n", first_data_blk_no);

	for (i=0; i<first_data_blk_no; i++) 
	{
		*bmap = (*bmap | 0x1); 
		*bmap <<= 1;
	}
}

#endif

unsigned int init_data_block(void )
{
	unsigned int *bit_map = first_data_block;
	unsigned int bit_position = 0;
	unsigned int *bmap;
	unsigned int bit_pos = 0;
	int i;

//	for (i=0; i < ((int*)first_data_block / WORD_SIZE); i++, bit_map += 1)
	for (i=0; i < (* (int *)first_data_block / WORD_SIZE); i++, bit_map += 1)
	{
		for (bit_pos = 0, bmap = bit_map; bit_pos < 32; bit_pos++, *bmap >>= 1)
		{
			if (!(*bmap & 0x1))
			{
				*bit_map |= (1 << bit_pos);
				return (bit_position + bit_pos);
			}
		}
		bit_position += bit_pos;
	}
	
	return 0;	
}

static void init_ext2(void)
{
	unsigned int *bmap;

	ext2_sb = (struct ext2_super_block *) dev_mem;
	ext2_bgd = (struct ext2_blk_grp_des *) (ext2_sb + 1);

	ext2_inode_bit_map = ext2_bgd + 1;
	bmap = ext2_inode_bit_map;
	*bmap |= 0x1; // Reserving the first bit of inode bit map
	DB_PRINT("address of  inode bit_map = 0x%08x \n",ext2_inode_bit_map);
	DB_PRINT(" *bmap value = %d \n",*bmap);	

	ext2_data_block_bit_map = ext2_inode_bit_map + BLOCK_SIZE;
	bmap = ext2_data_block_bit_map;
	*bmap |= 0x1; // Reserving the first bit of data blk bit map

	ext2_inode = (struct ext2_disk_inode (*)[])(ext2_data_block_bit_map + BLOCK_SIZE);

	first_data_block = ext2_inode + 1;
	printf ("first data block = %08x \n", (unsigned int )first_data_block);
	init_data_block();
	last_data_block = dev_mem + DEV_SIZE;

	return;	
}

unsigned int get_free_bit(void *block_addr)
{
	unsigned int *bit_map = block_addr;
	unsigned int bit_position = 0;
	unsigned int *bmap;
	unsigned int bit_pos = 0;
	int i;

	printf ("block_addr = 0x%08x \n", (unsigned int)(uintptr_t)block_addr);
	for (i=0; i < WORDS_PER_BLOCK; i++, bit_map += 1)
	{
		for (bit_pos = 0, bmap = bit_map; bit_pos < 32; bit_pos++ /*, *bmap >>= 1*/)
		{
			if (!(*bmap & (0x1<<bit_pos)))
			{
				*bit_map |= (1 << bit_pos);
				return (bit_position + bit_pos);
			}
		}
		bit_position += bit_pos;
	}
	
	return 0;	
}

unsigned int get_free_data_block(void)
{
	//return get_free_bit(first_data_block);
	return get_free_bit(ext2_data_block_bit_map);
}


unsigned int get_free_inode(void)
{
	return get_free_bit(ext2_inode_bit_map);
}

static void create_root_inode (void)
{
	struct ext2_disk_inode *inode = (struct ext2_disk_inode *) ext2_inode;
	unsigned int bit_pos = get_free_inode();

	inode[bit_pos].i_ctime = time(NULL);
	inode[bit_pos].i_size = BLOCK_SIZE;
	inode[bit_pos].i_blocks = 1;
	inode[bit_pos].i_block[0] = get_free_data_block();

	printf ("Free data block = %u \n", inode[bit_pos].i_block[0]);

	return;
}

void fs_init(void)
{

	if (dev_alloc())
	{
		printf ("posix_memalign: error \n");
	}

	init_ext2();
	create_root_inode();

#if 0
	printf ("Address of dev = 0x%08x \n", dev_mem);
	print_sizes ();

#endif
	print_addresses();

}

void get_root_inode_info(struct inode *in)
{
//	struct ext2_disk_inode *inode = (struct ext2_disk_inode *) ext2_inode;

	in->i_no = 1;
	in->blocks = 1;

	return;
}
static unsigned int get_next_dentry_offset(unsigned int in)
{
	struct ext2_disk_inode *inode = (struct ext2_disk_inode *) ext2_inode;
	unsigned int blk_no;
	unsigned char *rec;
  
	blk_no = inode[in].i_block[0];
	rec = dev_mem + (BLOCK_SIZE * blk_no);
//#if 0
	struct ext2_dir_entry_2 *pd_entry;
	unsigned int ino;
	pd_entry = (struct ext2_dir_entry_2 *)rec ;
	do {
		ino = pd_entry->rec_len;
		pd_entry = pd_entry + ino;
	} while (ino);
	printf("sending offset = 0x%08x \n",pd_entry);
	return pd_entry;
//#endif
	return 1;

}
static unsigned int create_d_entry (char *name, unsigned int p_inode, int type)
{
	unsigned int offset;
	unsigned int inode;

	offset = get_next_dentry_offset(p_inode);
	DB_PRINT("##########################\n");
	DB_PRINT("offset = 0x%08x \n",offset);
	inode = get_free_inode();	
	DB_PRINT("free inode num  = %d \n",inode);
	DB_PRINT("##########################\n");
	/**karthik Edited this code */

	return 1;
}
unsigned int create_file(char *name, unsigned int p_inode)
{
	DB_PRINT("inside Create File \n");
	return create_d_entry(name, p_inode, EXT2_FT_REG_FILE);
}
