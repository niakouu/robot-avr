include ../common.mk
PROJECTNAME=exec

SRCS=main.cpp
LIBS=-L../lib/build/ -lutil
INCLUDE=-I../lib/include/
MAKELIB=make -C ../lib/

TRG=$(PROJECTNAME).elf
HEXROMTRG=$(PROJECTNAME).hex
HEXTRG=$(HEXROMTRG) $(PROJECTNAME).ee.hex

OBJDEPS=$(patsubst %,$(OUTDIR)/%.o,$(SRCS))

CFLAGS+= $(INCLUDE)

.PHONY: all install clean libutil

all: libutil $(OUTDIR)/$(TRG) $(OUTDIR)/$(HEXROMTRG)

$(OUTDIR)/$(TRG): $(OBJDEPS)
	$(CC) $(LDFLAGS) -o $@ $(OBJDEPS) \
	-lm $(LIBS)

libutil:
	$(MAKELIB)

install: libutil $(OUTDIR)/$(HEXROMTRG)
	$(AVRDUDE) -c $(AVRDUDE_PROGRAMMER_ID) \
	-p $(MCU) -P usb -e -U flash:w:$(OUTDIR)/$(HEXROMTRG)

clean:
	$(REMOVE) -r $(OUTDIR)
	$(MAKELIB) clean