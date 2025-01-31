# Config variables
MAKE=make

# Default / basic targets
all:
	$(error Must set a target)

clean:
	rm -rf build

.PHONY: all clean

# Project definitions
PROJECTS=tp1 tp2pb1 tp2pb2 tp3led tp3motor
tp1%: SRCS=tp/tp1/delControl.cpp
tp2pb1%: SRCS=tp/tp2/pb1/pb1.cpp
tp2pb2%: SRCS=tp/tp2/pb2/pb2.cpp
tp3led%: SRCS=tp/tp3/led/led.cpp
tp3motor%: SRCS=tp/tp3/motor/motor.cpp

%: PROJ=$(firstword $(subst -, ,$@))

# Generic targets for projects
$(PROJECTS): %: %-sim %-release

$(PROJECTS:%=%-sim):
	$(MAKE) -f build.mk srcs="$(SRCS)" proj="$(PROJ)" config=sim

$(PROJECTS:%=%-release):
	$(MAKE) -f build.mk srcs="$(SRCS)" proj="$(PROJ)" config=release

$(PROJECTS:%=%-install):
	$(MAKE) -f build.mk srcs="$(SRCS)" proj="$(PROJ)" config=release install