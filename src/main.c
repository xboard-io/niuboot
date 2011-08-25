/*
 * NiuBoot c main entry code 
 *
 * Orson Zhai <city2008@gmail.com>
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

#include "regs_imx233.h"
#include "serial.h"
#include "utils.h"

static PINCTRL * const pinctrl = (PINCTRL*) REGS_PINCTRL_BASE_PHYS;
#define hw_pinctrl (*pinctrl) 
static CLKCTRL * const clkctrl = (CLKCTRL*) REGS_CLKCTRL_BASE_PHYS;
#define hw_clkctrl (*clkctrl)
static GPMI * const gpmi = (GPMI*) REGS_GPMI_BASE_PHYS;
#define hw_gpmi (*gpmi)


extern void *get_heap_start(void);

void gets(char *cmd)
{
	do
	{
		serial_putc(*cmd++ = serial_getc());
	}while(cmd[-1]!='\r');
	cmd[-1]=0;
}

int main(void) 
{
	volatile char *ram_addr = ((volatile char *)0x40000000);
	int i;
   /*we use bank1-pin26,27 (duart_tx&rx)as LED flash*/
    	hw_pinctrl.muxsel[3].set = 0xf00000; 
	
    	hw_pinctrl.dout[1].set = 3<<26;
	hw_pinctrl.doe[1].set = 3<<26;

	for(i=0;i<3;i++)
    {
	hw_pinctrl.dout[1].set = 3<<26;

      mdelay(0x50000);

	hw_pinctrl.dout[1].clr = 3<<26;

      mdelay(0x50000);
    }

// Set up writing (AND READING!) of the serial port.
	serial_init();

    
	char * cmd_buf = (char*)get_heap_start;	//compiler variable
	serial_puthex((unsigned int)cmd_buf);
	serial_puts("\n");
	serial_puts("1.starting mux-on gpmi pins, press any key to continue..\n");
	hw_pinctrl.muxsel[0].clr = 0xffff; //open gpmi_d0 - d7
	hw_pinctrl.muxsel[1].clr = 0x000fc3cf;//open r/w wp, ready0,1, ALE, CLE
	hw_pinctrl.muxsel[5].clr = 0x03c00000; //opne ce0/1
	
	//set gpmi clk
	hw_clkctrl.gpmi &= ~(1<<31); //xtal clk 24Mhz to GPMI
//	hw_clkctrl.clkseq 
	//while(1);
	while(1)
	{
		serial_puts("NiuBoot#");
		gets(cmd_buf);
		serial_puts("\nyou enterred:");
		serial_puts(cmd_buf);
		serial_puts("\n");
	}
}

