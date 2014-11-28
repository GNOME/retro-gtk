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

SRC_DIR = retro-gtk

OUT_DIR = out
VAPI_DIR = vapi

NAME=RetroGtk
DESC=Library for integrating Retro into GTK+
LIBNAME=retro-gtk
VERSION=1.0
PKGNAME=$(LIBNAME)-$(VERSION)

SO=      lib$(LIBNAME).so
H=       $(LIBNAME).h
VAPI=    $(PKGNAME).vapi
DEPS=    $(PKGNAME).deps
GIR=     RetroGtk-$(VERSION).gir
TYPELIB= RetroGtk-$(VERSION).typelib
PKGCONF= $(PKGNAME).pc

FILES= \
	audio/AudioDevice.vala \
	audio/AudioPlayer.vala \
	input/ControllerDevice.vala \
	input/ControllerHandler.vala \
	input/GamepadButtonType.vala \
	input/GamepadView.vala \
	input/Keyboard.vala \
	input/KeyboardBox.vala \
	input/KeyboardGamepadAdapter.vala \
	input/KeyboardGamepadConfiguration.vala \
	input/KeyboardGamepadConfigurationDialog.vala \
	video/Display.vala \
	video/video-converter.c \
	FileStreamLogger.vala \
	Options.vala \
	Runner.vala \
	$(NULL)

PKG= \
	gtk+-3.0 \
	cairo \
	clutter-gtk-1.0 \
	libpulse \
	libpulse-mainloop-glib \
	retro-gobject-1.0 \
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
		-o $(SO) $(SRC) \
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
	echo "Libs: -L$$""{libdir} -lretro-gtk" >> $@
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
	rm -Rf $(OUT_DIR)

.PHONY: all install clean

