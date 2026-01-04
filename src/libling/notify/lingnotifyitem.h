#pragma once

#include <gtk/gtk.h>
#include <lingoperate.h>

G_BEGIN_DECLS

#define LING_TYPE_NOTIFY_ITEM (ling_notify_item_get_type())
G_DECLARE_FINAL_TYPE(LingNotifyItem,ling_notify_item,LING,NOTIFY_ITEM,GtkBox)

GtkWidget * ling_notify_item_new();

void ling_notify_item_set_image(LingNotifyItem * self,const char * icon_name);

void ling_notify_item_set_text(LingNotifyItem * self,const char * text);

void ling_notify_item_set_time(LingNotifyItem * self,const char * text);

G_END_DECLS
