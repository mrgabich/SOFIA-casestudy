# Build locally without using a VLNV library
NOVLNV=1
VERBOSE=1


OVP_INCLUDES=-I.
OPTIMISATION?=-O0 -g

AARCH32_CROSSCOMPILER   = $(ARM_TOOLCHAIN_LINUX)
AARCH32_PREFIX= $(ARM_TOOLCHAIN_LINUX_PREFIX)

ifeq ($(CC_TYPE),GCC)
AARCH32_CFLAGS   = -mcpu=cortex-a9 -march=armv7-a -mlittle-endian -static -DUNIX -Wl,-Ttext=0x2FAFAFAF -fcommon
AARCH32_CC       = $(AARCH32_CROSSCOMPILER)/bin/$(AARCH32_PREFIX)-gcc $(AARCH32_CFLAGS) -DAARCH32 $(OPTIMISATION) $(OVP_INCLUDES)
AARCH32_OBJDUMP  = $(AARCH32_CROSSCOMPILER)/bin/$(AARCH32_PREFIX)-objdump
else
AARCH32_CFLAGS   = -mcpu=cortex-a9 -march=armv7-a -mlittle-endian -static -DUNIX -Wl,-Ttext=0x2FAFAFAF -fcommon
AARCH32_CC       = $(AARCH32_CROSSCOMPILER)/bin/clang --target=arm-none-linux-gnueabihf $(AARCH32_CFLAGS) -DAARCH32 $(OPTIMISATION) $(OVP_INCLUDES)
AARCH32_OBJDUMP  = $(AARCH32_CROSSCOMPILER)/bin/$(AARCH32_PREFIX)-objdump
endif

AARCH64_CROSSCOMPILER   = $(AARCH64_TOOLCHAIN_LINUX)
AARCH64_PREFIX= $(AARCH64_TOOLCHAIN_LINUX_PREFIX)

ifeq ($(CC_TYPE),GCC)
AARCH64_CFLAGS   = -mcpu=cortex-a53 -march=armv8-a -mlittle-endian -static -DUNIX -Wl,-Ttext=0x08000000 -fcommon
AARCH64_CC       = $(AARCH64_CROSSCOMPILER)/bin/$(AARCH64_PREFIX)-gcc $(AARCH64_CFLAGS) -DARM_CORTEX_A53 $(OPTIMISATION) $(OVP_INCLUDES)
AARCH64_OBJDUMP  = $(AARCH64_CROSSCOMPILER)/bin/$(AARCH64_PREFIX)-objdump
else
AARCH64_CFLAGS   = -mcpu=cortex-a53 -march=armv8-a -mlittle-endian -static -DUNIX -Wl,-Ttext=0x08000000 -fcommon
AARCH64_CC       = $(AARCH64_CROSSCOMPILER)/bin/clang --target=aarch64-none-linux-gnu $(AARCH64_CFLAGS) -DARM_CORTEX_A53 $(OPTIMISATION) $(OVP_INCLUDES)
AARCH64_OBJDUMP  = $(AARCH64_CROSSCOMPILER)/bin/$(AARCH64_PREFIX)-objdump
endif

SRC?=OVPFIM-ARMv7.c OVPFIM-ARMv8.c
EXE=$(patsubst %.c,%.elf,$(SRC))

all: $(EXE)
ovparmv8: clean OVPFIM-ARMv8.elf
ovparmv7: clean OVPFIM-ARMv7.elf

OVPFIM-ARMv7.elf: OVPFIM-ARMv7.c
	@    echo "# Compiling $<"
	$(V) $(AARCH32_CC) -o $@ $<  
	$(V) $(AARCH32_OBJDUMP) -d $@ > app.ARMv7.lst

OVPFIM-ARMv8.elf: OVPFIM-ARMv8.c
	@    echo "# Compiling $<"
	$(V) $(AARCH64_CC) -o $@ $<  
	$(V) $(AARCH64_OBJDUMP) -d $@ > app.ARMv8.lst


clean::
	- rm -f *.elf
	
