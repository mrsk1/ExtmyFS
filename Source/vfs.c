#include <stdio.h>
#include <string.h>
#include "vfs.h"

static struct dentry r_dentry;
static struct inode r_inode;
static struct inode *pwd_inode;
char pwd_name[NAME_LEN];

void ls (char param[])
{
	printf ("ls command \n");
	return ;
}

void cat (char param[])
{
	printf ("cat command \n");
	printf ("parameters = %s \n", param);
	return ;
}

void echo (char param[])
{
	printf ("echo command \n");
	printf ("parameters = %s \n", param);
	return ;
}

void rm (char param[])
{
	printf ("rm command \n");
	printf ("parameters = %s \n", param);
	return ;
}


void mkdir (char param[])
{
	printf ("mkdir command \n");
	printf ("parameters = %s \n", param);
	return ;
}
 
void touch (char param[])
{
	printf ("touch command \n");
	printf ("parameters = %s \n", param);
	create_file("myfs", 1);
	return ;
}

void init_root_dentry(void)
{
	strcpy(r_dentry.d_name, "/");
	r_dentry.d_parent = &r_dentry;
	r_dentry.d_sibling = &r_dentry;
	r_dentry.d_inode = &r_inode;
	r_dentry.d_subdirs = NULL;
	r_dentry.d_inodes.i_sibling = NULL;

	return ;
}

void init_root_inode (void)
{
	get_root_inode_info(&r_inode);
	r_inode.i_dentry = &r_dentry;
	r_inode.p_dentry = NULL; // If this is a inode of a directory

	return;
}

void vfs_init(void)
{
	init_root_dentry();
	init_root_inode();
	pwd_inode = &r_inode;
	strcpy(pwd_name, r_dentry.d_name);

	return;	
}

char *get_pwd_name(void)
{
	return pwd_name;
}
