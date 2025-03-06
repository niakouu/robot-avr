MCU=atmega324pa
AVRDUDE_PROGRAMMER_ID=usbasp

CC=avr-gcc
OBJCOPY=avr-objcopy
AVRDUDE=avrdude
REMOVE=rm -f
HEXFORMAT=ihex
OUTDIR=build

CFLAGS=-I. -I/usr/include/simavr -MMD $(INC) -g -mmcu=$(MCU) -O$(OPTLEVEL) \
	-std=c++14 -fpack-struct -fshort-enums \
	-funsigned-bitfields -funsigned-char \
	-Wall
CXXFLAGS=-fno-exceptions
LDFLAGS=-Wl,-Map,$(OUTDIR)/$(TRG).map -mmcu=$(MCU)

$(OUTDIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUTDIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CXXFLAGS) -c $< -o $@

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O $(HEXFORMAT) $< $@