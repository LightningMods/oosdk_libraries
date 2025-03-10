#---------------------------------------------------------------------------------
# Clear the implicit built in rules
#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(OO_PS4_TOOLCHAIN)),)
$(error "Please set OO_PS4_TOOLCHAIN in your environment. export OO_PS4_TOOLCHAIN=<path>")
endif

include $(OO_PS4_TOOLCHAIN)/build_rules.mk

#---------------------------------------------------------------------------------
ifeq ($(strip $(PLATFORM)),)
#---------------------------------------------------------------------------------
export BASEDIR		:= $(CURDIR)
export DEPS			:= $(BASEDIR)/deps
export LIBS			:=	$(BASEDIR)/lib

#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------

export LIBDIR		:= $(LIBS)/$(PLATFORM)
export DEPSDIR		:=	$(DEPS)/$(PLATFORM)

#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

TARGET		:=	libpng
BUILD		:=	build
SOURCE		:=	source
INCLUDE		:=	include
DATA		:=	data
LIBS		:=	 

CFLAGS      += -cc1 -triple x86_64-pc-freebsd-elf -munwind-tables -fuse-init-array -emit-obj -isysroot $(OO_PS4_TOOLCHAIN) -isystem $(OO_PS4_TOOLCHAIN)/include $(INCLUDES)
CXXFLAGS    += $(CFLAGS) -isystem $(OO_PS4_TOOLCHAIN)/include/c++/v1

ifneq ($(BUILD),$(notdir $(CURDIR)))

export OUTPUT	:=	$(CURDIR)/$(TARGET)
export VPATH	:=	$(foreach dir,$(SOURCE),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir))
export BUILDDIR	:=	$(CURDIR)/$(BUILD)
export DEPSDIR	:=	$(BUILDDIR)

CFILES		:= $(foreach dir,$(SOURCE),$(notdir $(wildcard $(dir)/*.c)))
CXXFILES	:= $(foreach dir,$(SOURCE),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:= $(foreach dir,$(SOURCE),$(notdir $(wildcard $(dir)/*.S)))
BINFILES	:= $(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.bin)))

export OFILES	:=	$(CFILES:.c=.o) \
					$(CXXFILES:.cpp=.o) \
					$(SFILES:.S=.o) \
					$(BINFILES:.bin=.bin.o)

export BINFILES	:=	$(BINFILES:.bin=.bin.h)

export INCLUDES	=	$(foreach dir,$(INCLUDE),-I$(CURDIR)/$(dir)) \
					-I$(CURDIR)/$(BUILD) -I$(OO_PS4_TOOLCHAIN)/include

.PHONY: $(BUILD) install clean

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@make --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile
	
install: $(BUILD)
	@echo Copying...
	@cp -frv include/png.h $(OO_PS4_TOOLCHAIN)/include
	@cp -frv include/pngconf.h $(OO_PS4_TOOLCHAIN)/include
	@cp -frv include/pnglibconf.h $(OO_PS4_TOOLCHAIN)/include
	@cp -frv include/pngstruct.h $(OO_PS4_TOOLCHAIN)/include
	@cp -frv include/pnginfo.h $(OO_PS4_TOOLCHAIN)/include
	@cp -frv $(TARGET).a $(OO_PS4_TOOLCHAIN)/lib
	@echo lib installed!
clean:
	@echo Clean...
	@rm -rf $(BUILD) $(OUTPUT).elf $(OUTPUT).self $(OUTPUT).a

else

DEPENDS	:= $(OFILES:.o=.d)

$(OUTPUT).a: $(OFILES)
$(OFILES): $(BINFILES)

-include $(DEPENDS)

endif
