NULL=

PREFIX=/usr
EXEC_PREFIX=$(PREFIX)
LIB_DIR = $(PREFIX)/lib
INCLUDE_DIR = $(PREFIX)/include
SHARE=/usr/share
GIR_DIR = $(SHARE)/gir-1.0
TYPELIB_DIR = $(LIB_DIR)/girepository-1.0
DEPS_DIR = $(SHARE)/vala/vapi

RETRO_DIR = retro
RETRO_GTK_DIR = retro-gtk
DEMO_DIR = demo

OUT_DIR = out
VAPI_DIR = vapi

RETRO_NAME=Retro
RETRO_GTK_NAME=RetroGtk

RETRO_DESC=GObject based libretro wrapper
RETRO_GTK_DESC=Library for integrating Retro into GTK+
	
RETRO_LIBNAME=retro-gobject
RETRO_GTK_LIBNAME=retro-gtk

RETRO_VERSION=1.0
RETRO_GTK_VERSION=1.0

RETRO_PKGNAME=$(RETRO_LIBNAME)-$(RETRO_VERSION)
RETRO_GTK_PKGNAME=$(RETRO_GTK_LIBNAME)-$(RETRO_GTK_VERSION)

RETRO_SONAME=lib$(RETRO_LIBNAME).so
RETRO_GTK_SONAME=lib$(RETRO_GTK_LIBNAME).so

RETRO_HNAME=$(RETRO_LIBNAME).h
RETRO_GTK_HNAME=$(RETRO_GTK_LIBNAME).h

RETRO_GIRNAME=Retro-$(RETRO_VERSION).gir
RETRO_GTK_GIRNAME=RetroGtk-$(RETRO_GTK_VERSION).gir

RETRO_TYPELIB=Retro-$(RETRO_VERSION).typelib
RETRO_GTK_TYPELIB=RetroGtk-$(RETRO_GTK_VERSION).typelib

RETRO_PKGCONF=$(RETRO_PKGNAME).pc
RETRO_GTK_PKGCONF=$(RETRO_GTK_PKGNAME).pc

RETRO_DOC=$(RETRO_PKGNAME)-doc

DEMO = $(OUT_DIR)/demo

RETRO_FILES= \
	AudioInput.vala \
	AudioHandler.vala \
	Camera.vala \
	CameraBuffer.vala \
	Core.vala \
	CoreDelegates.vala \
	Device.vala \
	DiskController.vala \
	Environment.vala \
	EnvironmentCommand.vala \
	FrameTime.vala \
	GameInfo.vala \
	GameType.vala \
	HardwareRender.vala \
	InputHandler.vala \
	Keyboard.vala \
	Location.vala \
	Log.vala \
	LogLevel.vala \
	MemoryType.vala \
	Message.vala \
	Module.vala \
	PerfCounter.vala \
	PerfLevel.vala \
	Performance.vala \
	PixelFormat.vala \
	Region.vala \
	Retro.vala \
	Rotation.vala \
	Rumble.vala \
	RumbleEffect.vala \
	Sensor.vala \
	SensorAction.vala \
	SensorAccelerometer.vala \
	SimdFlags.vala \
	SystemAvInfo.vala \
	SystemInfo.vala \
	Variable.vala \
	VideoHandler.vala \
	retro-core-cb-data.c \
	retro-core-extern.c \
	retro-core-interfaces.c \
	$(NULL)

RETRO_GTK_FILES= \
	input/ControllerDevice.vala \
	input/ControllerHandler.vala \
	input/GamepadButtonType.vala \
	input/GamepadView.vala \
	input/Keyboard.vala \
	input/KeyboardBox.vala \
	input/KeyboardGamepadAdapter.vala \
	input/KeyboardGamepadConfiguration.vala \
	input/KeyboardGamepadConfigurationDialog.vala \
	AudioDevice.vala \
	Display.vala \
	FileStreamLogger.vala \
	Options.vala \
	Runnable.vala \
	Runner.vala \
	video-converter.c \
	$(NULL)

