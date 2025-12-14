#pragma once

#include <gtk/gtk.h>
#include <lingfolder.h>
G_BEGIN_DECLS

#define LING_TYPE_BLE_LIST (ling_ble_list_get_type())
G_DECLARE_FINAL_TYPE(LingBleList,ling_ble_list,LING,BLE_LIST,GtkBox)

GtkWidget * ling_ble_list_new();

#define LING_TYPE_BLE_LIST_ITEM (ling_ble_list_item_get_type())
G_DECLARE_FINAL_TYPE(LingBleListItem,ling_ble_list_item,LING,BLE_LIST_ITEM,GtkBox)

GtkWidget * ling_ble_list_item_new();

G_END_DECLS
