#pragma once

#include <gtk/gtk.h>
#include <lingfolder.h>

G_BEGIN_DECLS

#define LING_TYPE_WLAN_LIST (ling_wlan_list_get_type())
G_DECLARE_FINAL_TYPE(LingWlanList,ling_wlan_list,LING,WLAN_LIST,GtkBox)

GtkWidget * ling_wlan_list_new();

#define LING_TYPE_WLAN_LIST_ITEM (ling_wlan_list_item_get_type())
G_DECLARE_FINAL_TYPE(LingWlanListItem,ling_wlan_list_item,LING,WLAN_LIST_ITEM,GtkBox)

GtkWidget * ling_wlan_list_item_new();

G_END_DECLS
