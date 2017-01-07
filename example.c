#include <string.h>

#include "tray.h"

static struct tray tray;

static void hello_cb(struct tray_menu *item) {
	if (strcmp(tray.icon, "indicator-messages") == 0) {
		tray.icon = "indicator-messages-new";
	} else {
		tray.icon = "indicator-messages";
	}
	tray_update(&tray);
}

static void quit_cb(struct tray_menu *item) {
	tray_exit();
}

static struct tray tray = {
	.icon = "indicator-messages-new",
	.menu = (struct tray_menu[]){
		{NULL, "Hello", 0, hello_cb, NULL},
		{NULL, "Quit", 0, quit_cb, NULL},
		{NULL, NULL, 0, NULL, NULL}
	},
};

int main(int argc, char *argv[]) {
	tray_init(&tray);
	while (tray_loop(1) == 0);
	return 0;
}
