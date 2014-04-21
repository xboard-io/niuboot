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

#include "types.h"
#include "utils.h"
#include "main.h"
#include "init.h"
#include "gpmi.h"
#include "net.h"

extern void *get_heap_start(void);
CMD_FUNC_DEF( cmd_dm )
{
	const char usage[] = "dm - dm9000 interface debug\n"
			     "\tdm <reg_no> [val]\n";

	if(argc < 2) {
		puts(usage);
		return 0;
	}
	unsigned int reg_idx = simple_strtoul(argv[1],NULL,16);
	unsigned int reg_val;
	
	if(argc == 3)
	{
		reg_val = simple_strtoul(argv[2], NULL, 16);
		gpmi_dm9000_write_reg_index(reg_idx);
		gpmi_dm9000_write_data_bulk(&reg_val, 1);
		
	/*char buf[]={0x41,0x42,0x43,0x44,0x45,0x46};
	gpmi_dm9000_write_reg_index(reg_idx);
	reg_val = simple_strtoul(argv[18], NULL, 16);
	gpmi_dm9000_write_data_bulk(buf, 1);
	gpmi_dm9000_write_data_bulk(buf+1,1);
	gpmi_dm9000_write_data_bulk(buf+2,1);
	return 0;
	*/
	}
	//for(i=0;i<1000;i++)
	//while(1)
	{
		gpmi_dm9000_write_reg_index(reg_idx);
		gpmi_dm9000_read_data_bulk(&reg_val,1);
		printf("reg = %x\n", reg_val);
		//printf("reg = %x\n", gpmi_dm9000_read_reg(0x55));

		printf("flash-id:%x\n", gpmi_k9f1208_read_id());
	}
	return 0;
}

CMD_FUNC_DEF( cmd_test )
{
	const char usage[] = "sdramtest\n"
			; 
	if(argc < 1)
	{
		puts(usage);
		return 0;
	}
	//printf("argc=%x,%s,%s\n", argc, argv[0], argv[1]);
	if((argc>=3) && (argv[1][0]=='s'))
	{
		volatile unsigned int * dst= (unsigned int*) 0x40000000;
		unsigned int i, temp;	
		unsigned int val = simple_strtoul(argv[2],NULL,16);
		printf("word test\n");	
		for(i=0 ; i < 0x1000000; i++ )
		{
			dst[i]= val;
		}
		//verify
		for(i=0 ; i < 0x1000000; i++ )
		{
			if( (temp = dst[i])!=val ) 
				printf("bad add:%x, val:%x, shouldbe %x\n", &dst[i], temp, val);		

		}
		return 0;
	}
	if((argc>=2) && (argv[1][0]=='w'))
	{
		volatile unsigned int * src= (unsigned int*) 0xc0000000;
		volatile unsigned int * dst= (unsigned int*) 0x40000000;
		unsigned int temp;
		unsigned int i;	
		printf("word test\n");	
		for(i=0 ; i < 0x1000000; i++ )
		{
			dst[i]= src[i%(64*1024)];
		}
		//verify
		for(i=0 ; i < 0x1000000; i++ )
		{
			if((temp = dst[i]) != src[i%(64*1024)])
				printf("bad add:%x, val:%x, shouldbe %x\n", &dst[i], temp, src[i%(64*1024)]);		

		}
		return 0;
	}
	/* word random test */
	if ((argc >= 2) && (argv[1][0] == 'r'))	{
		volatile unsigned int *src= (unsigned int *) 0xc0000000;
		volatile unsigned int *dst= (unsigned int *) 0x40000000;
		unsigned int temp;
		unsigned int i;
		unsigned int count;

		count = 0x800000;
		printf("word random test\n");
		for(i=0 ; i < count; i++) {
			temp = random() % 0x1000000 - 4;
			//printf("temp = %x\n", temp);
			//dst[temp] = src[temp];
			dst[i] = src[i];
		}
		//verify
		printf("verify...\n");
		for(i=0 ; i < count; i++ )
		{
			if(dst[i] != src[i])
				printf("bad add:%x, val:%x, shouldbe %x\n", &dst[i], dst[i], src[i]);
		}
		return 0;
	}
	volatile unsigned short * add = (unsigned short*)0x40000000;
	unsigned int i,j;
	unsigned int temp;
	unsigned short pat[8] =
	{
		0xaa, 0xaf1b, 0x99, 0xa603, 
		0xcf, 0xbe, 0x92, 0x2d,
		
	/*	0xff,0x00,0xff,0xff,
		0xff,0xff,0xff,0xff
		*/
	};
	for(i=0; i<64*1024*1024/16; i++, add+=8)
	{
		add[0] = pat[0];		
		add[2] = pat[2];
		add[4] = pat[4];
		add[5] = pat[5];
		add[1] = pat[1];
		add[7] = pat[7];
		add[6] = pat[6];
		add[3] = pat[3];

		for(j=0; j<8; j++)
			if((temp = add[j]) != pat[j])
			{
				printf("bad add:%x, add[%x]=%x, shouldbe %x\n", &add[j], j, temp, pat[j]);		
			//	return 0;
			}
	}
	return 0;
}

	
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
		{"sdramtest",cmd_test},
		{"ping", cmd_ping},
		{"dm", cmd_dm}

		
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

