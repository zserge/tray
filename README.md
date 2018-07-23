Tray
----

Cross-platform, single header, super tiny C99 implementation of a system tray icon with a popup menu.

Works well on:

* Linux/Gtk (libappindicator)
* Windows XP or newer (shellapi.h)
* MacOS (Cocoa/AppKit)

There is also a stub implementation that returns errors on attempt to create a tray menu.

# Setup

Before you can compile `tray`, you'll need to add an environment definition before the line where you include `tray.h`. 

**For Windows:**
```c
#include <stdio.h>
#include <string.h>

#define TRAY_WINAPI 1

#include "tray.h"
...
```

**For Linux:**
```c
#include <stdio.h>
#include <string.h>

#define TRAY_APPINDICATOR 1

#include "tray.h"
...
```

**For Mac:**
```c
#include <stdio.h>
#include <string.h>

#define TRAY_APPKIT 1

#include "tray.h"
...
```

# Demo

The included example `.c` files can be compiled based on your environment.

For example, to compile and run the program on Windows: 

```shell
$> gcc example_windows.c [Enter]
``` 

This will compile and build `a.out`. To run it: 

```
$> a [Enter]
```

# Example

```c
struct tray tray = {
    .icon = "icon.png",
    .menu = (struct tray_menu[]){{"Toggle me", 0, 0, toggle_cb, NULL},
                                 {"-", 0, 0, NULL, NULL},
                                 {"Quit", 0, 0, quit_cb, NULL},
                                 {NULL, 0, 0, NULL, NULL}},
};

void toggle_cb(struct tray_menu *item) {
	item->checked = !item->checked;
	tray_update(&tray);
}

void quit_cb(struct tray_menu *item) {
  tray_exit();
}

...

tray_init(&tray);
while (tray_loop(1) == 0);
tray_exit();

```

# API

Tray structure defines an icon and a menu.
Menu is a NULL-terminated array of items.
Menu item defines menu text, menu checked and disabled (grayed) flags and a
callback with some optional context pointer.

```c
struct tray {
  char *icon;
  struct tray_menu *menu;
};

struct tray_menu {
  char *text;
  int disabled;
  int checked;

  void (*cb)(struct tray_menu *);
  void *context;

  struct tray_menu *submenu;
};
```

* `int tray_init(struct tray *)` - creates tray icon. Returns -1 if tray icon/menu can't be created.
* `void tray_update(struct tray *)` - updates tray icon and menu.
* `int tray_loop(int blocking)` - runs one iteration of the UI loop. Returns -1 if `tray_exit()` has been called.
* `void tray_exit()` - terminates UI loop.

All functions are meant to be called from the UI thread only.

Menu arrays must be terminated with a NULL item, e.g. the last item in the
array must have text field set to NULL.

## Roadmap

* [x] Cross-platform tray icon
* [x] Cross-platform tray popup menu
* [x] Separators in the menu
* [x] Disabled/enabled menu items
* [x] Checked/unchecked menu items
* [x] Nested menus
* [ ] Icons for menu items
* [x] Rewrite ObjC code in C using ObjC Runtime (now ObjC code breaks many linters and static analyzers)
* [ ] Call GTK code using dlopen/dlsym (to make binaries run safely if Gtk libraries are not available)

## License

This software is distributed under [MIT license](http://www.opensource.org/licenses/mit-license.php),
 so feel free to integrate it in your commercial products.

