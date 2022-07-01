# Tiva Makefile
# #####################################
#
# Part of the uCtools project
# uctools.github.com
#
#######################################
# user configuration:
#######################################

# TARGET: name of the output file
TARGET = main
# MCU: part number to build for
MCU = TM4C123GH6PM
# SOURCES: list of input source sources
SOURCES = main.c startup_gcc.c PLL.c uart0_debug.c uart1.c systick_delay.c I2C0.c PWM0.c# ir.c ir_remote.c
SUB_MODULES_DIR = Ov7670 # esp8266
# INCLUDES: list of includes, by default, use Includes directory
INCLUDES = -IInclude -I./Ov7670
# OUTDIR: directory to use for output
OUTDIR = build
# TIVAWARE_PATH: path to tivaware folder
TIVAWARE_PATH = ${HOME}/Projects/Tiva/TivaWare_C_Series-2.2.0.295

# LD_SCRIPT: linker script
LD_SCRIPT = $(MCU).ld

# define flags
CFLAGS = -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp 
CFLAGS += -Os -ffunction-sections -fdata-sections -MD -std=c99 -Wall
CFLAGS += -pedantic -DPART_$(MCU) -c -I$(TIVAWARE_PATH)
export CFLAGS += -DTARGET_IS_BLIZZARD_RA1
LDFLAGS = -T $(LD_SCRIPT) --entry ResetISR --gc-sections  

#######################################
# end of user configuration
#######################################
#
#######################################
# binaries
#######################################
export CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy
RM      = rm -f
MKDIR	= mkdir -p
#######################################

# list of object files, placed in the build directory regardless of source path
OBJECTS = $(addprefix $(OUTDIR)/,$(notdir $(SOURCES:.c=.o)))
SUB_MODULES_OBJECTS = $(foreach n,$(SUB_MODULES_DIR),$(n)/*.o) 

all: sub_modules $(OUTDIR)/$(TARGET).bin

sub_modules:
	@for i in ${SUB_MODULES_DIR};\
	do				\
		make -C $${i} || exit $$?;\
	done 
	
$(OUTDIR)/%.o: %.c | $(OUTDIR)	
	$(CC) -o $@ $^ $(CFLAGS)

$(OUTDIR)/a.out: $(OBJECTS) $(SUB_MODULES_OBJECTS)
	$(LD) -o $@ $^ $(LDFLAGS)

$(OUTDIR)/main.bin: $(OUTDIR)/a.out
	$(OBJCOPY) -O binary $< $@

# create the output directory
$(OUTDIR):
	$(MKDIR) $(OUTDIR)

clean:
	@for i in ${SUB_MODULES_DIR}; \
	do							  \
		make -C $${i} clean;      \
	done
	-$(RM) $(OUTDIR)/*

.PHONY: all clean
