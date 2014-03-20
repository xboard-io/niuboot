
#include "regs_imx233.h"
#include "init.h"
#include "serial.h"
#include "dm9000x.h"

static PINCTRL * const pinctrl = (PINCTRL*) REGS_PINCTRL_BASE_PHYS;
#define hw_pinctrl (*pinctrl) 
static CLKCTRL * const clkctrl = (CLKCTRL*) REGS_CLKCTRL_BASE_PHYS;
#define hw_clkctrl (*clkctrl)
static GPMI * const gpmi = (GPMI*) REGS_GPMI_BASE_PHYS;
#define hw_gpmi (*gpmi)
static APBH * const apbh = (APBH*) REGS_APBH_BASE_PHYS;
#define hw_apbh (*apbh)
static POWER * const power = (POWER*) REGS_POWER_BASE_PHYS;
#define hw_power (*power)
static EMI * const emi = (EMI*) REGS_EMI_BASE_PHYS;
#define hw_emi (*emi)
static DRAM * const dram = (DRAM*) REGS_DRAM_BASE_PHYS;
#define hw_dram (*dram)
static DIGCTL * const digctl = (DIGCTL*) REGS_DIGCTL_BASE_PHYS;
#define hw_digctl (*digctl)


void udelay(unsigned int n) //delay n micro-seconds by digctrl
{
	unsigned int start = hw_digctl.microseconds.dat;
	while( hw_digctl.microseconds.dat < (start + n) ); 
} 

void mdelay(unsigned int n) 
{ 
	int i; 
	for(i=0; i<n; i++)
		udelay(1000);
}

//init_soc is to init the core functions of SOC chipset ,such as clock, dma, serial line, pin-ctrl mode, etc.
int init_soc(int soc_type)
{
	switch(soc_type)	
	{
		case MCIMX233:
			init_all_pins(); //base on board layout
			
			init_clock_power();
			//init_dma();

			serial_init(); //heading by serial_ means this function is from serial.c
	//		init_sdram();
			//gpmi_init(); //heading by gpmi_ means this function is from gpmi.c
//			dm9000_initialize();
			break;
	}
	return 0;
}
#if 0	
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
	//serial_init();
/*hw_pinctrl.dout[0].set = 1<<17;	
hw_pinctrl.dout[2].set = 1<<27;
hw_pinctrl.doe[0].set = 1<<17;
hw_pinctrl.doe[2].set = 1<<27;
*/
	//hw_pinctrl.drive[0].dat = 0x22222222;
	//hw_pinctrl.drive[1].dat = 0x22222222;
	//hw_pinctrl.drive[2].dat = 0x22222222;
	hw_pinctrl.pull[0].set = 0xff;
	//set gpmi clk
		udelay(0x100);
	hw_gpmi.ctrl0.clr = 1<<30; //open gate source
	hw_apbh.ctrl[0].clr = 1<<30; 

	volatile unsigned int dev_regno;
	volatile char dev_regval[8];//=0;
	
/*DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMAAAAAAAAAAAAAAAAAAAAA*/
/*	DMA_CMD reg_read[2] =
	{
		{
		&reg_read[1],//next command
		2,//command: 
		1,//:chain
		0,//irq_complete
		0,//nandlock
		0,//nandwait4ready
		0,//decrement_semaphore
		0,//wait4endcmd
		0,//un-used
		1, //piowords_cnt
		2, //xffer_cnt
		(unsigned int*)&dev_regno,
		{0x00c30002}
		},
		{
		0,//next command address

		1,//command, 01 for dma write
		0,//chain
		0,//irq_complete
		0,//nandlock
		0,//nandwait4ready
		1,//decrement semaphore
		1,//wait4endcmd
		0,//unused
		1,
		8, //xfer_cnt
		(unsigned int*)dev_regval,
		{0x01800008}
		}
		
	};//nand0 flash read id*/


	//gpmi-1, dm9000 ethernet
	DMA_CMD reg_read[2] =
	{
		{
		&reg_read[1],//next command
		2,//command: 
		1,//:chain
		0,//irq_complete
		0,//nandlock
		0,//nandwait4ready
		0,//decrement_semaphore
		0,//wait4endcmd
		0,//un-used
		1, //piowords_cnt
		1, //xffer_cnt
		(unsigned int*)&dev_regno,
		{0x00d00001}
		},
		{
		0,//next command address

		1,//command, 01 for dma write
		0,//chain
		0,//irq_complete
		0,//nandlock
		0,//nandwait4ready
		1,//decrement semaphore
		1,//wait4endcmd
		0,//unused
		1,
		1, //xfer_cnt
		(unsigned int*)dev_regval,
		{0x01920001}
		}
		
	};
	serial_puthex((unsigned int)reg_read);
	serial_puts("\n");

	serial_puthex((unsigned int)&hw_apbh.ch[5].nxtcmdar);
	serial_puts("\n");
	
		//reset gpmi
		hw_gpmi.ctrl0.set = 1<<31;
		udelay(0xfff);
		hw_gpmi.ctrl0.clr = 0xc0000000;
		//hw_gpmi.timing0 = 0x00200406;

		//reset APBH
		hw_apbh.ctrl[0].set =1<<31; //0x80100000;
		udelay(0xfff);
		hw_apbh.ctrl[0].clr = 0xc0000000;

		//hw_apbh.ctrl[0].dat = 0x00200000; //select nand 0
		dev_regno = 0x28282828;//0x90;//0x28; 
		//while(1){
		while(1)
	{
		hw_apbh.ch[5].nxtcmdar = (unsigned int) reg_read; 
		hw_apbh.ch[5].sema = 0x1;
		udelay(0xfff);
		serial_puthex(dev_regval[0]);
		serial_puts(" ");
		serial_puthex(dev_regval[1]);
		serial_puts(" ");
		serial_puthex(dev_regval[2]);
		serial_puts(" ");
		serial_puthex(dev_regval[3]);
		serial_puts(" ");

		serial_puts("\n");
	}
	while(1);

