#include <stdio.h>
#include "tray.h"

extern void toggle_cb(struct tray_menu *item); 
extern void quit_cb(struct tray_menu *item);

struct tray main_tray = {
    .icon = "icon.ico",
    .menu = (struct tray_menu[]){{"Toggle me", 0, 0, toggle_cb, NULL},
                                 {"-", 0, 0, NULL, NULL},
                                 {"Quit", 0, 0, quit_cb, NULL},
                                 {NULL, 0, 0, NULL, NULL}},
};

void toggle_cb(struct tray_menu *item) {
	item->checked = !item->checked;
	tray_update(&main_tray);
}

void quit_cb(struct tray_menu *item) {
  tray_exit();
}


int main() {

    

    tray_init(&main_tray);
    while (tray_loop(1) == 0);
    tray_exit();


    return 0;
}