
#include "regs_imx233.h"
#include "init.h"
#include "utils.h"

static GPMI * const gpmi = (GPMI*) REGS_GPMI_BASE_PHYS;
#define hw_gpmi (*gpmi)
static APBH * const apbh = (APBH*) REGS_APBH_BASE_PHYS;
#define hw_apbh (*apbh)

typedef struct _dma_cmd
{
	struct _dma_cmd *next;

	unsigned int command : 	2 ;
	unsigned int chain :	1 ;
	unsigned int irq_complete : 1 ;
	unsigned int nandlock : 1 ;
	unsigned int nandwait4ready : 1 ;
	unsigned int decrement_semaphore : 1 ;
	unsigned int wait4endcmd : 1 ;
	unsigned int un_used : 4 ;
	unsigned int piowords_cnt : 4 ;
	unsigned int xfer_cnt : 16 ;

	unsigned int *dma_buf;
	unsigned int piowords[1];	//size depends on piowords_cnt
}DMA_CMD;


unsigned int gpmi_dm9000_read_reg(unsigned int regno)
{
	//gpmi-1, dm9000 ethernet, 8bit mode
	volatile unsigned int regval = 0;
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
		1,//wait4endcmd
		0,//un-used
		1, //piowords_cnt
		1, //xffer_cnt
		(unsigned int*)&regno,
		{0x00d00001}
		},
		{
		0,//next command address

		1,//command, 01 for dma write
		0,//chain
		1,//irq_complete
		0,//nandlock
		0,//nandwait4ready
		1,//decrement semaphore
		1,//wait4endcmd
		0,//unused
		1,
		1, //xfer_cnt
		(unsigned int*)&regval,
		{0x01920001}
		}
		
	};

		//reset gpmi
		hw_gpmi.ctrl0.set = 1<<31;
		udelay(0xf);
		hw_gpmi.ctrl0.clr = 0xc0000000;
		//hw_gpmi.timing0 = 0x00200406;

		//reset APBH
		hw_apbh.ctrl[0].set =1<<31; //0x80100000;
		udelay(0xf);
		hw_apbh.ctrl[0].clr = 0xc0000000;
//while(1)
		//start dma transferring
		hw_apbh.ch[5].nxtcmdar = (unsigned int) reg_read; 
		//printf("reg_read=%x\n", reg_read);
		hw_apbh.ch[5].sema = 0x1;
		while(!(hw_apbh.ctrl[1].dat & (1<<5)));
		//udelay(0x1fff);
		//printf("regval add=%x\n",&regval);

		//hw_apbh.ctrl[1].clr = 1<<5;
		//while(1);
		return regval;
}

