#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vfs.h"

#define TOTAL_CMDS	6

void ls (char *);
void cat (char *);
void echo (char *);
void rm (char *);
void touch (char *);
void mkdir (char *);

extern void fs_init(void);

struct commands {
	char cmd[16];
	void (*cmd_func) (char *param);
	char param[64];
};

struct commands cmds[TOTAL_CMDS] = {
	{
		.cmd = "ls",
		.cmd_func = ls 
	},
	{
		.cmd = "cat",
		.cmd_func = cat
	},
	{
		.cmd = "echo",
		.cmd_func = echo 
	},
	{
		.cmd = "rm",
		.cmd_func = rm
	},
	{
		.cmd = "touch",
		.cmd_func = touch
	},
	{
		.cmd = "mkdir",
		.cmd_func = mkdir
	}
};

void hexdump (char line[])
{
	int len = strlen(line);
	int i = 0;
	
	printf ("hexdump .... \n");

	for (i = 0; i <= len; i++)
	{
		printf ("%02x ", line[i]);
	}
	printf ("\n");

	return;
}

void process_cmd(char cmd[], char param[])
{
	int i = 0;
	for (; i < TOTAL_CMDS; i++)
	{
		if (!(strcmp (cmds[i].cmd, cmd)))
			cmds[i].cmd_func(param);
	}
}
int main()
{
	char line[64];
	char cmd[64];
	char param[48];

	fs_init();
	vfs_init();

	while (1)
	{
		printf ("[GES %s]$ ", get_pwd_name());
		if (fgets (line, sizeof (line), stdin) == NULL){
		putchar('\n');
		exit (0);
		}

		if ( strlen (line) <= 1) continue;

		sscanf (line, "%s %48[^\n]", cmd, param);

		process_cmd(cmd, param);
		
		//printf (" cmd = %s %s \n", cmd, param);
		
	}	
}
