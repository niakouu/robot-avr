########       AVR Project Makefile       ########
#####                                        #####
#####      Makefile produit et ecrit par     #####
#####   Simon Barrette & Jerome Collin pour  #####
#####           INF1900 - 2016               #####
#####                                        #####
#####         Inspire de Pat Deegan -        #####
#####  Psychogenic Inc (www.psychogenic.com) #####
##################################################

# Ce Makefile vous permet de compiler des projets
# pour les microcontroleurs Atmel AVR sur 
# Linux ou Unix, en utilisant l'outil AVR-GCC. 
# Ce Makefile supporte C & C++


#####      Details specifique a la cible       #####
#####  Vous devez les adapter a votre projet   #####

# Nom du microcontroleur cible
# (exemple: 'at90s8515')
MCU=atmega324pa

# Nom de votre projet
# (utilisez un seul mot, exemple: 'monprojet')
PROJECTNAME=util

# Fichiers sources
# Utilisez le suffixe .cpp pour les fichiers C++
# Listez tous les fichiers a compiler, separes par
# un espace. exemple: 'tp1.c tp2.cpp':
PRJSRC=src/sleep.cpp src/memory_24.cpp

# Inclusions additionnels (ex: -I/path/to/mydir)
INC=-Iinclude

# Libraires a lier (ex: -lmylib)
LIBS=

# Niveau d'optimization
# Utilisez s (size opt), 1, 2, 3 ou 0 (off)
OPTLEVEL=s

####################################################
#####         Configuration terminee           #####
#####                                          #####
#####  Le reste de cette section contient les  #####
##### details d'implementation vous permettant #####
##### de mieux comprendre le fonctionnement de #####
#####   ce Makefile en vue de sa modification  #####
####################################################



####### variables #######

#compilateur utilise
CC=avr-gcc
#pour copier le contenu d'un fichier objet vers un autre
AR=avr-ar
#pour supprimer les fichiers lorsque l'on appel make clean
REMOVE=rm -rf

RELEASE_DIR=release
SIM_DIR=sim

####### Options de compilation #######

# Flags pour le compilateur en C
CFLAGS = -I. -MMD $(INC) -g -mmcu=$(MCU) -O$(OPTLEVEL) \
	-fpack-struct -fshort-enums \
	-funsigned-bitfields -funsigned-char \
	-Wall

# Flag supplémentaire pour retirer le bogue de "array subscript 0 is outside 
# array bounds" sur avr-gcc v12
GCCVERSION := $(shell expr `$(CC) -dumpversion | cut -f1 -d.` \>= 12)
ifeq "$(GCCVERSION)" "1"
    CFLAGS += --param=min-pagesize=0
endif

# Flags pour le compilateur en C++
CXXFLAGS = -fno-exceptions -std=c++14

# Linker pour lier les librairies utilisees
LDFLAGS = -Wl,-Map,$(OUTDIR)/$(TRG).map -mmcu=$(MCU) 

# Build configurations
ifeq ($(config), sim)
	CFLAGS += -DSIMULATION=1
	OUTDIR := $(SIM_DIR)
else ifeq ($(config), release)
	OUTDIR := $(RELEASE_DIR)
endif

####### Cible (Target) #######

#Nom des cibles par defaut
TRG=lib$(PROJECTNAME).a

####### Definition de tout les fichiers objets #######

# Cette fonction permet de differencier les fichiers .c
# des fichiers .cpp
# Fichier C
CFILES=$(filter %.c, $(PRJSRC))
# Fichier C++
CPPFILES=$(filter %.cpp, $(PRJSRC))

# Liste de tout les fichiers objet que nous devons creer
OBJDEPS=$(patsubst %.c,$(OUTDIR)/%.o,$(CFILES)) \
	$(patsubst %.cpp,$(OUTDIR)/%.o,$(CPPFILES))

# Pour plus d'information sur cette section, consulter :
# http://bit.ly/257R53E	
# Les fonctions $(filter pattern…,text) &
# $(patsubst pattern,replacement,text) sont pertinentes
	


####### Creation des commandes du Makefile ####### 

# Creation des cibles Phony (Phony Target)
# En plus de la commande make qui permet de compiler
# votre projet, vous pouvez utilisez les commandes
# make all, make install et make clean
.PHONY: all install clean sim release

# Make all permet simplement de compiler le projet
#
ifdef config
all: $(OUTDIR)/$(TRG)
sim release:
	$(error Cannot build sim or release targets directly with config variables)
else
all: sim release
sim:
	make config=sim
release:
	make config=release
endif


# Implementation de la cible
$(OUTDIR)/$(TRG): $(OBJDEPS)
	$(AR) rcs -o $(OUTDIR)/$(TRG) $(OBJDEPS)

# Production des fichiers object
# De C a objet
$(OUTDIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@
# De C++ a objet
$(OUTDIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CXXFLAGS) -c $< -o $@

# Verification des dependances (header dependencies)
-include *.d

# Pour plus d'information sur cette section, consulter:
# http://bit.ly/2580FU8

# Make install permet de compiler le projet puis
# d'ecrire le programme en memoire flash dans votre
# microcontroleur. Celui-ci doit etre branche par cable USB
ifeq ($(config), release)
install: $(RELEASE_DIR)/$(HEXROMTRG)
	$(AVRDUDE) -c $(AVRDUDE_PROGRAMMERID) \
	-p $(MCU) -P usb -e -U flash:w:$(RELEASE_DIR)/$(HEXROMTRG)
else ifdef config
install:
	$(error Must be in release mode to install)
else
install:
	make config=release install
endif

# Make clean permet d'effacer tout les fichiers generes
# lors de la compilation
clean:
	$(REMOVE) $(TRG) $(TRG).map $(OBJDEPS) $(HEXTRG) *.d $(RELEASE_DIR)/ $(SIM_DIR)/

# Pour plus d'information sur les phony target, consulter:
# http://bit.ly/1WBQe61

# De plus, pour mieux comprendre les makefiles et 
# leur fonctionnement, consulter la documentation de GNU Make:
# http://bit.ly/23Vpk8s

# Finalement, ce tutoriel en ligne constitut une bonne 
# introduction au Makefile:
# http://bit.ly/1XvxsN3

#####                    EOF                   #####