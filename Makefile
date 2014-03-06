NULL=

SRC_DIR = retro
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
	Runnable.vala \
	Runner.vala \
	AudioSamples.vala \
	retro-core-extern.c \
	retro-environment-extern.c \
	$(NULL)

DEMO_FILES= \
	Demo.vala \
	Engine.vala \
	Window.vala \
	OptionsDialog.vala \
	AudioDevice.vala \
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

RETRO_SRC = $(RETRO_FILES:%=$(SRC_DIR)/%)
DEMO_SRC = $(DEMO_FILES:%=$(DEMO_DIR)/%)

all: $(DEMO)

$(DEMO): $(RETRO_SRC) $(DEMO_SRC)
	mkdir -p $(OUT_DIR)
	cp -f $(SRC_DIR)/libretro.h $(OUT_DIR)/libretro.h
	valac -b $(SRC_DIR) -d $(@D) \
		-o $(@F) $^ \
		--vapidir=$(VAPI_DIR) $(PKG:%=--pkg=%) \
		--Xcc="-g"
		

clean:
	rm -Rf $(OUT_DIR)

.PHONY: all test clean

