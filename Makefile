NULL=

RETRO_DIR = retro
FLICKY_DIR = flicky
DEMO_DIR = demo

OUT_DIR = out
VAPI_DIR = vapi

DEMO = $(OUT_DIR)/demo

RETRO_FILES= \
	Core.vala \
	Environment.vala \
	Log.vala \
	Variable.vala \
	SystemInfo.vala \
	SystemAvInfo.vala \
	GameInfo.vala \
	Device.vala \
	AudioSamples.vala \
	retro-core-extern.c \
	retro-environment-extern.c \
	$(NULL)

FLICKY_FILES= \
	Runnable.vala \
	Runner.vala \
	$(NULL)

DEMO_FILES= \
	Demo.vala \
	Engine.vala \
	Window.vala \
	OptionsDialog.vala \
	AudioDevice.vala \
	ControllerDevice.vala \
	KeyboardHandler.vala \
	lol.c \
	$(NULL)

PKG= \
	gmodule-2.0 \
	gtk+-3.0 \
	libpulse \
	libpulse-mainloop-glib \
	Retro \
	$(NULL)

RETRO_SRC = $(RETRO_FILES:%=$(RETRO_DIR)/%)
FLICKY_SRC = $(FLICKY_FILES:%=$(FLICKY_DIR)/%)
DEMO_SRC = $(DEMO_FILES:%=$(DEMO_DIR)/%)

all: $(DEMO)

$(DEMO): $(RETRO_SRC) $(FLICKY_SRC) $(DEMO_SRC)
	mkdir -p $(OUT_DIR)
	cp -f $(RETRO_DIR)/libretro.h $(OUT_DIR)/libretro.h
	valac -b $(RETRO_DIR) -d $(@D) \
		-o $(@F) $^ \
		--vapidir=$(VAPI_DIR) $(PKG:%=--pkg=%) \
		--Xcc="-g"
		

clean:
	rm -Rf $(OUT_DIR)

.PHONY: all test clean