void gpmi_dm9000_write_reg(unsigned int regno, unsigned int regval)
{
	//gpmi-1, dm9000 ethernet, 8bit mode
	DMA_CMD reg_write[2] =
	{
		{
		&reg_write[1],//next command
		2,//command: 
		1,//:chain
		0,//irq_complete
		0,//nandlock
		0,//nandwait4ready
		0,//decrement_semaphore
		1,//wait4endcmd
		0,//un-used
		1, //piowords_cnt
		1, //xffer_cnt
		(unsigned int*)&regno,
		{0x00d00001}
		},
		{
		0,//next command address

		2,//command, 01 for dma read 
		0,//chain
		1,//irq_complete
		0,//nandlock
		0,//nandwait4ready
		1,//decrement semaphore
		1,//wait4endcmd
		0,//unused
		1,
		1, //xfer_cnt
		(unsigned int*)&regval,
		{0x00920001}
		}
		
	};

		//reset gpmi
		hw_gpmi.ctrl0.set = 1<<31;
		udelay(0xf);
		hw_gpmi.ctrl0.clr = 0xc0000000;
		//hw_gpmi.timing0 = 0x00200406;

		//reset APBH
		hw_apbh.ctrl[0].set =1<<31; //0x80100000;
		udelay(0xf);
		hw_apbh.ctrl[0].clr = 0xc0000000;

		//start dma transferring
		hw_apbh.ch[5].nxtcmdar = (unsigned int) reg_write; 
		hw_apbh.ch[5].sema = 0x1;
		//udelay(0x1fff);

		while(!(hw_apbh.ctrl[1].dat & (1<<5)));
		//hw_apbh.ctrl[1].clr = 1<<5;
}
void gpmi_dm9000_read_data_bulk(unsigned char *buf, int count)
{
	//gpmi-1, dm9000 ethernet, 8bit mode
	DMA_CMD data_read =
	{
		0,//next command address
		1,//command, 01 for dma write
		0,//chain
		1,//irq_complete
		0,//nandlock
		0,//nandwait4ready
		1,//decrement semaphore
		1,//wait4endcmd
		0,//unused
		1,
		count, //xfer_cnt
		(unsigned int *)buf,
		{0x01d30000 | (count&0xffff)}
	};
		//reset gpmi
		hw_gpmi.ctrl0.set = 1<<31;
		udelay(0xf);
		hw_gpmi.ctrl0.clr = 0xc0000000;
		//reset APBH
		hw_apbh.ctrl[0].set =1<<31; //0x80100000;
		udelay(0xf);
		hw_apbh.ctrl[0].clr = 0xc0000000;
		//start dma transferring
		hw_apbh.ch[5].nxtcmdar = (unsigned int) &data_read; 
		hw_apbh.ch[5].sema = 0x1;
		while(!(hw_apbh.ctrl[1].dat & (1<<5)));
}
void gpmi_dm9000_write_data_bulk(unsigned int *buf, int count)
{
	//gpmi-1, dm9000 ethernet, 8bit mode
	DMA_CMD data_write =
	{
		0,//next command address
		2,//command, 01 for dma read 
		0,//chain
		1,//irq_complete
		0,//nandlock
		0,//nandwait4ready
		1,//decrement semaphore
		1,//wait4endcmd
		0,//unused
		1,
		count, //xfer_cnt
		(unsigned int*)buf,
		{0x00d30000 | (count&0xffff)}
	};
		//reset gpmi
		hw_gpmi.ctrl0.set = 1<<31;
		udelay(0xf);
		hw_gpmi.ctrl0.clr = 0xc0000000;
		//reset APBH
		hw_apbh.ctrl[0].set =1<<31; 
		udelay(0xf);
		hw_apbh.ctrl[0].clr = 0xc0000000;
		//start dma transferring
		hw_apbh.ch[5].nxtcmdar = (unsigned int) &data_write; 
		hw_apbh.ch[5].sema = 0x1;
		while(!(hw_apbh.ctrl[1].dat & (1<<5)));
}
void gpmi_dm9000_write_reg_index(unsigned int index)
{
	//gpmi-1, dm9000 ethernet, 8bit mode
	DMA_CMD reg_write =
	{
		0,//next command
		2,//command: 
		0,//:chain
		1,//irq_complete
		0,//nandlock
		0,//nandwait4ready
		0,//decrement_semaphore
		1,//wait4endcmd
		0,//un-used
		1, //piowords_cnt
		1, //xffer_cnt
		(unsigned int*)&index,
		{0x00d00001}
	};

		//reset gpmi
		hw_gpmi.ctrl0.set = 1<<31;
		udelay(0xf);
		hw_gpmi.ctrl0.clr = 0xc0000000;
		//reset APBH
		hw_apbh.ctrl[0].set =1<<31; //0x80100000;
		udelay(0xf);
		hw_apbh.ctrl[0].clr = 0xc0000000;
		//start dma transferring
		hw_apbh.ch[5].nxtcmdar = (unsigned int) &reg_write; 
		hw_apbh.ch[5].sema = 0x1;
		while(!(hw_apbh.ctrl[1].dat & (1<<5)));
}

