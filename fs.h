
#ifndef _FS_H_
#define _FS_H_

#define EXT2_N_BLOCKS	1
#define BLOCK_SIZE    1024
#define INODE_SIZE  64
#define BITS_PER_BYTE	8
#define BYTES_PER_BLOCK (BLOCK_SIZE / 1)
#define WORDS_PER_BLOCK (BLOCK_SIZE / 4)
#define WORD_SIZE 4

/* EXT2 file types */
enum { 
	EXT2_FT_UNKNOWN = 0,
	EXT2_FT_REG_FILE = 1,
	EXT2_FT_DIR = 2,
	EXT2_FT_MAX
};

typedef unsigned int _u32;
typedef unsigned short _u16;
typedef unsigned char _u8;

struct ext2_super_block
{
	_u32 s_inodes_count;
	_u32 s_blocks_count;
	_u32 s_free_block_count;
	_u32 s_free_inode_count;
	_u32 s_first_data_block;
	_u32 s_blocks_per_group;
	_u32 s_inodes_per_group;
	_u32 s_first_ino;
	_u32 s_inode_size;

}__attribute__((aligned(BLOCK_SIZE)));

struct ext2_blk_grp_des
{
	_u32 free_inodes;

}__attribute__((aligned(BLOCK_SIZE)));

struct ext2_dir_entry_2 {
    _u32  inode;          /* Inode number */
    _u16  rec_len;        /* Directory entry length */
    _u8   name_len;       /* Name length */
    _u8   file_type;
    char  name[];         /* File name, up to EXT2_NAME_LEN */
};

struct ext2_disk_inode
{
    _u16  i_mode;     /* File mode */
    _u16  i_uid;      /* Low 16 bits of Owner Uid */
    _u32  i_size;     /* Size in bytes */
    _u32  i_atime;    /* Access time */
    _u32  i_ctime;    /* Creation time */
    _u32  i_mtime;    /* Modification time */
    _u32  i_dtime;    /* Deletion Time */
    _u32  i_blocks;   /* Blocks count */
    _u32  i_flags;    /* File flags */
	_u32  i_block[EXT2_N_BLOCKS]; /* Pointers to blocks */
	
}__attribute__((aligned(INODE_SIZE)));


#endif

