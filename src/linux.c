#include "setup.h"
/* imx233 */
char cmd[] = "noinitrd console=ttyAM0,115200 root=/dev/mmcblk0p2 rw rootwait ssp1=mmc";
//char cmd[] = "init=/linuxrc console=ttyAM0,115200 root=/dev/ram rw ramdisk_size=4096";

/*char cmd[] = "init=/linuxrc console=ttyAM0,115200 "
			     //"mtdparts=akae2440-nand:16k(myboot),1600k(kernel),8m(ext2),-(user) "
			     //"root=/dev/mtdblock2 rw";
			     "root=/dev/ram  rw ramdisk_size=8192"  ; 
			     */
	struct tag* p;
void init_taglist(int addr)
{
	int i;
	p = (struct tag*) addr;
	p->hdr.tag  =  ATAG_CORE;
	p->hdr.size = tag_size(tag_core);
	p->u.core.flags = 1;
	p->u.core.pagesize = 4096;
	p->u.core.rootdev = 0x00000000;
	
	p = tag_next(p);
	p->hdr.tag = ATAG_CMDLINE;
	p->hdr.size =  (sizeof (cmd) + sizeof(struct tag_header) + 3) >>2;
	for(i=0; i< sizeof (cmd); i++)
	{
		p->u.cmdline.cmdline[i] = cmd[i];
	}
	/*
	p = tag_next(p);
	p->hdr.tag = ATAG_MEM;
	p->hdr.size = tag_size(tag_mem32);
	p->u.mem.size = 0x1000;
	p->u.mem.start = 0;
	*/
	p = tag_next(p);
	p->hdr.tag = ATAG_MEM;
	p->hdr.size = tag_size(tag_mem32);
	p->u.mem.size = 64*1024*1024;
	p->u.mem.start = 0x40000000;
#if 1
	p = tag_next(p);
	p->hdr.tag = ATAG_INITRD2;
	p->hdr.size = tag_size(tag_initrd);
	//p->u.ramdisk.flags = 1;/* bit 0 = load, bit 1 = prompt */
	p->u.initrd.size = 4*1024*1024 ; //7M Bytes	/* decompressed ramdisk size in _kilo_ bytes */
	p->u.initrd.start = 0x40a00000;	/* starting block of floppy-based RAM disk image */
#endif
	
	p = tag_next(p);
	p->hdr.tag = ATAG_NONE;
	p->hdr.size = 0;
	
}
