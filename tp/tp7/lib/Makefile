include ../common.mk
PROJECTNAME = util

SRCSDIR = src
SRCSCLASS = Adc.cpp Board.cpp Button.cpp Led.cpp Memory24.cpp Motor.cpp \
			MovementManager.cpp Photoresistance.cpp Pin.cpp Timer.cpp \
			Uart.cpp WatchdogTimer.cpp
SRCS = $(SRCSCLASS:%=$(SRCSDIR)/%)

TRG = lib$(PROJECTNAME).a

RELEASE_OBJDEPS = $(SRCS:%=$(RELEASE_DIR)/%.o)
SIMULATION_OBJDEPS = $(SRCS:%=$(SIMULATION_DIR)/%.o)

CFLAGS += -Iinclude/

.PHONY: all install clean release simulation

all: release simulation
release: $(RELEASE_DIR)/$(TRG)
simulation: $(SIMULATION_DIR)/$(TRG)

$(RELEASE_DIR)/$(TRG): $(RELEASE_OBJDEPS)
	$(AR) rcs -o $@ $(RELEASE_OBJDEPS)

$(SIMULATION_DIR)/$(TRG): $(SIMULATION_OBJDEPS)
	$(AR) rcs -o $@ $(SIMULATION_OBJDEPS)

clean:
	$(REMOVE) -r $(OUT_DIR)