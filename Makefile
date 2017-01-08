ifeq ($(OS),Windows_NT)
	TRAY_CFLAGS := -DTRAY_WINAPI=1
	TRAY_LDFLAGS :=
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
	TRAY_CFLAGS := -DTRAY_APPINDICATOR=1 $(shell pkg-config --cflags appindicator3-0.1)
	TRAY_LDFLAGS := $(shell pkg-config --libs appindicator3-0.1)
endif
ifeq ($(UNAME_S),Darwin)
	TRAY_CFLAGS := -DTRAY_APPKIT=1 -x objective-c
	TRAY_LDFLAGS := -framework Cocoa
endif
endif


CFLAGS := -g -Wall $(TRAY_CFLAGS)
LDFLAGS := -g $(TRAY_LDFLAGS)

all: example
example: example.o
	$(CC) $^ $(LDFLAGS) -o $@
	
example.o: example.c tray.h

clean:
	rm -f example.o example example.exe
