#include <stdio.h>
#include <string.h>

#include "tray.h"

#if TRAY_APPINDICATOR
#define TRAY_ICON1 "indicator-messages"
#define TRAY_ICON2 "indicator-messages-new"
#elif TRAY_APPKIT
#define TRAY_ICON1 "icon.png"
#define TRAY_ICON2 "icon.png"
#elif TRAY_WINAPI
#define TRAY_ICON1 "icon.ico"
#define TRAY_ICON2 "icon.ico"
#endif

static struct tray tray;

static void toggle_cb(struct tray_menu *item) {
  printf("toggle cb\n");
  item->checked = !item->checked;
  tray_update(&tray);
}

static void hello_cb(struct tray_menu *item) {
  (void)item;
  printf("hello cb\n");
  if (strcmp(tray.icon, TRAY_ICON1) == 0) {
    tray.icon = TRAY_ICON2;
  } else {
    tray.icon = TRAY_ICON1;
  }
  tray_update(&tray);
}

static void quit_cb(struct tray_menu *item) {
  (void)item;
  printf("quit cb\n");
  tray_exit();
}

static struct tray tray = {
    .icon = TRAY_ICON1,
    .menu = (struct tray_menu[]){{"Hello", 0, 0, hello_cb, NULL},
                                 {"Checked", 0, 1, toggle_cb, NULL},
                                 {"Disabled", 1, 0, NULL, NULL},
                                 {"-", 0, 0, NULL, NULL},
                                 {"+", 0, 0, NULL, NULL},
                                 {"Quit", 0, 0, quit_cb, NULL},
                                 {NULL, 0, 0, NULL, NULL}},
};

int main() {
  if (tray_init(&tray) < 0) {
    printf("failed to create tray\n");
    return 1;
  }
  while (tray_loop(1) == 0) {
    printf("iteration\n");
  }
  return 0;
}
