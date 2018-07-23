ifeq ($(OS),Windows_NT)
	TRAY_CFLAGS := -DTRAY_WINAPI=1
	TRAY_LDFLAGS :=
else ifeq ($(shell uname -s),Linux)
	TRAY_CFLAGS := -DTRAY_APPINDICATOR=1 $(shell pkg-config --cflags appindicator3-0.1)
	TRAY_LDFLAGS := $(shell pkg-config --libs appindicator3-0.1)
else ifeq ($(shell uname -s),Darwin)
	TRAY_CFLAGS := -DTRAY_APPKIT=1
	TRAY_LDFLAGS := -framework Cocoa
endif


CFLAGS := -g -Wall $(TRAY_CFLAGS) -Wall -Wextra -std=c99 -pedantic
LDFLAGS := -g $(TRAY_LDFLAGS)

all: example
example: example.o
	$(CC) $^ $(LDFLAGS) -o $@
	
example.o: example.c tray.h

clean:
	rm -f example.o example example.exe
