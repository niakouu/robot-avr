# Config variables
MAKE=make

# Default / basic targets
all:
	$(error Must set a target)

clean:
	rm -rf build
	make -C libutil clean

.PHONY: all clean

# Project definitions
PROJECTS=tp1 tp2pb1 tp2pb2 tp3led tp3motor tp4led tp4game tp4motor tp5eeprom_storage
tp1%: SRCS=tp/tp1/delControl.cpp
tp2pb1%: SRCS=tp/tp2/pb1/pb1.cpp
tp2pb2%: SRCS=tp/tp2/pb2/pb2.cpp
tp3led%: SRCS=tp/tp3/led/led.cpp
tp3motor%: SRCS=tp/tp3/motor/motor.cpp
tp4led%: SRCS=tp/tp4/led/led.cpp
tp4game%: SRCS=tp/tp4/game/game.cpp
tp4motor%: SRCS=tp/tp4/motor/motor.cpp
tp5eeprom_storage%: SRCS=tp/tp5/eeprom_storage/eeprom_storage.cpp

%: PROJ=$(firstword $(subst -, ,$@))

# Generic targets for projects
$(PROJECTS): %: %-sim %-release

$(PROJECTS:%=%-sim):
	$(MAKE) -f build.mk srcs="$(SRCS)" proj="$(PROJ)" config=sim

$(PROJECTS:%=%-release):
	$(MAKE) -f build.mk srcs="$(SRCS)" proj="$(PROJ)" config=release

$(PROJECTS:%=%-install):
	$(MAKE) -f build.mk srcs="$(SRCS)" proj="$(PROJ)" config=release install