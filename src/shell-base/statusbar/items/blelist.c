#include "blelist.h"


struct _ClBleList{
    GtkBox parent;
    GtkWidget * folder;
    GtkWidget * scrollwin;
    GtkWidget * listbox;
};

G_DEFINE_FINAL_TYPE(ClBleList,cl_ble_list,GTK_TYPE_BOX);

struct _ClBleListItem{
    GtkBox box;
    GtkWidget * image;
    GtkWidget * name;
    GtkWidget * status;
};

G_DEFINE_FINAL_TYPE(ClBleListItem,cl_ble_list_item,GTK_TYPE_BOX);

static void cl_ble_list_item_class_init(ClBleListItemClass * klass){

}

static void cl_ble_list_item_init(ClBleListItem * self){
    gtk_orientable_set_orientation(GTK_ORIENTABLE(self),GTK_ORIENTATION_HORIZONTAL);
    self->image = gtk_image_new();
    self->name = gtk_label_new("");
    self->status = gtk_label_new("");
    gtk_box_append(GTK_BOX(self),self->name);
    gtk_box_append(GTK_BOX(self),self->status);
    gtk_box_append(GTK_BOX(self),self->image);
}

void cl_ble_list_item_set_name(ClBleListItem * self,const char * name){
    gtk_label_set_label(GTK_LABEL(self->name),name);
}

void cl_ble_list_item_set_status(ClBleListItem * self,const char * status){
    gtk_label_set_label(GTK_LABEL(self->status),status);
}

void cl_ble_list_item_set_icon(ClBleListItem * self,GIcon * icon){
    gtk_image_set_from_gicon(GTK_IMAGE(self->image),icon);
}

GtkWidget * cl_ble_list_item_new(){
    return  g_object_new(CL_TYPE_BLE_LIST_ITEM,NULL);
}

static void cl_ble_list_class_init(ClBleListClass * klass){

}

static void cl_ble_list_init(ClBleList * self){
    self->scrollwin = gtk_scrolled_window_new();

    self->folder = cl_folder_new("蓝牙",self->scrollwin);
    gtk_box_append(GTK_BOX(self),self->folder);
    self->listbox = gtk_list_box_new();
    gtk_widget_set_hexpand(self->listbox,TRUE);
    gtk_widget_set_vexpand(self->listbox,TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(self->scrollwin),self->listbox);

    GtkWidget * item1 = cl_ble_list_item_new();
    cl_ble_list_item_set_icon(CL_BLE_LIST_ITEM(item1),g_icon_new_for_string("error",NULL));
    cl_ble_list_item_set_name(CL_BLE_LIST_ITEM(item1),"item1");
    cl_ble_list_item_set_status(CL_BLE_LIST_ITEM(item1),"未连接");
    gtk_list_box_append(GTK_LIST_BOX(self->listbox),item1);

    GtkWidget * item2 = cl_ble_list_item_new();
    cl_ble_list_item_set_icon(CL_BLE_LIST_ITEM(item2),g_icon_new_for_string("error",NULL));
    cl_ble_list_item_set_name(CL_BLE_LIST_ITEM(item2),"item2");
    cl_ble_list_item_set_status(CL_BLE_LIST_ITEM(item2),"未连接");
    gtk_list_box_append(GTK_LIST_BOX(self->listbox),item2);

    GtkWidget * item3 = cl_ble_list_item_new();
    cl_ble_list_item_set_icon(CL_BLE_LIST_ITEM(item3),g_icon_new_for_string("error",NULL));
    cl_ble_list_item_set_name(CL_BLE_LIST_ITEM(item3),"item3");
    cl_ble_list_item_set_status(CL_BLE_LIST_ITEM(item3),"未连接");
    gtk_list_box_append(GTK_LIST_BOX(self->listbox),item3);

}

GtkWidget * cl_ble_list_new(){
    return g_object_new(CL_TYPE_BLE_LIST,NULL);
}
