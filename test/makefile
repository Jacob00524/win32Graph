# Default architecture (can be overridden: `make ARCH=32`)
ARCH ?= 64

BUILD-FOLDER = build$(ARCH)
TARGET = program$(ARCH).exe

SRC := $(wildcard src/*.c)
OBJ := $(patsubst src/%.c, $(BUILD-FOLDER)/%.o, $(SRC))

CC-32 = i686-w64-mingw32-gcc
CC-64 = x86_64-w64-mingw32-gcc

WININC-32 = /usr/i686-w64-mingw32/include
WININC-64 = /usr/x86_64-w64-mingw32/include

LIBS=-lwingraph

OTHER_INCLUDE = -Iinclude

CC = $(CC-$(ARCH))
WININC = $(WININC-$(ARCH))

all: $(OBJ) $(TARGET)

$(BUILD-FOLDER)/%.o: src/%.c
	@mkdir -p $(BUILD-FOLDER)
	$(CC) -c $< -o $@ -I$(WININC) $(OTHER_INCLUDE)

$(TARGET): $(OBJ)
	$(CC) $^ -o $@ -municode -mwindows $(LIBS)

clean:
	rm -rf $(BUILD-FOLDER)
	rm -f program$(ARCH).exe

clean-all:
	make clean ARCH=64
	make clean ARCH=32