unsigned int gpmi_k9f1208_read_id(void)
{
	volatile unsigned char flash[] = {0x90, 0x00};
	volatile unsigned int id=0;
	DMA_CMD read_id[2] =
	{
		{
		&read_id[1],//next command
		2,//command: 
		1,//:chain
		0,//irq_complete
		0,//nandlock
		0,//nandwait4ready
		0,//decrement_semaphore
		1,//wait4endcmd
		0,//un-used
		1, //piowords_cnt
		2, //xffer_cnt
		(unsigned int*)flash,
		{0x00c30002}
		},
		{
		0,//next command address

		1,//command, 01 for dma write
		0,//chain
		1,//irq_complete
		0,//nandlock
		0,//nandwait4ready
		1,//decrement semaphore
		1,//wait4endcmd
		0,//unused
		1,
		4, //xfer_cnt
		(unsigned int*)&id,
		{0x01800004}
		}
		
	};//nand0 flash read id*/
	//reset gpmi
	hw_gpmi.ctrl0.set = 1<<31;
	udelay(0xf);
	hw_gpmi.ctrl0.clr = 0xc0000000;
	//hw_gpmi.timing0 = 0x00200406;
//////////////////////////////
	//hw_gpmi.ctrl
	/*int i;
	hw_gpmi.ctrl0.dat = 0x00c30002;
	hw_gpmi.ctrl0.set = 1<<29;
	for(i=0;i<2;i++)
		hw_gpmi.data_byte = flash[i];
	hw_gpmi.ctrl0.clr = 1<<29;
	hw_gpmi.ctrl0.dat = 0x01800004;
	hw_gpmi.ctrl0.set = 1<<29;
	for(i=0;i<4;i++)
	{
		id<<=8;
		id|=hw_gpmi.data_byte;
	}
	hw_gpmi.ctrl0.clr = 1<<29;
	return id;
	*/
//////////////////////////////
	//reset APBH
	hw_apbh.ctrl[0].set =1<<31; //0x80100000;
	udelay(0xf);
	hw_apbh.ctrl[0].clr = 0xc0000000;

	//start dma transferring
	hw_apbh.ch[4].nxtcmdar = (unsigned int) read_id; 
	hw_apbh.ch[4].sema = 0x1;
	//udelay(0x1fff);
	//printf("stat=%x\n", hw_apbh.ctrl[1].dat);
	while(!(hw_apbh.ctrl[1].dat & (1<<4)));
	//hw_apbh.ctrl[1].clr = 1<<5;
	//printf("stat1=%x\n", hw_apbh.ctrl[1].dat);
	return id;

}