DEMO_CONFIG_FILE=$(DEMO_DIR)/config.vala

DEMO_FILES= \
	Demo.vala \
	Engine.vala \
	Window.vala \
	OptionsGrid.vala \
	$(NULL)


RETRO_PKG= \
	gmodule-2.0 \
	$(NULL)

RETRO_GTK_PKG= \
	gtk+-3.0 \
	cairo \
	clutter-gtk-1.0 \
	libpulse \
	libpulse-mainloop-glib \
	$(RETRO_PKGNAME) \
	$(NULL)

PKG= \
	$(RETRO_PKGNAME) \
	$(RETRO_GTK_PKGNAME) \
	$(NULL)

RETRO_SRC = $(RETRO_FILES:%=$(RETRO_DIR)/%)
RETRO_GTK_SRC = $(RETRO_GTK_FILES:%=$(RETRO_GTK_DIR)/%)
DEMO_SRC = $(DEMO_FILES:%=$(DEMO_DIR)/%)

RETRO_OUT= \
	$(OUT_DIR)/$(RETRO_SONAME) \
	$(OUT_DIR)/$(RETRO_PKGNAME).vapi \
	$(OUT_DIR)/$(RETRO_GIRNAME) \
	$(OUT_DIR)/$(RETRO_HNAME) \
	$(NULL)

RETRO_GTK_OUT= \
	$(OUT_DIR)/$(RETRO_GTK_SONAME) \
	$(OUT_DIR)/$(RETRO_GTK_PKGNAME).vapi \
	$(OUT_DIR)/$(RETRO_GTK_GIRNAME) \
	$(OUT_DIR)/$(RETRO_GTK_HNAME) \
	$(NULL)

RETRO_DEPS=$(OUT_DIR)/$(RETRO_PKGNAME).deps
RETRO_GTK_DEPS=$(OUT_DIR)/$(RETRO_GTK_PKGNAME).deps

all: demo retro retro-gtk

demo: $(DEMO)
retro: $(RETRO_OUT) $(RETRO_DEPS) $(OUT_DIR)/$(RETRO_TYPELIB) $(OUT_DIR)/$(RETRO_PKGCONF)
retro-gtk: $(RETRO_GTK_OUT) $(RETRO_GTK_DEPS) $(OUT_DIR)/$(RETRO_GTK_TYPELIB) $(OUT_DIR)/$(RETRO_GTK_PKGCONF)
doc: $(RETRO_DOC)

$(DEMO): $(RETRO_SRC) $(RETRO_GTK_SRC) $(DEMO_SRC) $(RETRO_OUT) $(RETRO_DEPS) $(RETRO_GTK_OUT) $(RETRO_GTK_DEPS) $(DEMO_CONFIG_FILE)
	mkdir -p $(OUT_DIR)
	valac -b $(<D) -d $(@D) \
		-o $(@F) $(DEMO_SRC) $(DEMO_CONFIG_FILE) \
		-X -I./$(OUT_DIR) -X $(OUT_DIR)/$(RETRO_GTK_SONAME) -X $(OUT_DIR)/$(RETRO_SONAME) \
		--vapidir=$(VAPI_DIR) --vapidir=$(OUT_DIR) $(PKG:%=--pkg=%) \
		--save-temps \
		-g
	@touch $@

$(DEMO_CONFIG_FILE):
	echo "const string PREFIX = \""$(PREFIX)\"";" > $@

$(RETRO_OUT): %: $(RETRO_SRC)
	mkdir -p $(@D)
	valac \
		-b $(<D) -d $(@D) \
		--library=$(RETRO_LIBNAME) \
		--vapi=$(RETRO_PKGNAME).vapi \
		--gir=$(RETRO_GIRNAME) \
		-H $(@D)/$(RETRO_HNAME) \
		-h $(<D)/$(RETRO_LIBNAME)-internal.h \
		-o $(RETRO_SONAME) $^ \
		--vapidir=$(VAPI_DIR) $(RETRO_PKG:%=--pkg=%) \
		--save-temps \
		-X -fPIC -X -shared
	@touch $@

