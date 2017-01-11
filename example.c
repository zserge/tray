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

// static void toggle_cb(struct tray_menu *item) {
//   printf("toggle cb\n");
//   item->checked = !item->checked;
//   tray_update(&tray);
// }

// static void hello_cb(struct tray_menu *item) {
//   (void)item;
//   printf("hello cb\n");
//   if (strcmp(tray.icon, TRAY_ICON1) == 0) {
//     tray.icon = TRAY_ICON2;
//   } else {
//     tray.icon = TRAY_ICON1;
//   }
//   tray_update(&tray);
// }

// static void quit_cb(struct tray_menu *item) {
//   (void)item;
//   printf("quit cb\n");
//   tray_exit();
// }

// static void submenu_cb(struct tray_submenu *item) {
//   (void)item;
//   printf("submenu cb\n");  
//   tray_update(&tray);
// }

//struct tray_submenu *t_sm = (struct tray_submenu[]){{"First",submenu_cb}, {"Second",submenu_cb}, {NULL, NULL},};

// static struct tray tray = {
//     .icon = TRAY_ICON1,
//     .menu = (struct tray_menu[]){{"Hello",    0, 0, NULL, hello_cb,  NULL},
//                                  {"Checked",  0, 1, NULL, toggle_cb, NULL},
//                                  {"Disabled", 1, 0, NULL,      NULL, NULL},
//                                  {"-",        0, 0, NULL,      NULL, NULL},
//                                  {"SubMenu",  0, 0, (struct tray_submenu[]){{"First", NULL,submenu_cb, NULL}, 
//                                                                             {"Second", 
//                                                                               (struct tray_submenu[]){{"Third", NULL,submenu_cb, NULL}, 
//                                                                                                       {NULL, NULL, NULL, NULL},}, 
//                                                                             submenu_cb, NULL}, 
//                                                                             {NULL, NULL, NULL},},      NULL, NULL},
//                                  {"Quit",     0, 0, NULL,   quit_cb, NULL},
//                                  {NULL, 0, 0, NULL, NULL, NULL}},
// };
static struct tray tray = {
    .icon = TRAY_ICON1,
    .menu = (struct tray_menu[]){{"Hello",    NULL},
                                 {"Checked",  NULL},                                                                
                                 {"SubMenu",  (struct tray_menu[]){{"FIRST",    NULL},
                                                                   {"SECOND",  (struct tray_menu[]){{"THIRD",    (struct tray_menu[]){{"7",    NULL},
                                                                                                                {"8",  NULL},
                                                                                                                {NULL,       NULL}}},
                                                                                                    {"FOUR",  (struct tray_menu[]){{"5",    NULL},
                                                                                                                {"6",  NULL},
                                                                                                                {NULL,       NULL}}},
                                                                                                    {NULL,       NULL}}},
                                                                   {NULL,       NULL}}
                                                                   },
                                 {NULL,       NULL}},
};
// static struct tray tray = {
//     .icon = TRAY_ICON1,
//     .menu = (struct tray_menu[]){{"Hello",    NULL},
//                                  {"Checked",  NULL},                                                                
//                                  {"SubMenu",  (struct tray_menu[]){{"FIRST",    NULL},
//                                                                    {"SECOND",   NULL},                                                                   
//                                                                    {NULL,       NULL}}
//                                                                    },
//                                  {NULL,       NULL}},
// };
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
