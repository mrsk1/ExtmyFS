#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "fs.h"
#include "vfs.h"

#define PAGE_ALIGN	0x1000
#define DEV_SIZE	(1 * 1024 * 1024)
#define PRINTFN   (printf("inside %s\n",__func__))

static struct ext2_super_block *ext2_sb;
static struct ext2_blk_grp_des *ext2_bgd;
//static struct ext2_dir_entry_2 *ext2_dentry;
static struct ext2_disk_inode (*ext2_inode)[BLOCK_SIZE * BITS_PER_BYTE]; 
static void * ext2_inode_bit_map;
static void * ext2_data_block_bit_map; 
static void *first_data_block; 
static void *last_data_block;
unsigned int first_data_blk_no;

static inline struct ext2_dir_entry_2 * get_next_dentry(struct ext2_dir_entry_2 *entry)
{
	return (struct ext2_dir_entry_2 *) ( entry + entry->rec_len);
}
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
	printf ("Address of dev_mem			= 0x%08x \n", (unsigned int)(uintptr_t)dev_mem);
	printf ("Address of ext2_sb			= 0x%08x \n", (unsigned int)(uintptr_t)ext2_sb);
	printf ("Address of ext2_bgd 			= 0x%08x \n",(unsigned int)(uintptr_t)ext2_bgd);
	printf ("Address of ext2_data_block_bit_map	= 0x%08x \n", (unsigned int)(uintptr_t)ext2_data_block_bit_map);
	printf ("Address of ext2_inode_bit_map		= 0x%08x \n", (unsigned int)(uintptr_t)ext2_inode_bit_map);
	printf ("Address of ext2_inode			= 0x%08x \n", (unsigned int)(uintptr_t)ext2_inode);
	printf ("Start of data blocks			= 0x%08x \n", (unsigned int)(uintptr_t)first_data_block);
	printf ("Last data block 			= 0x%08x \n", (unsigned int)(uintptr_t)last_data_block);

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
#ifdef WRONGLOGIC
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
#endif
static int init_data_block(void)
{
	//unsigned int *bmap = first_data_block;
	memset(first_data_block,'\0',BLOCK_SIZE);
	return 0;
}


