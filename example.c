#include <stdio.h>
#include <string.h>

#include "tray.h"

static struct tray tray;

static void toggle_cb(struct tray_menu *item) {
  printf("toggle cb\n");
  item->checked = !item->checked;
  tray_update(&tray);
}

static void hello_cb(struct tray_menu *item) {
  printf("hello cb\n");
  if (strcmp(tray.icon, "indicator-messages") == 0) {
    tray.icon = "indicator-messages-new";
  } else {
    tray.icon = "indicator-messages";
  }
  tray_update(&tray);
}

static void quit_cb(struct tray_menu *item) {
  printf("quit cb\n");
  tray_exit();
}

static struct tray tray = {
    .menu = (struct tray_menu[]){{"Hello", 0, 0, hello_cb, NULL},
                                 {"Checked", 0, 1, toggle_cb, NULL},
                                 {"Disabled", 1, 0, NULL, NULL},
                                 {"-", 0, 0, NULL, NULL},
                                 {"Quit", 0, 0, quit_cb, NULL},
                                 {NULL, 0, 0, NULL, NULL}},
};

int main(int argc, char *argv[]) {
#if TRAY_APPINDICATOR
  tray.icon = "indicator-messages-new";
#elif TRAY_COCOA
  tray.icon = "icon.png";
#elif TRAY_WINAPI
  tray.icon = "icon.ico";
#endif
  if (tray_init(&tray) < 0) {
    printf("failed to create tray\n");
    return 1;
  }
  while (tray_loop(1) == 0) {
    printf("iteration\n");
  }
  return 0;
}
