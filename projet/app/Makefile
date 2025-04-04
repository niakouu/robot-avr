include ../common.mk
PROJECTNAME = app

SRCSDIR = src
SRCSCLASS = main.cpp HouseChallengeHandler.cpp Challenge.cpp
SRCS = $(SRCSCLASS:%=$(SRCSDIR)/%)

RELEASE_LIBS = -L../lib/$(RELEASE_DIR) -lutil
SIMULATION_LIBS = -L../lib/$(SIMULATION_DIR) -lutil
INCLUDE = -I../lib/include/ -Iinclude
MAKELIB = make -C ../lib/
SERIEVIAUSB_DIR = ../../../serieViaUSB
PROGMEM_DIR = ../../../progmem

TRG = $(PROJECTNAME).elf
HEXROMTRG = $(PROJECTNAME).hex
HEXTRG = $(HEXROMTRG) $(PROJECTNAME).ee.hex

RELEASE_OBJDEPS = $(SRCS:%=$(RELEASE_DIR)/%.o)
SIMULATION_OBJDEPS = $(SRCS:%=$(SIMULATION_DIR)/%.o)
DEBUG_OBJDEPS = $(SRCS:%=$(DEBUG_DIR)/%.o)

CFLAGS += $(INCLUDE)

.PHONY: all clean libutil release simulation debug install install-debug load progmem serieViaUSB

all: libutil release simulation debug
release: libutil $(RELEASE_DIR)/$(TRG) $(RELEASE_DIR)/$(HEXROMTRG)
debug: libutil $(DEBUG_DIR)/$(TRG) $(DEBUG_DIR)/$(HEXROMTRG)
simulation: libutil $(SIMULATION_DIR)/$(TRG) $(SIMULATION_DIR)/$(HEXROMTRG)

$(RELEASE_DIR)/$(TRG): $(RELEASE_OBJDEPS) libutil
	$(call link_executable,release,$(RELEASE_OBJDEPS),$(RELEASE_LDFLAGS),$(RELEASE_LIBS))

$(DEBUG_DIR)/$(TRG): $(DEBUG_OBJDEPS) libutil
	$(call link_executable,debug,$(DEBUG_OBJDEPS),$(DEBUG_LDFLAGS),$(RELEASE_LIBS))

$(SIMULATION_DIR)/$(TRG): $(SIMULATION_OBJDEPS) libutil
	$(call link_executable,simulation,$(SIMULATION_OBJDEPS),$(SIMULATION_LDFLAGS),$(SIMULATION_LIBS))

libutil:
	$(MAKELIB) $(LIBUTIL_TRG)

progmem:
	make -C $(PROGMEM_DIR)

serieViaUSB:
	make -C $(SERIEVIAUSB_DIR)

install: release
	@echo 'Make sure PB3 and PB4 are unplugged during installation'
	$(AVRDUDE) -c $(AVRDUDE_PROGRAMMER_ID) \
		-p $(MCU) -P usb -e -U flash:w:$(RELEASE_DIR)/$(HEXROMTRG)

install-debug: debug
	@echo 'Make sure PB3 and PB4 are unplugged during installation'
	$(AVRDUDE) -c $(AVRDUDE_PROGRAMMER_ID) \
		-p $(MCU) -P usb -e -U flash:w:$(DEBUG_DIR)/$(HEXROMTRG)

load: install-debug progmem serieViaUSB
ifndef PROGRAM_PATH
	$(error PROGRAM_PATH variable is not set)
endif
	@test -f $(PROGRAM_PATH)
	$(PROGMEM_DIR)/progmem -o "$(OUT_DIR)/program.o" "$(PROGRAM_PATH)"
	$(SERIEVIAUSB_DIR)/serieViaUSB -l &
	@sleep 1
	@bash -c "$(SERIEVIAUSB_DIR)/serieViaUSB -e -f '$(OUT_DIR)/program.o' >/dev/null 2>&1"
	@sleep 10
	@killall serieViaUSB

clean:
	$(REMOVE) -r $(OUT_DIR)
	$(MAKELIB) clean