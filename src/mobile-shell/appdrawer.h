#pragma once

#include <gtk/gtk.h>
#include <mobileshell.h>
#include <appviewpage.h>
#include <desktopitem.h>

G_BEGIN_DECLS

#define CLM_DESKTOP_DRAWER_OP_NAME "drawer_switch"

#define CLM_TYPE_APP_DRAWER (clm_app_drawer_get_type())
G_DECLARE_FINAL_TYPE(ClmAppDrawer,clm_app_drawer,LING,APP_DRAWER,GtkBox)

GtkWidget * clm_app_drawer_new();

gboolean clm_app_drawer_add_item(ClmAppDrawer *self,app_info * info,
                                  int column,int row,int width,int height);


G_END_DECLS
