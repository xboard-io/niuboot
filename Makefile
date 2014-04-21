COMPILER_DIR	=
CROSS_COMPILE	?= arm-none-eabi-

SRCDIR		= $(CURDIR)/src
INCLUDEDIR	= $(CURDIR)/include
OUTPUTDIR	= $(CURDIR)/output

# Linker script 
BASE_ADDR	?= 0x00000000
BOOT_LAYOUT_IN	= $(SRCDIR)/niuboot.ld.in
BOOT_LAYOUT_OUT	= $(OUTPUTDIR)/niuboot.ld

# Output ELF/BIN binary image
NIUBOOT_ELF	= $(OUTPUTDIR)/niuboot
NIUBOOT_BIN	= $(OUTPUTDIR)/niuboot.bin

AS	= $(CROSS_COMPILE)as
CC	= $(CROSS_COMPILE)gcc
LD	= $(CROSS_COMPILE)ld
CPP	= $(CROSS_COMPILE)cpp
STRIP	= $(CROSS_COMPILE)strip
OBJCOPY	= $(CROSS_COMPILE)objcopy
OBJDUMP	= $(CROSS_COMPILE)objdump
NM	= $(CROSS_COMPILE)nm

LIBGCCDIR = $(dir $(shell $(CC) -print-libgcc-file-name))

CFLAGS 	+= -I$(INCLUDEDIR) -I$(LIBGCCDIR)/include
CFLAGS 	+= -Wall -O -g
CFLAGS 	+= -nostdinc -fno-builtin -fdump-rtl-expand

LDFLAGS += -static -nostdlib
LDFLAGS += -L$(LIBGCCDIR) -lgcc -T$(BOOT_LAYOUT_OUT)

# Generic code
SRC_OBJS  = entry.o serial.o main.o utils.o init.o gpmi.o dm9000x.o net.o linux.o
SRC_HEADS = dm9000x.h gpmi.h init.h main.h net.h regs_imx233.h serial.h types.h utils.h setup.h

NIUBOOT_OBJS  = $(addprefix $(SRCDIR)/, $(SRC_OBJS))
NIUBOOT_HEADS = $(addprefix $(INCLUDEDIR)/, $(SRC_HEADS))

# Default goal
all: $(OUTPUTDIR) $(NIUBOOT_BIN)
	@echo Do NOT put any useful thing in output folder....
#
# Define an implicit rule for assembler files
# to run them through C preprocessor
#
%.o: %.S
	$(CC) -c $(CFLAGS) -D__ASSEMBLY__ -o $@ $<

%.o: %.c ${NIUBOOT_HEADS}
	$(CC) -c $(CFLAGS) -o $@ $<

$(OUTPUTDIR):
	mkdir -p $@

#
# Rules to link and convert niuboot image
# 
$(NIUBOOT_BIN): $(NIUBOOT_ELF)
	$(OBJCOPY) -R -S -O binary -R .note -R .note.gnu.build-id -R .comment $< $@

$(NIUBOOT_ELF): $(NIUBOOT_OBJS) $(BOOT_LAYOUT_OUT)
	$(LD) -o $@ $(NIUBOOT_OBJS) $(LDFLAGS)
	$(NM) -n $@ > $@.map

$(BOOT_LAYOUT_OUT): $(BOOT_LAYOUT_IN)
	$(CPP) -P -DBASE_ADDR=$(BASE_ADDR) -o $@ $<

.PHONY: clean
clean:
	@echo Cleaning...
	rm -rf $(NIUBOOT_OBJS) $(BOOT_LAYOUT_OUT)
	@echo Build output:
	rm -rf $(OUTPUTDIR)
