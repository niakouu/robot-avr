# Target specific variables
MCU = atmega324pa
AVRDUDE_PROGRAMMER_ID = usbasp
HEXFORMAT = ihex
F_CPU = 8'000'000UL

# Commands and compilation variables
CC = avr-gcc
CXX = avr-g++
OBJCOPY = avr-objcopy
AVRDUDE = avrdude
REMOVE = rm -f

# Output directories
OUT_DIR = build
SIMULATION_DIR = $(OUT_DIR)/simulation
RELEASE_DIR = $(OUT_DIR)/release
DEBUG_DIR = $(OUT_DIR)/debug

# Utility flags
CFLAGS = -MMD -g -mmcu=$(MCU) \
	-fshort-enums \
	-funsigned-bitfields -funsigned-char \
	-Wall -DF_CPU=$(F_CPU)
CXXFLAGS = -fno-exceptions -std=c++14
RELEASE_LDFLAGS = -Wl,-Map,$(RELEASE_DIR)/$(TRG).map -mmcu=$(MCU)
RELEASE_CFLAGS = -Os
DEBUG_CFLAGS = $(RELEASE_CFLAGS) -DDEBUG=1
DEBUG_LDFLAGS = -Wl,-Map,$(DEBUG_DIR)/$(TRG).map -mmcu=$(MCU)
SIMULATION_LDFLAGS = -Wl,-Map,$(SIMULATION_DIR)/$(TRG).map -mmcu=$(MCU)
SIMULATION_CFLAGS = -Os -DSIMULATION=1

ifndef VERBOSE
Q = @
endif

# Arg 1: CC/CXX
# Arg 2: Name of build type
# Arg 3: Target specific flags
define compile_specific_target
	@mkdir -p $(dir $@)
	$(Q)$($1) $(CFLAGS) $3 -c $< -o $@
	@echo "$1($2): $<"
endef

# Arg 1: Name of build type
# Arg 2: Object dependencies
define link_library
	$(Q)$(AR) rcs -o $@ $2
	@echo "AR($1): $@"
endef

# Arg 1: Name of build type
# Arg 2: Object dependencies
# Arg 3: Target specific flags
# Arg 4: Extra libraries
define link_executable
	$(Q)$(CC) $3 -o $@ $2 $4 
	@echo "LD($1): $@"
endef

# Common target definitions
$(SIMULATION_DIR)/%.c.o: %.c
	$(call compile_specific_target,CC,simulation,$(SIMULATION_CFLAGS))

$(SIMULATION_DIR)/%.cpp.o: %.cpp
	$(call compile_specific_target,CXX,simulation,$(SIMULATION_CFLAGS) $(CXXFLAGS))

$(RELEASE_DIR)/%.c.o: %.c
	$(call compile_specific_target,CC,release,$(RELEASE_CFLAGS))

$(RELEASE_DIR)/%.cpp.o: %.cpp
	$(call compile_specific_target,CXX,release,$(RELEASE_CFLAGS) $(CXXFLAGS))

$(DEBUG_DIR)/%.c.o: %.c
	$(call compile_specific_target,CC,debug,$(DEBUG_CFLAGS))

$(DEBUG_DIR)/%.cpp.o: %.cpp
	$(call compile_specific_target,CXX,debug,$(DEBUG_CFLAGS) $(CXXFLAGS))

%.hex: %.elf
	@$(OBJCOPY) -j .text -j .data -O $(HEXFORMAT) $< $@
	@echo "OBJCOPY: $< -> $@"