$(RETRO_DEPS):
	mkdir -p $(@D)
	echo $(RETRO_PKG) | sed -e 's/\s\+/\n/g' > $@

$(OUT_DIR)/$(RETRO_TYPELIB):
	g-ir-compiler --shared-library $(RETRO_LIBNAME) --output $@ $(@D)/$(RETRO_GIRNAME)

$(OUT_DIR)/$(RETRO_PKGCONF):
	echo "prefix="$(PREFIX) > $@
	echo "exec_prefix="$(EXEC_PREFIX) >> $@
	echo "libdir="$(LIB_DIR) >> $@
	echo "includedir="$(INCLUDE_DIR) >> $@
	echo >> $@
	echo "Name: "$(RETRO_NAME) >> $@
	echo "Description: "$(RETRO_DESC) >> $@
	echo "Version: "$(RETRO_VERSION) >> $@
	echo "Requires: "$(RETRO_PKG) >> $@
	#echo "Libs: -L$$""{libdir}" >> $@
	echo "Cflags: -I$$""{includedir}" >> $@

$(RETRO_DOC): %: $(RETRO_SRC)
	rm -Rf $@
	valadoc \
		-b $(<D) -o $@ \
		$^ \
		--vapidir=$(VAPI_DIR) $(RETRO_PKG:%=--pkg=%) \
		--package-name=$(RETRO_LIBNAME) --package-version=$(RETRO_VERSION)

$(RETRO_GTK_OUT): %: $(RETRO_GTK_SRC) $(RETRO_OUT) $(RETRO_DEPS)
	mkdir -p $(@D)
	valac \
		-b $(<D) -d $(@D) \
		--library=$(RETRO_GTK_LIBNAME) \
		--vapi=$(RETRO_GTK_PKGNAME).vapi \
		--gir=$(RETRO_GTK_GIRNAME) \
		-H $(@D)/$(RETRO_GTK_HNAME) \
		-o $(RETRO_GTK_SONAME) $(RETRO_GTK_SRC) \
		-X -I./$(OUT_DIR) \
		--vapidir=$(VAPI_DIR) --vapidir=$(OUT_DIR) $(RETRO_GTK_PKG:%=--pkg=%) \
		--save-temps \
		-X -fPIC -X -shared
	@touch $@

$(RETRO_GTK_DEPS):
	mkdir -p $(@D)
	echo $(RETRO_GTK_PKG) | sed -e 's/\s\+/\n/g' > $@

$(OUT_DIR)/$(RETRO_GTK_TYPELIB):
	g-ir-compiler --shared-library $(RETRO_GTK_LIBNAME) --output $@ $(@D)/$(RETRO_GTK_GIRNAME)

$(OUT_DIR)/$(RETRO_GTK_PKGCONF):
	echo "prefix="$(PREFIX) > $@
	echo "exec_prefix="$(EXEC_PREFIX) >> $@
	echo "libdir="$(LIB_DIR) >> $@
	echo "includedir="$(INCLUDE_DIR) >> $@
	echo >> $@
	echo "Name: "$(RETRO_GTK_NAME) >> $@
	echo "Description: "$(RETRO_GTK_DESC) >> $@
	echo "Version: "$(RETRO_GTK_VERSION) >> $@
	echo "Requires: "$(RETRO_GTK_PKG) >> $@
	#echo "Libs: -L$$""{libdir}" >> $@
	echo "Cflags: -I$$""{includedir}" >> $@

clean:
	rm -Rf $(OUT_DIR) $(RETRO_DOC) $(RETRO_DIR)/$(RETRO_LIBNAME)-internal.h

.PHONY: all demo retro retro-gtk install doc clean $(DEMO_CONFIG_FILE)

