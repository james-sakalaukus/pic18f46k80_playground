# processor type
CHIP = 18f46k80

TOOL_LOC = /opt/microchip/xc8/v1.34/

# directories for C files, H files, and build outputs
SRC_DIR = ./src/
INCLUDES = -I./inc/
INCLUDES += -I$(TOOL_LOC)include/
INCLUDES += -I$(TOOL_LOC)include/plib/
OUTPUT_DIR = ./tmp/

# Output executable name
EXEC = pic18f46k80_test

SOURCES = $(wildcard $(SRC_DIR)*.c)
OBJECTS = $(addprefix $(OUTPUT_DIR), $(notdir $(SOURCES:.c=.p1)))

# generate warnings/errors suitable for CDT parser
IDE_ERRORS = "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"

# Compiler and flags 
CC = $(TOOL_LOC)bin/xc8
CC_FLAGS = --pass1 --chip=$(CHIP) -Q --mode=free --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=elf:multilocs $(IDE_ERRORS)


# linker and flags
LINKER = $(TOOL_LOC)bin/xc8
LINK_FLAGS = --chip=$(CHIP) -m$(EXEC).map --mode=free --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=elf:multilocs $(IDE_ERRORS)

#Programmer Commands
PROGRAM = /opt/microchip/mplabx/mplab_ide/bin/mdb.sh programmerOptions.script


print-%  : ; @echo $* = $($*)

all: $(OUTPUT_DIR)$(EXEC).hex
#	$(PROGRAM)
 
# Main target
$(OUTPUT_DIR)$(EXEC).hex: $(OBJECTS)
	$(LINKER) $(LINK_FLAGS) $(OBJECTS) -o$(OUTPUT_DIR)$(EXEC)

# To obtain object files
$(OUTPUT_DIR)%.p1: $(SRC_DIR)%.c
	$(CC) $(CC_FLAGS) $(INCLUDES) $< -o$@

# To remove generated files
clean:
	rm -f $(EXEC) $(OBJECTS) $(OUTPUT_DIR)*
