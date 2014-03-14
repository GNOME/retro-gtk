NULL=

RETRO_DIR = retro
FLICKY_DIR = flicky
DEMO_DIR = demo

OUT_DIR = out
VAPI_DIR = vapi

RETRO_LIBNAME=retro
FLICKY_LIBNAME=flicky

RETRO_VERSION=1.0
FLICKY_VERSION=1.0

RETRO_PKGNAME=$(RETRO_LIBNAME)-$(RETRO_VERSION)
FLICKY_PKGNAME=$(FLICKY_LIBNAME)-$(FLICKY_VERSION)

RETRO_DOC=$(RETRO_PKGNAME)-doc

DEMO = $(OUT_DIR)/demo

RETRO_FILES= \
	Audio.vala \
	Camera.vala \
	Core.vala \
	Device.vala \
	Disk.vala \
	Environment.vala \
	FrameTime.vala \
	GameInfo.vala \
	GameType.vala \
	Hardware.vala \
	Keyboard.vala \
	Location.vala \
	Log.vala \
	Memory.vala \
	Message.vala \
	Performance.vala \
	PixelFormat.vala \
	Region.vala \
	Rotation.vala \
	Rumble.vala \
	Sensor.vala \
	SystemAvInfo.vala \
	SystemInfo.vala \
	Variable.vala \
	retro-core-extern.c \
	retro-core-global.c \
	retro-core-interfaces.c \
	retro-environment-extern.c \
	$(NULL)

FLICKY_FILES= \
	AudioSamples.vala \
	ControllerDevice.vala \
	FileStreamLogger.vala \
	KeyboardBox.vala \
	Options.vala \
	Runnable.vala \
	Runner.vala \
	$(NULL)

DEMO_FILES= \
	Demo.vala \
	Engine.vala \
	Window.vala \
	OptionsDialog.vala \
	AudioDevice.vala \
	lol.c \
	$(NULL)

RETRO_PKG= \
	gmodule-2.0 \
	stdint \
	$(NULL)

FLICKY_PKG= \
	gtk+-3.0 \
	$(RETRO_PKGNAME) \
	$(NULL)

PKG= \
	$(RETRO_PKGNAME) \
	$(FLICKY_PKGNAME) \
	libpulse \
	libpulse-mainloop-glib \
	$(NULL)

RETRO_SRC = $(RETRO_FILES:%=$(RETRO_DIR)/%)
FLICKY_SRC = $(FLICKY_FILES:%=$(FLICKY_DIR)/%)
DEMO_SRC = $(DEMO_FILES:%=$(DEMO_DIR)/%)

RETRO_OUT= \
	$(OUT_DIR)/lib$(RETRO_LIBNAME).so \
	$(OUT_DIR)/$(RETRO_PKGNAME).vapi \
	$(OUT_DIR)/$(RETRO_PKGNAME).gir \
	$(OUT_DIR)/$(RETRO_LIBNAME).h \
	$(NULL)

FLICKY_OUT= \
	$(OUT_DIR)/lib$(FLICKY_LIBNAME).so \
	$(OUT_DIR)/$(FLICKY_PKGNAME).vapi \
	$(OUT_DIR)/$(FLICKY_PKGNAME).gir \
	$(OUT_DIR)/$(FLICKY_LIBNAME).h \
	$(NULL)

RETRO_DEPS=$(OUT_DIR)/$(RETRO_PKGNAME).deps
FLICKY_DEPS=$(OUT_DIR)/$(FLICKY_PKGNAME).deps

all: demo retro flicky

demo: $(DEMO)
retro: $(RETRO_OUT) $(RETRO_DEPS)
flicky: $(FLICKY_OUT) $(FLICKY_DEPS)
doc: $(RETRO_DOC)

$(DEMO): $(RETRO_SRC) $(FLICKY_SRC) $(DEMO_SRC) $(RETRO_OUT) $(RETRO_DEPS) $(FLICKY_OUT) $(FLICKY_DEPS)
	mkdir -p $(OUT_DIR)
	valac -b $(RETRO_DIR) -d $(@D) \
		-o $(@F) $(DEMO_SRC) \
		-X -I./$(OUT_DIR) -X $(OUT_DIR)/libflicky.so -X $(OUT_DIR)/libretro.so \
		--vapidir=$(VAPI_DIR) --vapidir=$(OUT_DIR) $(PKG:%=--pkg=%) \
		--save-temps \
		-g

$(RETRO_OUT): %: $(RETRO_SRC)
	mkdir -p $(@D)
	valac \
		-b $(<D) -d $(@D) \
		--library=$(RETRO_LIBNAME) \
		--vapi=$(RETRO_PKGNAME).vapi \
		--gir=$(RETRO_PKGNAME).gir \
		-H $(@D)/$(RETRO_LIBNAME).h \
		-h $(<D)/$(RETRO_LIBNAME)-internal.h \
		-o lib$(RETRO_LIBNAME).so $^ \
		--vapidir=$(VAPI_DIR) $(RETRO_PKG:%=--pkg=%) \
		--save-temps \
		-X -fPIC -X -shared

$(RETRO_DEPS):
	mkdir -p $(@D)
	echo $(RETRO_PKG) | sed -e 's/\s\+/\n/g' > $@

$(RETRO_DOC): %: $(RETRO_SRC)
	rm -Rf $@
	valadoc \
		-b $(<D) -o $@ \
		$^ \
		--vapidir=$(VAPI_DIR) $(RETRO_PKG:%=--pkg=%) \
		--package-name=$(RETRO_LIBNAME) --package-version=$(RETRO_VERSION)

$(FLICKY_OUT): %: $(FLICKY_SRC) $(RETRO_OUT) $(RETRO_DEPS)
	mkdir -p $(@D)
	valac \
		-b $(<D) -d $(@D) \
		--library=$(FLICKY_LIBNAME) \
		--vapi=$(FLICKY_PKGNAME).vapi \
		--gir=$(FLICKY_PKGNAME).gir \
		-H $(@D)/$(FLICKY_LIBNAME).h \
		-o lib$(FLICKY_LIBNAME).so $(FLICKY_SRC) \
		-X -I./$(OUT_DIR) \
		--vapidir=$(VAPI_DIR) --vapidir=$(OUT_DIR) $(FLICKY_PKG:%=--pkg=%) \
		--save-temps \
		-X -fPIC -X -shared

$(FLICKY_DEPS):
	mkdir -p $(@D)
	echo $(FLICKY_PKG) | sed -e 's/\s\+/\n/g' > $@

clean:
	rm -Rf $(OUT_DIR) $(RETRO_DOC) $(RETRO_DIR)/$(RETRO_LIBNAME)-internal.h

.PHONY: all demo retro flicky doc clean

