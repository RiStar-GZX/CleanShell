#pragma once

#include <gtk/gtk.h>
#include <folder.h>

G_BEGIN_DECLS

#define CL_TYPE_WLAN_LIST (cl_wlan_list_get_type())
G_DECLARE_FINAL_TYPE(ClWlanList,cl_wlan_list,CL,WLAN_LIST,GtkBox)

GtkWidget * cl_wlan_list_new();

#define CL_TYPE_WLAN_LIST_ITEM (cl_wlan_list_item_get_type())
G_DECLARE_FINAL_TYPE(ClWlanListItem,cl_wlan_list_item,CL,WLAN_LIST_ITEM,GtkBox)

GtkWidget * cl_wlan_list_item_new();

G_END_DECLS
