// This list of physical registers was extracted by running:
// grep _PHYS linux-2.6.26.3/src/include/asm/arch/* | cut -d':' -f 2
#define REGS_DIGCTL_BASE_PHYS (0x8001C000)
#define REGS_DRAM_BASE_PHYS (0x800E0000)
#define REGS_DRI_BASE_PHYS (0x80074000)
#define REGS_ECC8_BASE_PHYS (0x80008000)
#define REGS_EMI_BASE_PHYS (0x80020000)
#define REGS_GPMI_BASE_PHYS (0x8000C000)
#define REGS_I2C_BASE_PHYS (0x80058000)
#define REGS_IR_BASE_PHYS (0x80078000)
#define REGS_ICOLL_BASE_PHYS (0x80000000)
#define REGS_LCDIF_BASE_PHYS (0x80030000)
#define REGS_LRADC_BASE_PHYS (0x80050000)
#define REGS_OCOTP_BASE_PHYS (0x8002C000)
#define REGS_PINCTRL_BASE_PHYS (0x80018000)
#define REGS_POWER_BASE_PHYS (0x80044000)
#define REGS_PWM_BASE_PHYS (0x80064000)
#define REGS_SAIF1_BASE_PHYS (0x80042000)
#define REGS_PXP_BASE_PHYS (0x8002A000)
#define REGS_RTC_BASE_PHYS (0x8005C000)
#define REGS_SAIF2_BASE_PHYS (0x80046000)
#define REGS_SPDIF_BASE_PHYS (0x80054000)
#define REGS_TIMROT_BASE_PHYS (0x80068000)
#define REGS_SSP1_BASE_PHYS (0x80010000)
#define REGS_SSP2_BASE_PHYS (0x80034000)
#define REGS_SYDMA_BASE_PHYS (0x80026000)
#define REGS_DCP_BASE_PHYS (0x80028000)
#define REGS_TVENC_BASE_PHYS (0x80038000)
#define REGS_UARTAPP1_BASE_PHYS (0x8006C000)
#define REGS_UARTAPP2_BASE_PHYS (0x8006E000)
#define REGS_UARTDBG_BASE_PHYS (0x80070000)
#define REGS_USBCTRL_BASE_PHYS (0x80080000)
#define REGS_USBPHY_BASE_PHYS (0x8007C000)
#define REGS_APBH_BASE_PHYS (0x80004000)
#define REGS_APBX_BASE_PHYS (0x80024000)
#define REGS_AUDIOIN_BASE_PHYS (0x8004C000)
#define REGS_AUDIOOUT_BASE_PHYS (0x80048000)
#define REGS_BCH_BASE_PHYS (0x8000A000)
#define REGS_CLKCTRL_BASE_PHYS (0x80040000)


typedef volatile int reg;
typedef volatile char byte;

typedef struct {
	reg dat;
	reg set;
	reg clr;
	reg tog;
}REG;
#define reg(reg_name) union{ reg reg_name; REG _reg_name##_aligned;}


typedef struct {
	REG ctrl __attribute__ ((aligned(0x100)));
	REG muxsel[8] __attribute__ ((aligned(0x100)));
	REG drive[14] __attribute__ ((aligned(0x100)));
	REG _pad_16[16] __attribute__ ((aligned(0x100)));
	REG pull[4] __attribute__ ((aligned(0x100)));
	REG dout[3] __attribute__ ((aligned(0x100)));
	REG din[3] __attribute__ ((aligned(0x100)));
	REG doe[3] __attribute__ ((aligned(0x100)));
	REG pin2irq[3] __attribute__ ((aligned(0x100)));
	REG irqen[3] __attribute__ ((aligned(0x100)));
	REG irqlevel[3] __attribute__ ((aligned(0x100)));
	REG irqpol[3] __attribute__ ((aligned(0x100)));
	REG irqstat[3] __attribute__ ((aligned(0x100)));
}PINCTRL;


typedef struct {
	reg dr;
	union{
	reg rsr;
	reg ecr;
	};
	reg _pad_[0x4];
	reg fr;
	reg _pad_1;
	reg ilpr;
	reg ibrd;
	reg fbrd;
	reg lcr_h;
	reg cr;
	reg ifls;
	reg imsc;
	reg ris;
	reg mis;
	reg icr;
	reg macr;
}UARTDBG;

typedef struct {
	REG pllctrl[2];
	REG cpu;
	REG hbus;
	REG xbus;
	REG xtal;
	reg( pix ); 
	reg( ssp );
	reg( gpmi );
	reg( spdif );
	reg( emi );
	REG _pad_1;
	reg( saif );
	reg( tv );
	reg( etm );
	REG frac;
	REG frac1;
	REG clkseq;
	reg( reset );
	reg( status );
	reg( version );
}CLKCTRL;

typedef struct {
	REG ctrl0;
	reg( compare );
	REG eccctrl;
	reg( ecccount );
	reg( payload );
	reg( auxiliary );
	REG ctrl1;
	reg( timing0 );
	reg( timing1 );
	REG _pad_1;
	union{
	reg data;
	byte data_byte;
	REG _data_ligned;
	};
	reg( stat );
	reg( debug );
	reg( version );
	reg( debug2 );
	reg( debug3 );
}GPMI;

typedef struct {
	REG ctrl[3];
	reg( devsel );
	struct {
		reg( curcmdar );
		reg( nxtcmdar );
		reg( cmd );
		reg( bar );
		reg( sema );
		reg( debug1 );
		reg( debug2 );
	}ch[8];
	REG _pad_[3];
	reg( version );
}APBH;

typedef struct {
	REG ctrl;
	REG v5ctrl;
	REG minpwr;
	REG charge;
	reg( vdddctrl );
	reg( vddactrl );
	reg( vddioctrl );
	reg( vddmemctrl );
	reg( dcdc4p2 );
	reg( misc );
	reg( dclimits );
	reg( loopctrl );
	reg( sts );
	REG( speed );
	reg( battmonitor );
	REG _pad_1;
	REG reset;
	REG debug;
	REG special;
	reg version;
}POWER;

typedef struct {
	REG ctrl;
	REG _pad_1[14];
	reg( version );
}EMI;

typedef struct {
	reg ctl[41];
}DRAM;

typedef struct {
	REG ctrl;
	REG status;
	REG hclkcount;
	REG ramctrl;
	REG ramrepair;
	REG romctrl;
	reg( writeonce );
	REG _pad_1[2];
	reg( entropy );
	reg( entropy_latched );
	reg( sjtag_dbg );
	REG microseconds;
	reg( dbgrd );
	reg( dbg );
	REG ocram_bist_csr;
	REG _pad_2;
	REG ocram_status[14];
	//not finished , TBD
}DIGCTL;
