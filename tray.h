#ifndef TRAY_H
#define TRAY_H

struct tray_menu;

struct tray {
	char *icon;
	char *tooltip;
	struct tray_menu *menu;
};

struct tray_menu {
	char *icon;
	char *text; /* label */
	int flags;

	void (*cb)(struct tray_menu *);
	void *context;
};

static void tray_update(struct tray *tray);

#if defined(TRAY_APPINDICATOR)

#include <gtk/gtk.h>
#include <libappindicator/app-indicator.h>

#define TRAY_APPINDICATOR_ID "tray-id"

static AppIndicator *indicator = NULL;
static int loop_result = 0;

static void _tray_menu_cb(GtkMenuItem *item, gpointer data) {
	struct tray_menu *m = (struct tray_menu *) data;
	m->cb(m);
}

static int tray_init(struct tray *tray) {
	if (gtk_init_check(0, NULL) == FALSE) {
		return -1;
	}
	indicator = app_indicator_new(TRAY_APPINDICATOR_ID, tray->icon,
			APP_INDICATOR_CATEGORY_APPLICATION_STATUS);
	app_indicator_set_status(indicator, APP_INDICATOR_STATUS_ACTIVE);
	tray_update(tray);
	return 0;
}

static int tray_loop(int blocking) {
	gtk_main_iteration_do(blocking);
	return loop_result;
}

static void tray_update(struct tray *tray) {
  struct tray_menu *m;

	app_indicator_set_icon(indicator, tray->icon);
  GtkMenuShell *gtk_menu = (GtkMenuShell *)gtk_menu_new();
  for (m = tray->menu;; m++) {
    if (m->text == NULL) {
      break;
    }
    GtkWidget *item = gtk_menu_item_new_with_label(m->text);
    gtk_widget_show(item);
    gtk_menu_shell_append(GTK_MENU_SHELL(gtk_menu), item);
		if (m->cb != NULL) {
			g_signal_connect(item, "activate", G_CALLBACK(_tray_menu_cb), m);
		}
  }
  app_indicator_set_menu(indicator, GTK_MENU(gtk_menu));
}

static void tray_exit() {
	loop_result = -1;
}

#elif defined(TRAY_APPKIT)
#elif defined(TRAY_WINAPI)
#else
#endif

#endif /* TRAY_H */
