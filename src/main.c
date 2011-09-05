/*
 * NiuBoot c main entry code 
 *
 * Orson Zhai <orsonzhai@gmail.com>
 *
 * Copyright 2011 Orson Zhai, Beijing, China.
 * Copyright 2011 CFFHH Open Embedded Org. 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "serial.h"
#include "utils.h"
#include "main.h"

extern void *get_heap_start(void);
static const CMD cmd_list[] = 
{
		{"help", cmd_help },
		{"?",    cmd_help },
		{"mem",  cmd_mem },
		{"*",    cmd_mem },
		{"word", cmd_word },
		{"@",    cmd_word },
		{"conf", cmd_config },
		{"ping", cmd_ping },
		{"tftp", cmd_tftp },
		{"go",   cmd_go },
		
};

//return 0 for success, a pointer for re-type string
//*************
//move those code to terminal.c when get more big
#define BS '\b'
#define ESC 0x1B//'\e'
#define CTRL_C 0x3
#define TAB '\t'
#define SPACE ' ' 
#define ENTER '\r'
#define NEXT '\n'
#define UP	0x11
#define DOWN	0x12
#define LEFT	0x14
#define RIGHT	0x13
int term_getchar(void)
{
	int ch = getchar(); //standard input : serial in this case
	if(ch == ESC) //escape sequence
	{
		int mode = 1;
		while(mode)
			switch(ch = getchar())
			{
				case '[':
					mode = 2;
					break;
				case 'A':
				case 'B':
				case 'C':
				case 'D':
					if(mode==2)
					{//up
						mode = 0;
						ch = ch - 'A' + UP;
					}
					else
						mode = 0;
					break;
				default:
					ch |= ESC<<8;
					mode = 0;
			}
	}
	return ch;
}
//*************

char* get_cmd(char *cmd) //accept ctrl-c, bs, tab, ESC sequence ... up , down , left , right
{
	char ch;
	int pos = 0, i;
	char *last_par = cmd;
	const char zero8[] = "00000000";
	static char history[256]= {0};
	static unsigned char ring = 0;
	unsigned char moving =  ring-1 ;	//pointing to the tail of string '\0'

	for(;;)
		switch( ch = term_getchar() )
		{
			case BS:
				if(pos) 
				{
					pos--;
					puts("\b \b");
				}
				break;
			
			case TAB:
				cmd[pos]='\0';

				for(i=0; i< sizeof cmd_list / sizeof (CMD); i++)
				{
					if(strncmp(last_par, cmd_list[i].name, strlen(last_par)) == 0)
					{
						//finish cmd
						strcat( last_par, puts( cmd_list[i].name + strlen(last_par) ) );	
						break;
					}
				}
				if( i == sizeof cmd_list / sizeof (CMD) )
				{
					strcat( last_par, puts( zero8 + strlen(last_par) ) );
				}
				pos = strlen(cmd);
				break;
					
			case UP:
				for(i=0; i<pos; i++)
					puts("\b \b");		//clear current cmd literal string

				while( history[(--moving)&0xff] )	//find previous cmd
					;	 		//a little bit dangous?
				for( pos=0; history[(pos+moving+1)&0xff] != '\0'; pos++ )
				{
					putchar(cmd[pos] = history[(pos+moving+1)&0xff]);
				}
				break;

			case DOWN:
				for(i=0; i<pos; i++)
					puts("\b \b");

				for( pos=0, moving++; history[moving&0xff] != '\0'; pos++,moving++ )
				{
					putchar(cmd[pos] = history[moving&0xff]);
				}
				break;

			case CTRL_C:
				puts("^C");
				pos = 0;
			case ENTER:
			case NEXT:
				cmd[pos] = '\0';
				for(i=0; i<=pos; i++, ring++)
				{
					history[ring&0xff] = cmd[i];
				}
				return NULL;

			case SPACE:
				last_par = cmd + pos + 1;
			default:
				cmd[pos++] = ch;
				putchar(ch); //echo to console 
				break;
		}
}

int main(void) 
{
//	volatile char *ram_addr = ((volatile char *)0x40000000);
	
	int i;
    
	char * cmd_buf = (char*)get_heap_start;	//compiler variable
	//serial_puthex((unsigned int)cmd_buf);
	//serial_puts("\n");
/*	i = 0x40000000;
	while(i+=4)
	{
		serial_puthex(i);	
	}
*/

	serial_init();
	for(;;)
	{
		puts("NiuBoot#");
		get_cmd(cmd_buf);
		serial_puts("\nyou enterred:");
		serial_puts(cmd_buf);
		serial_puts("\n");
	}
	
}

CMD_FUNC_DEF (cmd_help)
{
	const char usage[] = "help - print this\n" "? - alias of help\n";
	if(argc < 1)
	{
		puts(usage);
		return 0;
	}
	int i;
	for( i = 0; i<sizeof cmd_list / sizeof(CMD); i++)
	{
		cmd_list[i].func( -1, &cmd_list[i].name );
	}
	return 0;
		
}
CMD_FUNC_DEF( cmd_mem )
{
	const char usage[] = "mem <add(hex)> [num=1] - show num of data from specified address\n"
			"* - alias of mem\n";
	if(argc < 2)
	{
		if( strcmp( "mem", argv[0] )==0 );
			puts(usage);
		return 0;
	}
	return 0;

}
CMD_FUNC_DEF( cmd_word )
{
	const char usage[] = "word <add(hex)> <value(hex)> - set value to specified address\n"
			"@ - alias of word\n";	
	if(argc < 3) 
	{
		if( strcmp( "word", argv[0] )==0 );
			puts(usage);
		return 0;
	}
	return 0;
}
CMD_FUNC_DEF( cmd_config )
{
	const char usage[] = "config - show or set run-time configuration items of NiuBoot\n";
	if(argc < 1)
	{
		puts(usage);
		return 0;
	}
	return 0;
}
CMD_FUNC_DEF( cmd_ping )
{
	const char usage[] = "ping <ipv4> - send ARP to host machine\n";
	if(argc < 1)
	{
		puts(usage);
		return 0;
	}
	return 0;
}
CMD_FUNC_DEF( cmd_tftp )
{
	const char usage[] = "tftp <host> <file_name> <add(hex)> - download specified file from host to specified memory address\n";
	if(argc < 1)
	{
		puts(usage);
		return 0;
	}
	return 0;
}

CMD_FUNC_DEF( cmd_go )
{
	const char usage[] = "go <add(hex)> [tag_list] - change PC to add or booting linux kernel with tag_list\n";
	if(argc < 1)
	{
		puts(usage);
		return 0;
	}
	return 0;
}