static void init_ext2(void)
{
	unsigned int *bmap;

	ext2_sb = (struct ext2_super_block *) dev_mem;
	ext2_bgd = (struct ext2_blk_grp_des *) (ext2_sb + 1);

	ext2_data_block_bit_map = ext2_bgd +1;
	bmap = ext2_data_block_bit_map;
	*bmap |= 0x1; // Reserving the first bit of data blk bit map

	ext2_inode_bit_map = ext2_data_block_bit_map + BLOCK_SIZE;
	bmap = ext2_inode_bit_map;
	*bmap |= 0x1; // Reserving the first bit of inode bit map
	NOPRINT("address of  inode bit_map = 0x%08x \n",ext2_inode_bit_map);
	NOPRINT(" *bmap value = %d \n",*bmap);	
#if 0
	ext2_inode_bit_map = ext2_bgd + 1;
	bmap = ext2_inode_bit_map;
	*bmap |= 0x1; // Reserving the first bit of inode bit map
	DB_PRINT("address of  inode bit_map = 0x%08x \n",ext2_inode_bit_map);
	DB_PRINT(" *bmap value = %d \n",*bmap);	

	ext2_data_block_bit_map = ext2_inode_bit_map + BLOCK_SIZE;
	bmap = ext2_data_block_bit_map;
	*bmap |= 0x1; // Reserving the first bit of data blk bit map

#endif
	ext2_inode = (struct ext2_disk_inode (*)[])(ext2_inode_bit_map + BLOCK_SIZE);

	first_data_block = ext2_inode + 1;
	printf ("first data block = 0x%08x \n", (unsigned int )(uintptr_t )first_data_block);
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

	NOPRINT ("block_addr = 0x%08x \n", (unsigned int)(uintptr_t)block_addr);
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

unsigned int set_free_bit(void *block_addr, unsigned int pos)
{
	unsigned int *bit_map = block_addr;
	unsigned int bit_position = 0;
	unsigned int *bmap;
	unsigned int bit_pos = 0;
	// go to the pos of blockaddr and clear the bit
	bit_position = pos / 32;
	bit_pos = pos % 32 ;
	DB_PRINT("bit_move %d \n",bit_position);
	DB_PRINT("bit_pos %d \n",bit_pos);
	bmap = bit_map + bit_position;
	DB_PRINT("before 0x%08x \n",*bmap);
	*bmap &= (~(1 << bit_pos));
	DB_PRINT("after  0x%08x \n",*bmap);
	return 0;
}
unsigned int set_free_data_block(unsigned int pos)
{
	return set_free_bit(ext2_data_block_bit_map, pos);
}


unsigned int set_free_inode(unsigned int pos)
{
	return set_free_bit(ext2_inode_bit_map, pos);
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
//static unsigned int  get_next_dentry_offset(unsigned int in)
static unsigned int * get_next_dentry_offset(unsigned int in)
{
	struct ext2_disk_inode *inode = (struct ext2_disk_inode *) ext2_inode;
	unsigned int blk_no;
	unsigned char *rec;

	blk_no = inode[in].i_block[0];
	NOPRINT("in =%d blk no = %d \n ",in, blk_no);
	//	rec = dev_mem + (BLOCK_SIZE * blk_no);
	rec = first_data_block + (BLOCK_SIZE * blk_no);
	struct ext2_dir_entry_2 *pd_entry;
	unsigned int ino;
	pd_entry = (struct ext2_dir_entry_2 *)rec ;
	do {
		ino = pd_entry->rec_len;
//		printf(" ino = %d \n",ino);
		pd_entry = pd_entry + ino;
	} while (ino);
	return (unsigned int *)pd_entry;

}
static int fill_inode_details (unsigned int pos, int type)
{
	struct ext2_disk_inode *inode = (struct ext2_disk_inode *) ext2_inode;
	if (pos < 0)
		return -1;

	inode[pos].i_atime = inode[pos].i_ctime = inode[pos].i_mtime = time(NULL);
	inode[pos].i_size = BLOCK_SIZE;
	inode[pos].i_blocks = 1;
	inode[pos].i_block[0] = get_free_data_block();
	inode[pos].i_mode = type ;
	inode[pos].i_flags = type  ;
	/** TODO: i_flags need to check how to update */
	NOPRINT ("Free data block = %u \n", inode[pos].i_block[0]);

	return 1;
}
static unsigned int create_d_entry (char *name, unsigned int p_inode, int type)
{
	unsigned int *offset;
	unsigned int inode = 0;
	unsigned short flength = 0;
	struct ext2_dir_entry_2  *d_entry;
	/** validate parameters */
	if (NULL == name)
		return -1;
	if (p_inode < 0)
		return -1;
	offset = get_next_dentry_offset(p_inode);
//	printf("offset = 0x%08x \n",offset);
	/**  go to the d_entry and fill the filename and record length */
	d_entry = (struct ext2_dir_entry_2 *) offset;
	flength = strlen(name);
	d_entry->name_len = flength;
	d_entry->rec_len = sizeof(* d_entry) + flength;
	d_entry->file_type = type;
	inode = get_free_inode();	
//	printf("free inode num  = %d \n",inode);
	d_entry->inode = inode;
	memcpy(d_entry->name,name,flength);
	/** get inode num then fill the file details */
	if (fill_inode_details(inode, type) < 0)
		return -1;

	return 1;
}
unsigned int create_file(char *name, unsigned int p_inode) {
	return create_d_entry(name, p_inode, EXT2_FT_REG_FILE);
}


unsigned int list_dentry(unsigned char * index)
{
	char name[EXT2_NAME_LEN] = {0};
	int length ;
	struct ext2_dir_entry_2 *d_entry = (struct ext2_dir_entry_2 *) index;
	if (d_entry->rec_len == 0){
		DB_PRINT("directroy is empty \n");
		return 0;
	}else {
		while ((length = d_entry->rec_len) != 0){
			memset(name,'\0',EXT2_NAME_LEN);
			memcpy(name,d_entry->name ,d_entry->name_len);
			DB_PRINT("%d\t%s \n",d_entry->inode, name);
			d_entry = get_next_dentry(d_entry);
		}
	}
	return 1;
}
unsigned int list_files(unsigned int p_inode)
{
	unsigned short i = 0;
	unsigned short dblk_index = 0;
	unsigned char * rec ;
//	PRINTFN;
	if(p_inode < 0)
		return -1;
	struct ext2_disk_inode *inode = 
		(struct ext2_disk_inode *) ext2_inode + p_inode;
	for( i = 0; i < inode->i_blocks ; i++){
		dblk_index = inode->i_block[i];	
		NOPRINT("dblk_index = %d \n",dblk_index);
		rec = first_data_block + (BLOCK_SIZE * dblk_index);   
		list_dentry(rec);
	}
	return 0;
}
unsigned int find_dentry(unsigned char * rec, char *name)
{
//	char name[EXT2_NAME_LEN] = {0};
	int length ;
	struct ext2_dir_entry_2 *d_entry = (struct ext2_dir_entry_2 *) rec;
	if (d_entry->rec_len == 0){
		return 0;
	}else {
		while ((length = d_entry->rec_len) != 0){
			if (strncmp(name,d_entry->name,d_entry->name_len) == 0){
				return d_entry->inode ;
				break;
			}
			d_entry = get_next_dentry(d_entry);
		}
	}

	return 0;
}

unsigned int delete_dentry(unsigned char * rec, char *name)
{
	PRINTFN;
	int length ;
	struct ext2_dir_entry_2 * tmp;
	struct ext2_dir_entry_2 * prev;
	struct ext2_dir_entry_2 *d_entry = (struct ext2_dir_entry_2 *) rec;
	if (d_entry->rec_len == 0){
		return 0;
	}else {
		while ((length = d_entry->rec_len) != 0){
			DB_PRINT(" length = %d \n",length);
			//TODO: need to take care delete at first file name
			if (strncmp(name,d_entry->name,d_entry->name_len) == 0){
				prev->rec_len = prev->rec_len + d_entry->rec_len;
				break;
			}
			prev = d_entry;
			d_entry = get_next_dentry(d_entry);
		}
	}
	return 0;
}

int remove_file(char *name, unsigned int p_inode)
{
	unsigned short i = 0;
	unsigned short dblk_index = 0;
	unsigned char * rec ;
	unsigned int inum;
	PRINTFN;
	if (NULL == name){
		DB_PRINT("no file name !!\n");
		return -1;
	}
	if(p_inode < 0)
		return -1;

	struct ext2_disk_inode *inode = 
		(struct ext2_disk_inode *) ext2_inode + p_inode;
	for( i = 0; i < inode->i_blocks ; i++){
		DB_PRINT("inode->i_blocks = %d \n", inode->i_block[i]);
		dblk_index = inode->i_block[i];	
		DB_PRINT("dblk_index = %d \n",dblk_index);
		rec = first_data_block + (BLOCK_SIZE * dblk_index);   
		inum =find_dentry(rec,name);
		}
		if(0 == inum){
			DB_PRINT("file not found\n");
		}else{
			DB_PRINT("file found and inode num = %d \n",inum);
		}
		// go to inode process the data blocks and memset it and clear dbm
		inode = (struct ext2_disk_inode *) ext2_inode + inum;
		for (i = 0 ;i < inode->i_blocks ; i++){
			set_free_data_block(inode->i_block[i]);
		}
		// rm inode details and clear ibm
		set_free_inode(inum);
		delete_dentry(rec,name);


	return 0;
}