#endif


void init_all_pins(void)
{
	int i;
	hw_pinctrl.ctrl.set = 0;
	/*hw_pinctrl.doe[0].set = 1<<23; //beep 3rd sword
	hw_pinctrl.dout[0].clr = 1<<23;
	*/
   	/*we use bank1-pin26,27 (duart_tx&rx)as LED flash*/
    /*	hw_pinctrl.muxsel[3].set = 0xf00000; 
	
    	hw_pinctrl.dout[1].set = 0x0c000000;
	hw_pinctrl.doe[1].set = 0x0c000000;

	for(i=0;i<3;i++)
    	{
		hw_pinctrl.dout[1].set = 3<<26;
		mdelay(300);
		hw_pinctrl.dout[1].clr = 3<<26;
		mdelay(300);
	}
     */
	/* GPIO init to DEBUG-UART rx and tx pins*/
	hw_pinctrl.muxsel[3].set = 0x00f00000;
	hw_pinctrl.muxsel[3].clr = 0x00500000;



	/*GPMI pin initialized for GPMI-8 and CE0 & CE1*/
	hw_pinctrl.muxsel[0].clr = 0xffff; //open gpmi_d0 - d7
	hw_pinctrl.muxsel[1].clr = 0x000fc3cf;//0x000fc3c3;open r/w wp, ready0,1, ALE, CLE
	hw_pinctrl.muxsel[5].clr = 0x03c00000;// 0x03000000; ////opne ce0/1
	hw_pinctrl.pull[0].set = 0xff;

	/*EMI DDR SDRAM init for EMI pins*/

	//hw_pinctrl.drive[9-13]: no need to set this time, we use default value, 2.5V/4ma
	//seems not work, change to 2.5V/12ma
	for(i=9;i<=14;i++)
		hw_pinctrl.drive[i].dat = 0x33333333;//0;//0x77777777;

	hw_pinctrl.muxsel[4].clr = 0xfffc0000; //open A0-A6
	hw_pinctrl.muxsel[5].clr = 0xfc3fffff; //open cas,ras,we,ce0/1,a7-a12,ba0/1
	hw_pinctrl.muxsel[6].clr = 0xffffffff; //open d0-d15
	hw_pinctrl.muxsel[7].clr = 0x00000fff; //open clk,clkn, dqs,dqm

	//hw_pinctrl.pull[3].clr = 0x0003ffff;	//disable padkeeper on EMI pins
}

