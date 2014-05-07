
#include "regs_imx233.h"
#include "init.h"
#include "utils.h"
#include "sd.h"

static SSP * const ssp = (SSP*) REGS_SSP1_BASE_PHYS;
#define hw_ssp (*ssp)
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

#define LONG_RESP (1<<19)
#define GET_RESP (1<<17)
#define CMD_ENABLE (1<<16)
#define APPEND_8CYC (1<<20)
#define RUN (1<<29)
#define CLKGATE (1<<30)


/* response:
    - pointer to buffer with 32B length 
*/
#define SDIO_IRQ (1<<31)
#define RESP_ERR_IRQ (1<<29)
#define RESP_TIMEOUT_IRQ (1<<27)
#define IGNORE_CRC (1<<26)
#define DATA_TIMEOUT_IRQ (1<<25)
#define DATA_CRC_IRQ (1<<23)
#define FIFO_UNDERRUN_IRQ (1<<21)
#define RECV_TIMEOUT_IRQ (1<<17)
#define FIFO_OVERRUN_IRQ (1<<15)

int sd_cmd( unsigned int id, unsigned int argument, unsigned int * response )
{
	unsigned int mode = CMD_ENABLE; 
	int err;
	if ( id >= 64 )		// invalid cmd number
	       return -1;	
	if ( (id != 0) && (id != 5) && (id != 15) ) {
		mode |= GET_RESP;
		if ( (id == 2) || (id == 9) || (id == 10) ) {
			mode |= LONG_RESP;
		}
		if ( id == 41 ) {
			mode |= IGNORE_CRC;
		}
	} 
	hw_ssp.ctrl0.clr = CMD_ENABLE | GET_RESP | LONG_RESP | IGNORE_CRC; 
	hw_ssp.ctrl0.set = mode; 
	hw_ssp.cmd0.dat = APPEND_8CYC | id;
	hw_ssp.cmd1 = argument;
	hw_ssp.ctrl1.clr = 0xffff0000; 	//clear error status

	hw_ssp.ctrl0.set = RUN;
	while (hw_ssp.ctrl0.dat & RUN);
	//udelay(10);
	err = hw_ssp.ctrl1.dat & ( RESP_ERR_IRQ | RESP_TIMEOUT_IRQ ); 
	if ( !err && response ) {
		response[0] = hw_ssp.sdresp0;
		response[1] = hw_ssp.sdresp1;
		response[2] = hw_ssp.sdresp2;
		response[3] = hw_ssp.sdresp3;
	}
	return err;
}

void sd_init( void )
{
	hw_ssp.ctrl0.clr = CLKGATE;
	hw_ssp.ctrl0.clr = (1<<31); // enable pio
	hw_ssp.ctrl0.dat = 0;
	//while (hw_ssp.ctrl0.dat & CLKGATE);
	hw_ssp.ctrl1.dat =0x273;// 0x2273;	 // sd mode, dma enable,polarity
	hw_ssp.timing = 0xEFF041D; // initial clk 200khz
}

#define TRY 1000 
#define SDHC (1<<30)
#define INIT_COMPLETE (1<<31)
#define V3_3 0x300000

/* ret: 
    - success: 0
    - failed: non-zero errno.
*/
int sd_probe( void )
{
	int i, err;
	unsigned int resp[4] = {0};
	unsigned int arg = 0;

	sd_cmd( 0, 0, NULL );

	/* CMD8, query interface */
	err = sd_cmd( 8, 0x1AB, resp );
	if ( !err && (resp[0]&0xff)==0xAB ) { /* SDHC responses */
		arg |= SDHC;
		printf("SD: rev 2.0 supported\n");
	}

	/* CMD55, ACMD41, sd initializing */
	for( i=0; i<TRY; i++) {
		err = sd_cmd( 55, 0, resp );
		if( err ) {
			printf( "SD: cmd55 err=%x, r1=%x\n", err, resp[0] );
			return err;
		}
		/* voltage is around 3.3v */
		err = sd_cmd( 41, i==1 ? arg|V3_3 : arg, resp );
		if( (resp[0]&V3_3)==0 ) {
			printf( "SD: 3.3v not support=%x, r1=%x\n", err, resp[0] );
			return err;
		}
		if( resp[0]&INIT_COMPLETE ) {
			break;
		}
		udelay(1000);
	}
	if( i==TRY ) { 
		printf( "SD: init failed, err=%x\n", err );
		return -1;
	}

	/* CMD2, send all card CID */
	err = sd_cmd( 2, 0, resp );
	if( err ) {
		printf( "SD: CID err=%x\n", err );
		return err;
	}
	else {
		char *cid = (char*) resp;
		printf( "SD TYPE: " );
		for( i=12; i>=8; i-- ) {
			putchar( cid[i] );
		}
		putchar( '\n' );
		printf( "SERIAL #: %x\n", (resp[1]<<8) | cid[3] );
	}

	/* CMD3, send new RCA */
	err = sd_cmd( 3, 0, resp );
	if( err || (resp[0]&0xE000) ) {
		printf( "SD: RCA err=%x, r1=%x\n", err, resp[0] );
		return err;
	}
	else {
		arg = resp[0] & 0xFFFF0000; /* store RCA for later use */
	}

	/* CMD9, get CSD */
	err = sd_cmd( 9, arg, resp );
	if( err ) {
		printf( "SD: CSD err=%x\n", err );
		return err;
	}
	else {
		char *csd = (char *) resp;
		unsigned int blocks = 0;
		unsigned int bsize = csd[10]&0xF;
		printf( "SD max speed: %x (32=20MHz,5A=50MHz)\n", csd[12] ); 
		printf( "SD block size: %x bytes\n", 1<<bsize );
		switch( csd[15]>>6 ) {
		case 0: /* rev 1.0 */
			blocks = ((((resp[2]&0x3FF)<<2)|(resp[1]>>30))+1) << (((resp[1]>>15)&0x7)+2);
			break;
		case 1: /* rev 2.0 */
			blocks = ( ((resp[2]&0x3F)<<16) | (resp[1]>>16) ) + 1;	
			break;
		}
		printf( "total size %x blocks(%xMB)\n", blocks, bsize >= 10 ? blocks << (bsize-10) : blocks >> (10-bsize) );
	}

	return err; 
}

/* addr: 
    - in bytes
    - positive means from the head of sd card 
    - negtive means from the end of the card 
   len:
    - the bytes to read
   buf:
    - memory buf to hold the data from sd card
*/
int sd_read( int addr, unsigned int len, unsigned char * buf)
{
	return 0;
}

#if 0
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
#endif