/*input:
	cmd: the cmd str to split into pieces by space 
	n: the lenth of argv
output:
	argv: the pointer to an str array to store the pieces str
return:
	the number of pieces str.
*/
int cut_cmd(char *cmd, int argv_len, char* argv[]) 
{
	int i,cnt;
	int str_len = strlen(cmd);

	for( i=0; i<str_len; i++ ) 
	{
		if(cmd[i]==' ') 
			cmd[i] = '\0';
	}
	for( i=0,cnt=0; (i<str_len) && (cnt<argv_len); i++ )
	{
		if(cmd[i])
		{
			argv[cnt++] = cmd+i;
			i += strlen(cmd+i);
		}
	}
	return cnt;
}

//extern void beep(void);
int main(void) 
{
//	volatile char *ram_addr = ((volatile char *)0x40000000);
	
	int i;
   	int param_cnt; 
	char * cmd_buf = (char*)get_heap_start;	//compiler variable
	char * cmd_params[8];

	init_soc(MCIMX233);

	puts("\n\n\t--NiuBoot v0.9--\n(C) CFFHH Open Embedded Org. 2011\n\tDistributed Under GPLv3\n\n");
	for(;;) /*{ beep(); }*/
/*	{
		puts("a");
		mdelay(100);
	}
*/
	{
		puts("NIUBOOT# ");
		get_cmd(cmd_buf);
		puts("\n");
		param_cnt = cut_cmd(cmd_buf, sizeof cmd_params / sizeof (char*), cmd_params);
		
		if(param_cnt)
		{
			for(i=0; i<sizeof cmd_list / sizeof(CMD); i++)
				if( strcmp( cmd_params[0], cmd_list[i].name) == 0 )
				{
					cmd_list[i].func( param_cnt,(const char* const*) cmd_params );
					break;
				}
			if( i == sizeof cmd_list / sizeof(CMD) )
				puts("Wrong Command. ? for help\n");
		}
	}
	
}

CMD_FUNC_DEF (cmd_help)
{
	const char usage[] = "help - print this\n" "? - alias of help\n";
	if(argc < 0)
	{
		if(strcmp("help", argv[0])==0)
			puts(usage);
		return 0;
	}
	int i;
	puts("\n");
	for( i = 0; i<sizeof cmd_list / sizeof(CMD); i++)
	{
		cmd_list[i].func( -1, &cmd_list[i].name );
	}
	return 0;
		
}
CMD_FUNC_DEF( cmd_mem )
{
	const char usage[] = "mem - show num of data from specified address\n"
			"\tmem <add(hex)> [num=1]\n"
			"* - alias of mem\n";
	if(argc < 2)
	{
		if( argc>0 ||  strcmp( "mem", argv[0] )==0 )
			puts(usage);
		return 0;
	}
	volatile unsigned int *addr = (unsigned int*) (~0x3 & simple_strtoul( argv[1], NULL, 16));
	unsigned int i, num = 1;
	if( argc > 2 ) 
		num = simple_strtoul(argv[2], NULL, 16);	

	for( i=0; i<num; i++, addr++)	
	{
		if( (i&0x3)==0 ) 
			printf("\n%x\t", addr);
		printf("%x ", *addr); 
	}
	puts("\n");
	return 0;

}
CMD_FUNC_DEF( cmd_word )
{
	const char usage[] = "word  - set value to specified address\n"
			"\t<add(hex)> <value(hex)>\n"
			"@ - alias of word\n";	
	if(argc < 3) 
	{
		if( strcmp( "word", argv[0] )==0 )
			puts(usage);
		return 0;
	}
	volatile unsigned int *addr = (unsigned int*) (~0x3 & simple_strtoul( argv[1], NULL, 16));
	unsigned int val = simple_strtoul(argv[2], NULL, 16);	
	*addr = val;
	cmd_mem(2, argv);
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
	const char usage[] = "ping - send ARP to host machine\n"
			"\tping <ipv4>\n" ;
	int i;

	if (argc < 2) {
		puts(usage);
		return 0;
	}
#if 1
	char *ip_str = (char*)argv[1];

	IPV4 ip = {{0,0,0,0}};
	for(i=0; i<4; i++, ip_str++)
	{
		while(*ip_str!='.')
		{
			if(!*ip_str)
			{
				if(i<3)
					i=5;
				break;
			}
			else if( (*ip_str-='0')>=0 && *ip_str<=9 )
			{
				ip.b[i] *= 10;
				ip.b[i] += *ip_str;
			}
			else
			{
				i=100;
			}
			ip_str++;
		}
	}
	if(i!=4)
	{
		printf("invalid ip address:%x.%x.%x.%x\n",ip.b[0],ip.b[1],ip.b[2],ip.b[3]);
		return 0;
	}
		if( ping(ip) ) 
	{
		printf("remote no ack\n");
	}	
	else
		printf("remote is alive\n");
	
#else
	unsigned int reg_idx = simple_strtoul(argv[1],NULL,16);
	unsigned int reg_val;
	if(argc == 3)
	{
		reg_val = simple_strtoul(argv[2], NULL, 16);
		gpmi_dm9000_write_reg_index(reg_idx);
		gpmi_dm9000_write_data_bulk(&reg_val, 1);
	}
	//for(i=0;i<1000;i++)
	//while(1)
	{
		gpmi_dm9000_write_reg_index(reg_idx);
		gpmi_dm9000_read_data_bulk(&reg_val,1);
		printf("reg = %x\n", reg_val);
		//printf("reg = %x\n", gpmi_dm9000_read_reg(0x55));

		printf("flash-id:%x\n", gpmi_k9f1208_read_id());
	}
#endif
	return 0;
}