unsigned int gpmi_k9f1208_read_page(int block, int page, char*buf)
{
	volatile unsigned char flash[] = {
		0x00,
		0x00,
		page | ((block<<5) & 0xff),
		(block>>3) & 0xff,
		(block >> 11) & 0xff
	};

	DMA_CMD read_id[2] = {
		{
		&read_id[1],//next command
		2,//command: 
		1,//:chain
		0,//irq_complete
		0,//nandlock
		0,//nandwait4ready
		0,//decrement_semaphore
		1,//wait4endcmd
		0,//un-used
		1, //piowords_cnt
		5, //xffer_cnt
		(unsigned int*)flash,
		{0x00c30005}
		},
		{
		0,//next command address

		1,//command, 01 for dma write
		0,//chain
		1,//irq_complete
		0,//nandlock
		0,//nandwait4ready
		1,//decrement semaphore
		1,//wait4endcmd
		0,//unused
		1,
		512, //xfer_cnt
		(unsigned int*)buf,
		{0x01800200}
		}
		
	};//nand0 flash read id*/
	//reset gpmi
	hw_gpmi.ctrl0.set = 1<<31;
	udelay(0xf);
	hw_gpmi.ctrl0.clr = 0xc0000000;
	//hw_gpmi.timing0 = 0x00200406;

	//reset APBH
	hw_apbh.ctrl[0].set =1<<31; //0x80100000;
	udelay(0xf);
	hw_apbh.ctrl[0].clr = 0xc0000000;

	//start dma transferring
	hw_apbh.ch[4].nxtcmdar = (unsigned int) read_id; 
	hw_apbh.ch[4].sema = 0x1;
	//udelay(0x1fff);
	//printf("stat=%x\n", hw_apbh.ctrl[1].dat);
	while(!(hw_apbh.ctrl[1].dat & (1<<4)));
	//hw_apbh.ctrl[1].clr = 1<<5;
	//printf("stat1=%x\n", hw_apbh.ctrl[1].dat);
	return 512;

}
/*	hw_gpmi.ctrl0.clr = 0x40000000;
	hw_gpmi.ctrl0.clr = 0x80000000;
	mdelay(0x1000);

	hw_gpmi.ctrl0.set = 0x00d00000; //see note zhai
	hw_gpmi.timing0 &= ~0xffff0000;
*/
//	hw_clkctrl.clkseq 
#if 0
//	hw_pinctrl.doe[2].set = 1<<27;///0x0f000000;// /
//	hw_pinctrl.dout[2].set = 1<<27;//0x10000000;
//	ready to read 0x28,29,2a,2b of dma9000, should be 0a46, 9000

	//hw_pinctrl.dout[2].clr = 1<<27;
	//serial_puthex((int)&hw_gpmi.data_byte);
	for(i=0, dev_regno = 0x28;i<4;i++, dev_regno++)
	{
		serial_puts("CTRL0:");
		serial_puthex(hw_gpmi.ctrl0.dat);
		serial_puts("\ndm9000 reg no:");
		hw_gpmi.payload = (unsigned int)&dev_regno;
		hw_gpmi.ctrl0.set = 1<<17; //set ale
		hw_gpmi.ctrl0.clr = 1<<24; //write mode
		hw_gpmi.ctrl0.clr = 0xffff;
		hw_gpmi.ctrl0.set = 0x1;
		//hw_pinctrl.dout[0].clr = 1<<17;
		
		serial_puthex(dev_regno);
		serial_puts("\n");
		hw_gpmi.ctrl0.set = 1<<29; //run command to ALE enable write
		hw_gpmi.data_byte = (volatile unsigned char)dev_regno;
		//serial_puts("debug:");
		//serial_puthex(hw_gpmi.debug);
		//serial_puts("\n");
		udelay(0x1ffff);
		hw_gpmi.ctrl0.clr = 1<<29; //stop command

		serial_puts("debug:");
		serial_puthex(hw_gpmi.debug);
		serial_puts("\nCTRL0-2:");
		serial_puthex(hw_gpmi.ctrl0.dat);
		serial_puts("\n");
		//hw_pinctrl.dout[0].set = 1<<17;
	/******************************/
		hw_gpmi.payload = (unsigned int)&dev_regval;
		hw_gpmi.ctrl0.clr = 1<<17; //set to data
		hw_gpmi.ctrl0.set = 1<<24; //read  mode
		//while(hw_gpmi.ctrl0.dat & (1<<29)); //wait command finish
		//hw_gpmi.ctrl0.clr = 1<<29;
		//hw_gpmi.ctrl0.clr = 1<<18; //clr ale to data mode
		hw_gpmi.ctrl0.set = 1<<29; //start command
		dev_regval = hw_gpmi.data_byte;
		udelay(0x1ffff);
		hw_gpmi.ctrl0.clr = 1<<29; //stop command
		//udelay(0xfff);
		serial_puts("reg_val:");
		serial_puthex(dev_regval);
		serial_puts("\n"); 

	/*	serial_puts("0x8000c0a0:");
		serial_puthex(hw_gpmi.data_byte);
		serial_puts("\n"); 
*/
	}
#endif
	//while(1);

