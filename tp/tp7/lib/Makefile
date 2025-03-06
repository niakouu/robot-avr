include ../common.mk
PROJECTNAME=util

SRCSDIR=src
SRCSCLASS=Adc.cpp Button.cpp Led.cpp Memory24.cpp Pin.cpp Uart.cpp
SRCS=$(patsubst %,$(SRCSDIR)/%,$(SRCSCLASS))

TRG=lib$(PROJECTNAME).a

OBJDEPS=$(patsubst %,$(OUTDIR)/%.o,$(SRCS))

CFLAGS+= -Iinclude/

.PHONY: all install clean

all: $(OUTDIR)/$(TRG)

$(OUTDIR)/$(TRG): $(OBJDEPS)
	$(AR) rcs -o $@ $(OBJDEPS)

clean:
	$(REMOVE) -r $(OUTDIR)