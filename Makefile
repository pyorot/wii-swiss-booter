# see the readme for instructions: https://github.com/systemwii/make

# --- target ---
TARGET		:=	boot
TYPE		:=  dol
PLATFORM	:=	wii
BUILD		:=	_
CACHE		:=	_/cache

# --- sources ---
SRCS		 =	src lib/monke/console
SRCEXTS		 =	.c
BINS		 =	$(wildcard lib/*/_)
BINEXTS		 =	.dol
LIBS		 =	-lsystitver -logc -lfat
LIBDIRSBNDLE =	$(wildcard lib/*/_)
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
RULESDIR	:=	lib/make/rules
include $(RULESDIR)/main.mk

# --- custom targets ---
dist:
	@mkdir -p _/temp/apps/Swiss ; \
	cd _/temp ; \
	cp -f ../boot.dol apps/Swiss && { \
		rm -f ../wii-swiss-booter.7z ; \
		cp -f ../../meta/* apps/Swiss ; \
		7z a ../wii-swiss-booter apps | grep -e "archive:" -e "Ok" ; \
	} ; \
	cd ../.. ; \
	rm -r _/temp ;
