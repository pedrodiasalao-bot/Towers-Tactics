# Commands
# make           -> builds debug (default)
# make debug     -> debug build
# make release   -> optimized build
# make run       -> builds debug then runs ./app (or .exe)
# make build     -> creates Build/ folder with exe + assets (+ DLLs on MSYS2)
# make clean     -> deletes .o, .d, and app

# ---------------- Build bundle config ----------------
# PATH THAT WILL BE CREATED
BUILD_DIR := Build

# FOLDERS TO INCLUDE IN THE BUILD PATH
ASSET_DIRS := Maps Assets Assets/Sounds Assets/Sprites

# INDIVIDUAL FILES TO INCLUDE IN THE BUILD PATH
ASSET_FILES :=

# ---------------- Platform detection ----------------
UNAME_S := $(shell uname -s)

ifdef MSYSTEM
    IS_MSYS2 := 1
endif

# ---------------- Compiler ----------------
ifeq ($(UNAME_S),Darwin)
    CC := cc
else ifdef IS_MSYS2
    CC := x86_64-w64-mingw32-gcc
else
    CC := gcc
endif

# ---------------- pkg-config (SDL3) ----------------
SDL_PKG  := sdl3 sdl3-image sdl3-ttf

ifeq ($(UNAME_S),Darwin)
    PKGCONF := pkg-config
else ifdef IS_MSYS2
    PKGCONF := x86_64-w64-mingw32-pkg-config
else
    PKGCONF := pkg-config
endif

PKG_CFLAGS := $(shell $(PKGCONF) --cflags $(SDL_PKG) 2>/dev/null)
PKG_LIBS   := $(shell $(PKGCONF) --libs   $(SDL_PKG) 2>/dev/null)

ifeq ($(PKG_LIBS),)
	PKG_LIBS := -lSDL3 -lSDL3_image -lSDL3_ttf
endif

# ---------------- Common flags ----------------
CSTD := -std=c17
WARN := -Wall -Wextra -Wpedantic
DEPS := -MMD -MP
INC  := -Isource -Iutils

DBG := -g -O0 -fno-omit-frame-pointer
REL := -O2 -DNDEBUG

SRC := $(wildcard *.c) $(wildcard **/*.c)
OBJ := $(SRC:.c=.o)
DEP := $(OBJ:.o=.d)

BIN := app

# Add .exe on MSYS2 builds if you want:
ifdef IS_MSYS2
    BIN_EXE := $(BIN).exe
else
    BIN_EXE := $(BIN)
endif

.PHONY: all debug release run clean build build-rel

all: debug

debug: CFLAGS := $(CSTD) $(WARN) $(DBG) $(DEPS) $(INC) $(PKG_CFLAGS)
debug: LDFLAGS := $(PKG_LIBS)
debug: $(BIN_EXE)

release: CFLAGS := $(CSTD) $(WARN) $(REL) $(DEPS) $(INC) $(PKG_CFLAGS)
release: LDFLAGS := $(PKG_LIBS)
release: $(BIN_EXE)

$(BIN_EXE): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: debug
	./$(BIN_EXE)

# --------- Bundle target (debug by default) ----------
build: debug bundle

build-rel: release bundle

bundle:
	@mkdir -p $(BUILD_DIR)
	@cp -f $(BIN_EXE) $(BUILD_DIR)/
	@for d in $(ASSET_DIRS); do \
		if [ -d "$$d" ]; then \
			mkdir -p "$(BUILD_DIR)/$$(dirname "$$d")"; \
			cp -R "$$d" "$(BUILD_DIR)/$$(dirname "$$d")/"; \
		fi; \
	done
	@for f in $(ASSET_FILES); do \
		if [ -f "$$f" ]; then \
			mkdir -p "$(BUILD_DIR)/$$(dirname "$$f")"; \
			cp -f "$$f" "$(BUILD_DIR)/$$f"; \
		fi; \
	done

# Optional: copy SDL runtime DLLs when building on MSYS2/MinGW
# (Your exe needs DLLs next to it unless you ship static or depend on PATH.)
ifdef IS_MSYS2
bundle: copy-dlls

copy-dlls:
	@echo "Copying runtime DLLs into $(BUILD_DIR)/ ..."
	@# If you have ldd available, this is the easiest generic approach:
	@# - list DLL dependencies
	@# - copy anything that lives under your MSYS2/mingw prefix
	@ldd $(BIN_EXE) | awk '/mingw64|clang64|ucrt64/ {print $$3}' | grep -iE '\.dll$$' | while read f; do \
		cp -f "$$f" "$(BUILD_DIR)/"; \
	done
endif

clean:
	rm -f $(OBJ) $(DEP) $(BIN) $(BIN).exe
	rm -rf $(BUILD_DIR)

-include $(DEP)