CMD_FUNC_DEF( cmd_tftp )
{
	const char usage[] = "tftp - download specified file from host to specified memory address\n"
			"\ttftp <host> <file_name> <add(hex)>\n";
	if(argc < 1)
	{
		puts(usage);
		return 0;
	}
	return 0;
}
extern void init_taglist(int);
CMD_FUNC_DEF( cmd_go )
{
	const char usage[] = "go - change PC to add or booting linux kernel with tag_list\n"
			"\tgo <add(hex)> [tag_list]\n"; 
	if(argc < 1)
	{
		puts(usage);
		return 0;
	}
	/*while(1)
	{
		*((unsigned short *)0x402298a4) = 0xaaaa;
	}
	int block=simple_strtoul(argv[1],NULL,16);
	int page=simple_strtoul(argv[2],NULL,16);
	char *buf = (char*)simple_strtoul(argv[3],NULL,16);
	gpmi_k9f1208_read_page(block,page,buf);
	*/

	int block=simple_strtoul(argv[1],NULL,16);
	init_taglist(block);
	printf("ok\n");

	return 0;
}

CMD_FUNC_DEF( cmd_xmodem )
{
#if 0
	const char usage[] = "xmodem - download file to sdram via x-modem protocal\n"
			"\txmodem <add(hex)> \n"; 
	if(argc < 2)
	{
		putsy(usage);
		return 0;
	}
	/*while(1)
	{
		*((unsigned short *)0x402298a4) = 0xaaaa;
	}*/
	int addr = simple_strtoul(argv[1],NULL,16);
	int i,c;
	//send
#define SOH 0x01
#define EOT 0x04
#define ACK 0x06
#define NAK 0x15
#define CAN 0x18
	for( i=0; i<5; i++)
	{
		udelay(1000);
		putchar('0'+i);
	}
	putchar(NAK);
	while( (c = getchar()) == SOH )
	{
		getchar();
		getchar();
		//for( i=0; i<
		*addr++ = getchar();	
	}
	return 0;
#endif
}
/*
 * Linear congruential random number generator
 */
unsigned long random() 
{
	static unsigned long rand_seed = 47;
	unsigned long rand_a = 48271L;
	unsigned long rand_m = 2147483647L;
	unsigned long rand_q = rand_m / rand_a;
	unsigned long rand_r = rand_m % rand_a;
	double d;
	long tmp_seed;
 
	tmp_seed = rand_a * (rand_seed % rand_q) - rand_r * (rand_seed / rand_q); 
	if(tmp_seed >= 0) 
		rand_seed = tmp_seed; 
	else 
		rand_seed = tmp_seed + rand_m; 
	d = (double)rand_seed / rand_m; 

	return (unsigned long)(d * 100000000);
}

