APP := mgcs
UNAME_S := $(shell uname -s 2>/dev/null || echo Unknown)
UNAME_M := $(shell uname -m 2>/dev/null || echo unknown)

ifeq ($(UNAME_S):$(UNAME_M),Darwin:arm64)
AUTO_PLATFORM := darwin_arm64
else
AUTO_PLATFORM := linux_x86_64
endif

PLATFORM ?= $(AUTO_PLATFORM)
OUT_DIR := out
SRC := src/$(PLATFORM)/main.S
OBJ := $(OUT_DIR)/main.o
BIN := $(OUT_DIR)/$(APP)
LINUX_CHECK_OBJ := $(OUT_DIR)/linux_x86_64-check.o

.PHONY: all format lint build test clean check-linux-x86_64

all: build

format:
	@printf 'No formatter configured for $(PLATFORM) assembly.\n'

lint: build

check-linux-x86_64: $(OUT_DIR)
	clang --target=x86_64-unknown-linux-gnu -c src/linux_x86_64/main.S -o $(LINUX_CHECK_OBJ)

build: $(BIN)

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

$(OBJ): $(SRC) | $(OUT_DIR)
	clang -c $(SRC) -o $(OBJ)

$(BIN): $(OBJ)
	clang $(OBJ) -o $(BIN)

test: build
	sh tests/smoke.sh

clean:
	rm -rf $(OUT_DIR)
