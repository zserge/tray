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
  for (struct tray_menu *m = tray->menu; m != NULL && m->text != NULL; m++) {
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

#import <Cocoa/Cocoa.h>

static NSAutoreleasePool *pool;
static NSStatusBar *statusBar;
static id statusItem;
static id statusBarButton;

@interface Tray : NSObject<NSApplicationDelegate>
- (void) menuCallback: (id) sender;
@end
@implementation Tray
- (void) menuCallback: (id) sender {
  struct tray_menu *m = (struct tray_menu *)
   [[sender representedObject] pointerValue];
  m->cb(m);
}
@end

static int tray_init(struct tray *tray) {
  pool = [NSAutoreleasePool new];
  [NSApplication sharedApplication];

  Tray *trayDelegate = [Tray new];
  [NSApp setDelegate: trayDelegate];

  statusBar = [NSStatusBar systemStatusBar];
  statusItem = [statusBar statusItemWithLength:NSVariableStatusItemLength];
  [statusItem retain];
  [statusItem setHighlightMode:YES];
  statusBarButton = [statusItem button];

  tray_update(tray);
  [NSApp activateIgnoringOtherApps:YES];
  return -1;
}

static int tray_loop(int blocking) {
  NSEvent *event;
  NSDate *until = (blocking ? [NSDate distantFuture] : [NSDate distantPast]);
  event = [NSApp nextEventMatchingMask:NSAnyEventMask
    untilDate:until
    inMode:NSDefaultRunLoopMode
    dequeue:YES];
  if (event) {
    [NSApp sendEvent:event];
  }
  return 0;
}

static void tray_update(struct tray *tray) {
  [statusBarButton setImage:[NSImage imageNamed:@"icon.png"]];

  NSMenu *menu = [NSMenu new];
  [menu autorelease];
  [menu setAutoenablesItems:NO];
  for (struct tray_menu *m = tray->menu; m != NULL && m->text != NULL; m++) {
    NSMenuItem *menuItem = [NSMenuItem alloc];
    [menuItem autorelease];
    [menuItem
      initWithTitle: [NSString stringWithUTF8String: m->text]
      action:@selector(menuCallback:)
      keyEquivalent:@""];
    [menuItem setEnabled:YES];
    [menuItem setRepresentedObject: [NSValue valueWithPointer:m]];

    [menu addItem:menuItem];

    //[menu addItem:[NSMenuItem separatorItem]];
  }

  [statusItem setMenu:menu];
}

static void tray_exit() {
  [NSApp terminate:NSApp];
}

#elif defined(TRAY_WINAPI)
#else
#endif

#endif /* TRAY_H */
