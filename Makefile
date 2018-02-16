DO_LOGGING := 1

#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITPPC)),)
$(error "Please set DEVKITPPC in your environment. export DEVKITPPC=<path to>devkitPPC)
endif
ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>devkitPRO")
endif

include $(DEVKITPPC)/wii_rules

export PORTLIBS		:=	$(DEVKITPRO)/portlibs/ppc

#---------------------------------------------------------------------------------
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
# DATA is a list of directories containing binary files
# LIBDIR is where the built library will be placed
# all directories are relative to this makefile
#---------------------------------------------------------------------------------
BUILD		?=	release
SOURCES		:=	source \
				source/utils \
				source/network \
				source/patcher \
				source/config \
INCLUDES	:=	include
DATA		:=
LIB		:=	lib

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
CFLAGS	= -g -O2 -Wall -D__wiiu__ $(MACHDEP) $(INCLUDE)
CXXFLAGS	=	$(CFLAGS) -D_GNU_SOURCE

ifeq ($(DO_LOGGING), 1)
   CFLAGS += -D__LOGGING__
   CXXFLAGS += -D__LOGGING__
endif

ASFLAGS	:=	-g

export WIIUBIN	:=	$(LIB)/libcontrollerpatcher.a

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS	:= -ldynamiclibs -lutils

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:=

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export TOPDIR ?= $(CURDIR)/..

export DEPSDIR := $(CURDIR)/$(BUILD)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(DATA),$(CURDIR)/$(dir))

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))


export OFILES	:=	$(addsuffix .o,$(BINFILES)) \
			$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD) -I$(PORTLIBS)/include -I$(PORTLIBS)/include/libutils
			
			

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib) $(PORTLIBS)/lib

.PHONY: $(BUILD) clean

#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr debug release bin obj $(LIB) include

all: $(WIIUBIN)

install:
	@cp $(BUILD)/lib/libcontrollerpatcher.a $(PORTLIBS)/lib
	@mkdir -p $(PORTLIBS)/include/controller_patcher
	@cp source/ControllerPatcher.hpp $(PORTLIBS)/include/controller_patcher
	@cp source/ControllerPatcherDefs.h $(PORTLIBS)/include/controller_patcher

#---------------------------------------------------------------------------------
else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(WIIUBIN)	:	$(OFILES) $(LIB)
	@rm -f "$(WIIUBIN)"
	@$(AR) rcs "$(WIIUBIN)" $(OFILES)
	@echo built ... $(notdir $@)

$(LIB):
	mkdir $(LIB)

-include $(DEPENDS)

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------