void init_clock_power(void)
{
	hw_power.ctrl.clr = 0x40000000; //gate on power-ctrl domain
	/*Vdd-Digital output voltage*/
#if 1
	unsigned int val;
	val = hw_power.vdddctrl;
	val &= ~0x1f;	//bit0-4, vddd output voltage triger 
	val |= ((1550-800)/25)&0x1f;	//set to 1.55v, default 1.2v
	hw_power.vdddctrl = val; //for higher cpu freq.: 454MHz

	/*Vdd-Mem output voltage*/
	hw_power.vddmemctrl = 0xf00 | (((2500-1700)/50)&0x1f); //2.5v output, and enable current limit in case of damage 
	udelay(1000); //wait 1ms for capacitor charging
	hw_power.vddmemctrl &= ~0x600;	//clear current limit and pull down

	/*enable PLL and set dividor fraction*/
	hw_clkctrl.pllctrl[0].set = 0x10000; //power on pll
	udelay(150); //wait pll lock

	/*set cpu freq. to 454Mhz CLK_P*/
	hw_clkctrl.frac.clr = 0x3f;//6bit int. divider
	hw_clkctrl.frac.set = 19; //480Mhz * (18/19) = 454Mhz :ref_cpu
	hw_clkctrl.frac.clr = 0x80; //enable clock gate of cpu
	
	/*set AHB bus freq. CLK_H*/
	hw_clkctrl.hbus.set = 0x1f; //mask div fraction
	hw_clkctrl.hbus.clr = 0x1b; //454Mhz/4=113.5Mhz
	//hw_clkctrl.hbus.clr = 0x19;

	hw_clkctrl.clkseq.clr = 0x80; //switch cpu clock to pll (clear bypassing)
	// well, we got cpu running at 454Mhz now! 

	/*set EMI clock to 80Mhz */

	hw_clkctrl.frac.set = 0x3f00; 
	//hw_clkctrl.frac.clr = 0x400; //~27 ; //480Mhz * (18/27) = 320Mhz :ref_emi
	hw_clkctrl.frac.clr = 0x2100;  //480Mhz * (18/30) = 288Mhz :ref_emi
	hw_clkctrl.frac.clr = 0x8000; //enable clock gate of emi
	udelay(100);	
	//hw_clkctrl.emi &= ~0x1f; //clear emi dividor
	hw_clkctrl.emi |= 0x3f;
	//hw_clkctrl.emi |= 0x4; //320Mhz/4 =80Mhz , the final emi freq.
	//hw_clkctrl.emi &= ~0x17;//1b; //320M/4=80M
	hw_clkctrl.emi &= ~0x3c; //288M/3=96M
	//zhai comment @class
	hw_clkctrl.emi &= ~0xc0000000; //clr gate, syc disble

	hw_clkctrl.clkseq.clr = 0x40; //enable emi clock to pll (clear bypassing)
#endif
	/*set GPMI freq. */
	hw_clkctrl.gpmi &= ~(1<<31); //xtal clk 24Mhz to GPMI

	/*set UART freq. */
	//fixed to 24Mhz from xstal
}

void init_sdram(void) //imx233 128pin MT46V32M16
{
	hw_emi.ctrl.clr = 0xc0000000; //clr soft-rest and gate

	hw_dram.ctl[0] = 0x01010001;
	hw_dram.ctl[1] = 0x00010000;
	hw_dram.ctl[2] = 0x01000000;
	hw_dram.ctl[3] = 0x00000001;
	hw_dram.ctl[4] = 0x00000101;
	hw_dram.ctl[5] = 0x00000000;
	hw_dram.ctl[6] = 0x00010000;
	hw_dram.ctl[7] = 0x01000001;
	hw_dram.ctl[9] = 0x00000001;

	hw_dram.ctl[10] = 0x07000200;
	hw_dram.ctl[11] = 0x00070202; //64MB, col-line: 10
	hw_dram.ctl[12] = 0x02020000;
	hw_dram.ctl[13] = 0x04040a01;
	hw_dram.ctl[14] = 0x00000201;
	hw_dram.ctl[15] = 0x02040000;
	hw_dram.ctl[16] = 0x02000000;
	hw_dram.ctl[17] = 0x25001506;
	hw_dram.ctl[18] = 0x1f1f0000;
	hw_dram.ctl[19] = 0x027f1a1a;

	hw_dram.ctl[20] = 0x02041c22;//ras-min: 5->4 
	hw_dram.ctl[21] = 0x00000006;//trfc: 7->6
	hw_dram.ctl[22] = 0x00080008;
	hw_dram.ctl[23] = 0x00200020;
	hw_dram.ctl[24] = 0x00200020;
	hw_dram.ctl[25] = 0x00200020;
	hw_dram.ctl[26] = 0x00000269;//tref:0x2e6->269
	hw_dram.ctl[29] = 0x00000000;//lowpower_external_cnt:20->00
	hw_dram.ctl[30] = 0x00000000;//lowpower_external_cnt:20->00
	hw_dram.ctl[31] = 0x00c80000;
	hw_dram.ctl[32] = 0x000615d6;//0x00081a3b;
	hw_dram.ctl[33] = 0x000000c8;
	hw_dram.ctl[34] = 0x00003e80;//4b0d;
	hw_dram.ctl[36] = 0x00000101;
	hw_dram.ctl[37] = 0x00040001;
	hw_dram.ctl[38] = 0x00000000;
	hw_dram.ctl[39] = 0x00000000;
	hw_dram.ctl[40] = 0x00010000;
	hw_dram.ctl[8] = 0x01000000; //start sdram controller (0x40000000~0x44000000)
	udelay(10000);
	hw_dram.ctl[8] |= 0x00010000;
}
