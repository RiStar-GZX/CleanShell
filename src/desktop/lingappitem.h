#pragma once

#include <gtk/gtk.h>
#include <lingshell.h>
#include <lingdesktop.h>

G_BEGIN_DECLS

#define LING_TYPE_APP_ITEM (ling_app_item_get_type())
G_DECLARE_FINAL_TYPE(LingAppItem,ling_app_item,LING,APP_ITEM,GtkBox)


GtkWidget * ling_app_item_new();

GtkWidget * ling_app_item_copy(GtkWidget * source);

gboolean ling_app_item_set(LingAppItem * self,app_info * app,guint icon_size,gboolean label);

void ling_app_item_set_label_visible(LingAppItem * self,gboolean visible);

app_info * ling_app_item_get_app_info(LingAppItem * self);

// void ling_app_item_drag_start(LingFixedViewItem * item,gpointer user_data);

// void ling_app_item_drag_end(LingFixedViewItem * item,gpointer item_data,gpointer fv_data);

G_END_DECLS
