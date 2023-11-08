IMPERAS_HOME := $(shell getpath.exe "$(IMPERAS_HOME)")

# Build locally without using a VLNV library
NOVLNV=1
VERBOSE=1

APP_ROOT?=$(WORKING_FOLDER)

CFLAGS= $(MAKEFILE_CFLAGS) -I$(APP_ROOT)/

MULT?=
DEF?=

TARGET:=    app$(MULT).$(CROSS).elf
TARGET_LST:=app$(MULT).$(CROSS).lst
TARGET_BIN:=app$(MULT).$(CROSS).bin
TARGET_HEX:=app$(MULT).$(CROSS).hex
TARGET_MAP:=app$(MULT).$(CROSS).map

SRC?=$(wildcard *.c)
ASRC?=$(wildcard *.s)
SRCPP?=$(wildcard *.cpp)
SRCINC?=
SSDBG?=
ASSCOBJ=$(patsubst %.s,%.o,$(ASRC))
EXE=$(patsubst %.c,%.o,$(SRC))
EXEPP=$(patsubst %.cpp,%.o,$(SRCPP))
INCOBJ=$(patsubst %.c,%$(MULT).$(CROSS).o,$(SRCINC))

-include $(BAREMETAL_FOLDER)/$(CROSS).makefile.include
-include Makefile.include

# ---------------------------------------------------------------------------------------
# gcc
ifeq ($(CC_TYPE),GCC)
ifeq ($(wildcard *.cpp),)
all: clean app$(MULT).$(CROSS).elf

app$(MULT).$(CROSS).elf: $(EXE) $(INCOBJ) $(ASSCOBJ)
	@    echo "# Linking $(CROSS) $@"
	$(V) $(IMPERAS_LINK) -o $@ $^
	$(V) $(IMPERAS_OBJDUMP) -d $@ > app.FIM_log.lst
	$(V) $(IMPERAS_OBJDUMP) -j .text -j .init -j .fini -j .rodata -j .data -j .bss -d $@ > app.lst

%.o: %.c
	@    echo "# Compiling $<"
	$(V) $(IMPERAS_CC) -c -o $@ $< -D$(CROSS) $(DEF) -std=gnu99 $(CFLAGS)

%.o: %.s
	@    echo "# Compiling $<"
	$(V) $(IMPERAS_AS) -o $@ $< $(ASSDBG)

%$(MULT).$(CROSS).o: %.c
	@    echo "# Compiling $(CROSS) $<"
	$(V) $(IMPERAS_CC) -c -o $@ $< -D$(CROSS) $(DEF) $(CFLAGS)
endif
ifeq ($(wildcard *.c),)
all: clean app$(MULT).$(CROSS).elf

app$(MULT).$(CROSS).elf: $(EXEPP) $(INCOBJ) $(ASSCOBJ)
	@    echo "# Linking $(CROSS) $@"
	$(V) $(IMPERAS_LINKXX) -o $@ $^
	$(V) $(IMPERAS_OBJDUMP) -d $@ > app.FIM_log.lst
	$(V) $(IMPERAS_OBJDUMP) -j .text -j .init -j .fini -j .rodata -j .data -j .bss -d $@ > app.lst

%.o: %.cpp
	@    echo "# Compiling $<"
	$(V) $(IMPERAS_CXX) -lstdc++ -c -o $@ $< -D$(CROSS) $(DEF) $(CFLAGS)

%.o: %.s
	@    echo "# Compiling $<"
	$(V) $(IMPERAS_AS) -o $@ $< $(ASSDBG)

%$(MULT).$(CROSS).o: %.c
	@    echo "# Compiling $(CROSS) $<"
	$(V) $(IMPERAS_CXX) -c -o $@ $< -D$(CROSS) $(DEF) $(CFLAGS)

endif
# ---------------------------------------------------------------------------------------
# clang
else
ifeq ($(wildcard *.cpp),)
all: clean $(TARGET_LST) $(TARGET_HEX)

%.o: %.c
	@echo Compiling $^
	@mkdir -p $(dir $@)
	$(V)$(IMPERAS_CC) -o $@ $(CFLAGS) -std=gnu99 -c $<

$(TARGET): $(EXE)
	@echo Linking $@
	$(V)$(IMPERAS_LINK) -o $@ $(IMPERAS_LDFLAGS) -lm -lc -std=gnu99 $(CFLAGS) $(EXE)

$(TARGET_LST): $(TARGET)
	@echo DBG Application $@
	$(V)$(IMPERAS_OBJDUMP) --disassemble-all -j .text -j .init -j .fini -j .rodata -j .data -j .bss -j .heap -j .comment -D $< > $@

$(TARGET_BIN): $(TARGET)
	@echo Linking $@
	$(V)$(IMPERAS_OBJCOPY) -O binary $< $@

$(TARGET_HEX): $(TARGET)
	@echo HEX to $@
	$(V)$(IMPERAS_OBJCOPY) -O verilog $< $@
endif
ifeq ($(wildcard *.c),)
all: clean $(TARGET_LST) $(TARGET_HEX)

%.o: %.cpp
	@echo Compiling $^
	@mkdir -p $(dir $@)
	$(V)$(IMPERAS_CXX) -o $@ $(CFLAGS) -c $<

$(TARGET): $(EXEPP)
	@echo Linking $@
	$(V)$(IMPERAS_LINKXX) -o $@ $(IMPERAS_LDFLAGS) -lm -lc $(CFLAGS) $(EXEPP)

$(TARGET_LST): $(TARGET)
	@echo DBG Application $@
	$(V)$(IMPERAS_OBJDUMP) --disassemble-all -j .text -j .init -j .fini -j .rodata -j .data -j .bss -j .heap -j .comment -D $< > $@

$(TARGET_BIN): $(TARGET)
	@echo Linking $@
	$(V)$(IMPERAS_OBJCOPY) -O binary $< $@

$(TARGET_HEX): $(TARGET)
	@echo HEX to $@
	$(V)$(IMPERAS_OBJCOPY) -O verilog $< $@
endif
endif

# ---------------------------------------------------------------------------------------

clean::
	- rm -f *.$(CROSS).elf *.$(CROSS).o 