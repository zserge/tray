#ifndef TRAY_H
#define TRAY_H

struct tray_menu;

struct tray {
  char *icon;
  struct tray_menu *menu;
};

struct tray_menu {
  char *text;
  int disabled;
  int checked;
  struct tray_menu *submenu;

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
  (void)item;
  struct tray_menu *m = (struct tray_menu *)data;
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

//recursive proc
static void  submenu_update(struct tray_menu *m, 
                      GtkWidget *_item, GtkMenuShell *_submenu) {
  GtkMenuShell *submenu; 
  for (struct tray_menu *s_m = m->submenu; s_m!=NULL && s_m->text!=NULL; s_m++) {
    GtkWidget *item;
    if (s_m->submenu != NULL) {      
       item = gtk_menu_item_new_with_label(s_m->text);
       submenu = (GtkMenuShell *)gtk_menu_new();
       gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), (GtkWidget *)submenu);
       submenu_update(s_m, item, submenu);
       gtk_widget_show(item);
       gtk_menu_shell_append(GTK_MENU_SHELL(_submenu), item);

    }else if (strcmp(s_m->text, "-") == 0) {
      gtk_menu_item_set_submenu(GTK_MENU_ITEM(_item), (GtkWidget *)_submenu);
      item = gtk_separator_menu_item_new();
      gtk_widget_show(item);
      gtk_menu_shell_append(GTK_MENU_SHELL(_submenu), item);
    }
    else{  
      gtk_menu_item_set_submenu(GTK_MENU_ITEM(_item), (GtkWidget *)_submenu);
      item = gtk_check_menu_item_new_with_label(s_m->text);
      gtk_widget_set_sensitive(item, !s_m->disabled);
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item), !!s_m->checked);
      gtk_widget_show(item);
      gtk_menu_shell_append(GTK_MENU_SHELL(_submenu), item);
      if (s_m->cb != NULL) {
        g_signal_connect(item, "activate", G_CALLBACK(_tray_menu_cb), s_m);
      }
    }
  }  
}

static void tray_update(struct tray *tray) {
  GtkMenuShell *menu = (GtkMenuShell *)gtk_menu_new();
  for (struct tray_menu *m = tray->menu; m != NULL && m->text != NULL; m++) {
    GtkWidget *item;
    if (m->submenu != NULL) {      
      GtkMenuShell *submenu = (GtkMenuShell *)gtk_menu_new();
      item = gtk_menu_item_new_with_label(m->text);
      gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), (GtkWidget *)submenu);
      submenu_update(m, item, submenu);
    } else if (strcmp(m->text, "-") == 0) {
      item = gtk_separator_menu_item_new();
    } else {      
      item = gtk_check_menu_item_new_with_label(m->text);
      gtk_widget_set_sensitive(item, !m->disabled);
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item), !!m->checked);      
    }

    gtk_widget_show(item);    
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
    if (m->cb != NULL) {
      g_signal_connect(item, "activate", G_CALLBACK(_tray_menu_cb), m);
    }
  }
  app_indicator_set_icon(indicator, tray->icon);
  // GTK is all about reference counting, so previous menu should be destroyed
  // here
  app_indicator_set_menu(indicator, GTK_MENU(menu));
}


static void tray_exit() { loop_result = -1; }

#elif defined(TRAY_APPKIT)

#import <Cocoa/Cocoa.h>

static NSAutoreleasePool *pool;
static NSStatusBar *statusBar;
static id statusItem;
static id statusBarButton;

@interface Tray : NSObject <NSApplicationDelegate>
- (void)menuCallback:(id)sender;
@end
@implementation Tray
- (void)menuCallback:(id)sender {
  struct tray_menu *m =
      (struct tray_menu *)[[sender representedObject] pointerValue];
  m->cb(m);
}
@end

static int tray_init(struct tray *tray) {
  pool = [NSAutoreleasePool new];
  [NSApplication sharedApplication];

  Tray *trayDelegate = [Tray new];
  [NSApp setDelegate:trayDelegate];

  statusBar = [NSStatusBar systemStatusBar];
  statusItem = [statusBar statusItemWithLength:NSVariableStatusItemLength];
  [statusItem retain];
  [statusItem setHighlightMode:YES];
  statusBarButton = [statusItem button];

  tray_update(tray);
  [NSApp activateIgnoringOtherApps:YES];
  return 0;
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
  [statusBarButton
      setImage:[NSImage imageNamed:[NSString stringWithUTF8String:tray->icon]]];

  NSMenu *menu = [NSMenu new];
  [menu autorelease];
  [menu setAutoenablesItems:NO];
  for (struct tray_menu *m = tray->menu; m != NULL && m->text != NULL; m++) {
    if (strcmp(m->text, "-") == 0) {
      [menu addItem:[NSMenuItem separatorItem]];
    } else {
      NSMenuItem *menuItem = [NSMenuItem alloc];
      [menuItem autorelease];
      [menuItem initWithTitle:[NSString stringWithUTF8String:m->text]
                       action:@selector(menuCallback:)
                keyEquivalent:@""];
      [menuItem setEnabled:(m->disabled ? NO : YES)];
      [menuItem setState:(m->checked ? NSOnState : NSOffState)];
      [menuItem setRepresentedObject:[NSValue valueWithPointer:m]];

      [menu addItem:menuItem];
    }
  }

  [statusItem setMenu:menu];
}

