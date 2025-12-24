#pragma once

#include <gtk/gtk.h>
#include <lingshell.h>
#include <lingappviewpage.h>
#include <lingappitem.h>

G_BEGIN_DECLS

#define LING_TYPE_APP_DRAWER (ling_app_drawer_get_type())
G_DECLARE_FINAL_TYPE(LingAppDrawer,ling_app_drawer,LING,APP_DRAWER,GtkBox)

GtkWidget * ling_app_drawer_new();

gboolean ling_app_drawer_add_item(LingAppDrawer *self,app_info * info,
                                  int column,int row,int width,int height);


G_END_DECLS
