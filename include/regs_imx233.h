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

typedef struct {
	reg dat;
	reg set;
	reg clr;
	reg tog;
}REG;


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
	reg pix  __attribute__ ((aligned(sizeof(REG))));
	reg ssp  __attribute__ ((aligned(sizeof(REG))));
	reg gpmi  __attribute__ ((aligned(sizeof(REG))));
	reg spdif  __attribute__ ((aligned(sizeof(REG))));
	reg emi  __attribute__ ((aligned(sizeof(REG))));
	REG _pad_1;
	reg saif  __attribute__ ((aligned(sizeof(REG))));
	reg tv  __attribute__ ((aligned(sizeof(REG))));
	reg etm  __attribute__ ((aligned(sizeof(REG))));
	REG frac;
	REG frac1;
	REG clkseq;
	reg reset  __attribute__ ((aligned(sizeof(REG))));
	reg status  __attribute__ ((aligned(sizeof(REG))));
	reg version  __attribute__ ((aligned(sizeof(REG))));
}CLKCTRL;

typedef struct {
	REG ctrl0;
	reg compare __attribute__ ((aligned(sizeof(REG))));
	REG eccctrl;
	reg ecccount __attribute__ ((aligned(sizeof(REG))));
	reg payload __attribute__ ((aligned(sizeof(REG))));
	reg auxiliary __attribute__ ((aligned(sizeof(REG))));
	REG ctrl1;
	reg timing[2] __attribute__ ((aligned(sizeof(REG))));
	REG _pad_1;
	reg data __attribute__ ((aligned(sizeof(REG))));
	reg stat __attribute__ ((aligned(sizeof(REG))));
	reg debug __attribute__ ((aligned(sizeof(REG))));
	reg version __attribute__ ((aligned(sizeof(REG))));
	reg debug2 __attribute__ ((aligned(sizeof(REG))));
	reg debug3 __attribute__ ((aligned(sizeof(REG))));
}GPMI;
