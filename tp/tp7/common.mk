# Target specific variables
MCU = atmega324pa
AVRDUDE_PROGRAMMER_ID = usbasp
HEXFORMAT = ihex

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

# Utility flags
CFLAGS = -MMD -g -mmcu=$(MCU) \
	-fshort-enums \
	-funsigned-bitfields -funsigned-char \
	-Wall -DF_CPU=8000000
CXXFLAGS = -fno-exceptions -std=c++14
RELEASE_LDFLAGS = -Wl,-Map,$(RELEASE_DIR)/$(TRG).map -mmcu=$(MCU) -s
RELEASE_CFLAGS = -Os -s
SIMULATION_LDFLAGS = -Wl,-Map,$(SIMULATION_DIR)/$(TRG).map -mmcu=$(MCU)
SIMULATION_CFLAGS = -Os -DSIMULATION=1

# Common target definitions
$(SIMULATION_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(SIMULATION_CFLAGS) -c $< -o $@

$(SIMULATION_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CFLAGS) $(SIMULATION_CFLAGS) $(CXXFLAGS) -c $< -o $@

$(RELEASE_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(RELEASE_CFLAGS) -c $< -o $@

$(RELEASE_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CFLAGS) $(RELEASE_CFLAGS) $(CXXFLAGS) -c $< -o $@

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O $(HEXFORMAT) $< $@