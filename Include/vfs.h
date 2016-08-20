
#ifndef __VFS_H
#define __VFS_H

#define NAME_LEN	256

#include <time.h>

typedef unsigned int _u32;
typedef unsigned short _u16;
typedef unsigned char _u8;

struct inode {
	_u16 i_mode;
	unsigned long i_no;
	_u32 i_size;
	_u32 blocks;
    struct timespec  i_atime;
    struct timespec  i_mtime;
    struct timespec  i_ctime;   

	struct inode *i_sibling;
	struct dentry *i_dentry; // Associated dentry
	struct dentry *p_dentry; // Parent dentry
};

struct list_head {
	struct inode *i_sibling;
};

struct dentry {

	char d_name[NAME_LEN];
	struct dentry *d_parent;
	struct dentry *d_subdirs;// Holds all the subdirs
	struct dentry *d_sibling; // Holds all the siblings
	struct inode *d_inode;
	struct list_head d_inodes;
};

char *get_pwd_name(void) ;
void vfs_init(void);
void get_root_inode_info(struct inode *in);
unsigned int create_file(char *name, unsigned int p_inode);


#endif

