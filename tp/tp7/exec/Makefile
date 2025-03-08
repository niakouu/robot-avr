include ../common.mk
PROJECTNAME = exec

SRCS = main.cpp
RELEASE_LIBS = -L../lib/$(RELEASE_DIR) -lutil
SIMULATION_LIBS = -L../lib/$(SIMULATION_DIR) -lutil
INCLUDE = -I../lib/include/
MAKELIB = make -C ../lib/

TRG = $(PROJECTNAME).elf
HEXROMTRG = $(PROJECTNAME).hex
HEXTRG = $(HEXROMTRG) $(PROJECTNAME).ee.hex

RELEASE_OBJDEPS = $(SRCS:%=$(RELEASE_DIR)/%.o)
SIMULATION_OBJDEPS = $(SRCS:%=$(SIMULATION_DIR)/%.o)

CFLAGS += $(INCLUDE)

.PHONY: all install clean libutil release simulation

all: libutil release simulation
release: libutil $(RELEASE_DIR)/$(TRG) $(RELEASE_DIR)/$(HEXROMTRG)
simulation: libutil $(SIMULATION_DIR)/$(TRG) $(SIMULATION_DIR)/$(HEXROMTRG)

$(RELEASE_DIR)/$(TRG): $(RELEASE_OBJDEPS)
	$(CC) $(RELEASE_LDFLAGS) -o $@ $(RELEASE_OBJDEPS) -lm $(RELEASE_LIBS)

$(SIMULATION_DIR)/$(TRG): $(SIMULATION_OBJDEPS)
	$(CC) $(SIMULATION_LDFLAGS) -o $@ $(SIMULATION_OBJDEPS) -lm $(SIMULATION_LIBS)

libutil:
	$(MAKELIB) $(LIBUTIL_TRG)

install: release
	echo 'Make sure PB3 and PB4 are unplugged during installation'
	$(AVRDUDE) -c $(AVRDUDE_PROGRAMMER_ID) \
		-p $(MCU) -P usb -e -U flash:w:$(RELEASE_DIR)/$(HEXROMTRG)

clean:
	$(REMOVE) -r $(OUT_DIR)
	$(MAKELIB) clean