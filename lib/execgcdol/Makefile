# see the readme for instructions: https://github.com/systemwii/make

# --- target ---
TARGET		:=	execgcdol
TYPE		:=  a+h
PLATFORM	:=	wii
BUILD		:=	_
CACHE		:=	_/cache

# --- sources ---
SRCS		 =	src ../monke/sha1
SRCEXTS		 =	.c
BINS		 =  $(wildcard lib/*/_)
BINEXTS		 =  .dol
LIBS		 =	
LIBDIRSBNDLE =	
LIBDIRSLOOSE =	
INCLUDES	 =	

# --- flags ---
CFLAGS		 =	-save-temps -g -O2 -Wall -Wno-unused-variable
CXXFLAGS	 =	$(CFLAGS)
ASFLAGS		 =	-D_LANGUAGE_ASSEMBLY
LDFLAGS		 =	-g -Wl,-Map,$(CACHE)/$(notdir $@).map
ARFLAGS		 =	rcs

# --- runs the templates ---
$(if $(findstring /,$(DEVKITPPC)),,$(error DEVKITPPC not set; run: export DEVKITPPC=<path to>devkitPPC))
RULESDIR	:=	../make/rules
include $(RULESDIR)/main.mk
