#pragma once

#include <gtk/gtk.h>
#include <folder.h>
G_BEGIN_DECLS

#define CL_TYPE_BLE_LIST (cl_ble_list_get_type())
G_DECLARE_FINAL_TYPE(ClBleList,cl_ble_list,CL,BLE_LIST,GtkBox)

GtkWidget * cl_ble_list_new();

#define CL_TYPE_BLE_LIST_ITEM (cl_ble_list_item_get_type())
G_DECLARE_FINAL_TYPE(ClBleListItem,cl_ble_list_item,CL,BLE_LIST_ITEM,GtkBox)

GtkWidget * cl_ble_list_item_new();

G_END_DECLS