static void tray_exit() { [NSApp terminate:NSApp]; }

#elif defined(TRAY_WINAPI)
#include <windows.h>

#include <shellapi.h>

#define WM_TRAY_CALLBACK_MESSAGE (WM_USER + 1)
#define WC_TRAY_CLASS_NAME "TRAY"
#define ID_TRAY_FIRST 1000

static WNDCLASSEX wc;
static NOTIFYICONDATA nid;
static HWND hwnd;
static HMENU hmenu = NULL;

static LRESULT CALLBACK _tray_wnd_proc(HWND hwnd, UINT msg, WPARAM wparam,
                                       LPARAM lparam) {
  switch (msg) {
  case WM_CLOSE:
    DestroyWindow(hwnd);
    return 0;
  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;
  case WM_TRAY_CALLBACK_MESSAGE:
    if (lparam == WM_LBUTTONUP || lparam == WM_RBUTTONUP) {
      POINT p;
      GetCursorPos(&p);
      WORD cmd = TrackPopupMenu(hmenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON |
                                           TPM_RETURNCMD | TPM_NONOTIFY,
                                p.x, p.y, 0, hwnd, NULL);
      SendMessage(hwnd, WM_COMMAND, cmd, 0);
      return 0;
    }
    break;
  case WM_COMMAND:
    if (wparam >= ID_TRAY_FIRST) {
      MENUITEMINFO item = {
          .cbSize = sizeof(MENUITEMINFO), .fMask = MIIM_ID | MIIM_DATA,
      };
      if (GetMenuItemInfo(hmenu, wparam, FALSE, &item)) {
        struct tray_menu *menu = (struct tray_menu *)item.dwItemData;
        menu->cb(menu);
      }
      return 0;
    }
    break;
  }
  return DefWindowProc(hwnd, msg, wparam, lparam);
}

static int tray_init(struct tray *tray) {
  memset(&wc, 0, sizeof(wc));
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.lpfnWndProc = _tray_wnd_proc;
  wc.hInstance = GetModuleHandle(NULL);
  wc.lpszClassName = WC_TRAY_CLASS_NAME;
  if (!RegisterClassEx(&wc)) {
    return -1;
  }

  hwnd = CreateWindowEx(0, WC_TRAY_CLASS_NAME, NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  if (hwnd == NULL) {
    return -1;
  }
  UpdateWindow(hwnd);

  memset(&nid, 0, sizeof(nid));
  nid.cbSize = sizeof(NOTIFYICONDATA);
  nid.hWnd = hwnd;
  nid.uID = 0;
  nid.uFlags = NIF_ICON | NIF_MESSAGE;
  nid.uCallbackMessage = WM_TRAY_CALLBACK_MESSAGE;
  Shell_NotifyIcon(NIM_ADD, &nid);

  tray_update(tray);
  return 0;
}

static int tray_loop(int blocking) {
  MSG msg;
  if (blocking) {
    GetMessage(&msg, NULL, 0, 0);
  } else {
    PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
  }
  if (msg.message == WM_QUIT) {
    return -1;
  }
  TranslateMessage(&msg);
  DispatchMessage(&msg);
  return 0;
}

static void tray_update(struct tray *tray) {
  HMENU prevmenu = hmenu;
  int i = 0;
  hmenu = CreatePopupMenu();
  for (struct tray_menu *m = tray->menu; m != NULL && m->text != NULL;
       m++, i++) {
    if (strcmp(m->text, "-") == 0) {
      InsertMenu(hmenu, i, MF_SEPARATOR, TRUE, "");
    } else {
      MENUITEMINFO item;
      memset(&item, 0, sizeof(item));
      item.cbSize = sizeof(MENUITEMINFO);
      item.fMask = MIIM_ID | MIIM_TYPE | MIIM_STATE | MIIM_DATA;
      item.fType = 0;
      item.fState = 0;
      if (m->disabled) {
        item.fState |= MFS_DISABLED;
      }
      if (m->checked) {
        item.fState |= MFS_CHECKED;
      }
      item.wID = i + ID_TRAY_FIRST;
      item.dwTypeData = m->text;
      item.dwItemData = (ULONG_PTR)m;

      InsertMenuItem(hmenu, i, TRUE, &item);
    }
  }
  SendMessage(hwnd, WM_INITMENUPOPUP, (WPARAM)hmenu, 0);
  HICON icon;
  ExtractIconEx(tray->icon, 0, NULL, &icon, 1);
  if (nid.hIcon) {
    DestroyIcon(nid.hIcon);
  }
  nid.hIcon = icon;
  Shell_NotifyIcon(NIM_MODIFY, &nid);

  if (prevmenu != NULL) {
    DestroyMenu(prevmenu);
  }
}

static void tray_exit() {
  Shell_NotifyIcon(NIM_DELETE, &nid);
  if (nid.hIcon != 0) {
    DestroyIcon(nid.hIcon);
  }
  if (hmenu != 0) {
    DestroyMenu(hmenu);
  }
  PostQuitMessage(0);
  UnregisterClass(WC_TRAY_CLASS_NAME, GetModuleHandle(NULL));
}
#else
static int tray_init(struct tray *tray) { return -1; }
static int tray_loop(int blocking) { return -1; }
static void tray_update(struct tray *tray) {}
static void tray_exit();
#endif

#endif /* TRAY_H */
