NULL=

PREFIX=/usr
EXEC_PREFIX=$(PREFIX)
LIB_DIR = $(PREFIX)/lib64
INCLUDE_DIR = $(PREFIX)/include
SHARE=/usr/share
GIR_DIR = $(SHARE)/gir-1.0
PKG_DIR = $(SHARE)/pkgconfig
TYPELIB_DIR = $(LIB_DIR)/girepository-1.0
DEPS_DIR = $(SHARE)/vala/vapi

SRC_DIR = retro

OUT_DIR = out
VAPI_DIR = vapi

NAME=Retro
DESC=GObject based libretro wrapper
LIBNAME=retro-gobject
VERSION=1.0
PKGNAME=$(LIBNAME)-$(VERSION)

SO=      lib$(LIBNAME).so
H=       $(LIBNAME).h
VAPI=    $(PKGNAME).vapi
DEPS=    $(PKGNAME).deps
GIR=     Retro-$(VERSION).gir
TYPELIB= Retro-$(VERSION).typelib
PKGCONF= $(PKGNAME).pc

FILES= \
	Audio.vala \
	AudioInput.vala \
	Camera.vala \
	Core.vala \
	Device.vala \
	DiskController.vala \
	FrameTime.vala \
	GameInfo.vala \
	GameType.vala \
	HardwareRender.vala \
	Input.vala \
	Keyboard.vala \
	Language.vala \
	Location.vala \
	Log.vala \
	MemoryType.vala \
	Module.vala \
	Performance.vala \
	Region.vala \
	Retro.vala \
	Rumble.vala \
	Sensor.vala \
	SystemAvInfo.vala \
	SystemInfo.vala \
	Variables.vala \
	Video.vala \
	retro-core-cb-data.c \
	retro-environment.c \
	retro-environment-interfaces.c \
	$(NULL)

PKG= \
	gmodule-2.0 \
	gio-2.0 \
	$(NULL)

SRC = $(FILES:%=$(SRC_DIR)/%)

OUT= \
	$(OUT_DIR)/$(SO) \
	$(OUT_DIR)/$(VAPI) \
	$(OUT_DIR)/$(GIR) \
	$(OUT_DIR)/$H \
	$(NULL)

VALAC_OPTIONS= --save-temps

all: $(OUT) $(OUT_DIR)/$(DEPS) $(OUT_DIR)/$(TYPELIB) $(OUT_DIR)/$(PKGCONF)

$(OUT): %: $(SRC)
	mkdir -p $(@D)
	valac \
		-b $(<D) -d $(@D) \
		--library=$(LIBNAME) \
		--vapi=$(VAPI) \
		--gir=$(GIR) \
		-H $(@D)/$H \
		-h $(<D)/$(LIBNAME)-internal.h \
		-o $(SO) $^ \
		$(PKG:%=--pkg=%) \
		-X -fPIC -X -shared \
		$(VALAC_OPTIONS) \
		$(NULL)
	@touch $@

$(OUT_DIR)/$(DEPS):
	mkdir -p $(@D)
	# Split deps into lines
	echo $(PKG) | sed -e 's/\s\+/\n/g' > $@

$(OUT_DIR)/$(TYPELIB):
	g-ir-compiler --shared-library $(LIBNAME) --output $@ $(@D)/$(GIR)

$(OUT_DIR)/$(PKGCONF):
	echo "prefix="$(PREFIX) > $@
	echo "exec_prefix="$(EXEC_PREFIX) >> $@
	echo "libdir="$(LIB_DIR) >> $@
	echo "includedir="$(INCLUDE_DIR) >> $@
	echo >> $@
	echo "Name: "$(NAME) >> $@
	echo "Description: "$(DESC) >> $@
	echo "Version: "$(VERSION) >> $@
	echo "Requires: "$(PKG) >> $@
	echo "Libs: -L$$""{libdir} -lretro-gobject" >> $@
	echo "Cflags: -I$$""{includedir}" >> $@

install:
	install $(OUT_DIR)/$(SO)      $(LIB_DIR)
	install $(OUT_DIR)/$H         $(INCLUDE_DIR)
	install $(OUT_DIR)/$(GIR)     $(GIR_DIR)
	install $(OUT_DIR)/$(TYPELIB) $(TYPELIB_DIR)
	install $(OUT_DIR)/$(PKGCONF) $(PKG_DIR)
	install $(OUT_DIR)/$(DEPS)    $(DEPS_DIR)
	install $(OUT_DIR)/$(VAPI)    $(DEPS_DIR)

clean:
	rm -Rf $(OUT_DIR) $(SRC_DIR)/$(LIBNAME)-internal.h

.PHONY: all install clean

