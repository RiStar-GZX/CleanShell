#include <lingblelist.h>


struct _LingBleList{
    GtkBox parent;
    GtkWidget * folder;
    GtkWidget * scrollwin;
    GtkWidget * listbox;
};

G_DEFINE_FINAL_TYPE(LingBleList,ling_ble_list,GTK_TYPE_BOX);

struct _LingBleListItem{
    GtkBox box;
    GtkWidget * image;
    GtkWidget * name;
    GtkWidget * status;
};

G_DEFINE_FINAL_TYPE(LingBleListItem,ling_ble_list_item,GTK_TYPE_BOX);

static void ling_ble_list_item_class_init(LingBleListItemClass * klass){

}

static void ling_ble_list_item_init(LingBleListItem * self){
    gtk_orientable_set_orientation(GTK_ORIENTABLE(self),GTK_ORIENTATION_HORIZONTAL);
    self->image = gtk_image_new();
    self->name = gtk_label_new("");
    self->status = gtk_label_new("");
    gtk_box_append(GTK_BOX(self),self->name);
    gtk_box_append(GTK_BOX(self),self->status);
    gtk_box_append(GTK_BOX(self),self->image);
}

void ling_ble_list_item_set_name(LingBleListItem * self,const char * name){
    gtk_label_set_label(GTK_LABEL(self->name),name);
}

void ling_ble_list_item_set_status(LingBleListItem * self,const char * status){
    gtk_label_set_label(GTK_LABEL(self->status),status);
}

void ling_ble_list_item_set_icon(LingBleListItem * self,GIcon * icon){
    gtk_image_set_from_gicon(GTK_IMAGE(self->image),icon);
}

GtkWidget * ling_ble_list_item_new(){
    return  g_object_new(LING_TYPE_BLE_LIST_ITEM,NULL);
}

static void ling_ble_list_class_init(LingBleListClass * klass){

}

static void ling_ble_list_init(LingBleList * self){
    self->scrollwin = gtk_scrolled_window_new();

    self->folder = ling_folder_new("蓝牙",self->scrollwin);
    gtk_box_append(GTK_BOX(self),self->folder);
    self->listbox = gtk_list_box_new();
    gtk_widget_set_hexpand(self->listbox,TRUE);
    gtk_widget_set_vexpand(self->listbox,TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(self->scrollwin),self->listbox);

    GtkWidget * item1 = ling_ble_list_item_new();
    ling_ble_list_item_set_icon(LING_BLE_LIST_ITEM(item1),g_icon_new_for_string("error",NULL));
    ling_ble_list_item_set_name(LING_BLE_LIST_ITEM(item1),"item1");
    ling_ble_list_item_set_status(LING_BLE_LIST_ITEM(item1),"未连接");
    gtk_list_box_append(GTK_LIST_BOX(self->listbox),item1);

    GtkWidget * item2 = ling_ble_list_item_new();
    ling_ble_list_item_set_icon(LING_BLE_LIST_ITEM(item2),g_icon_new_for_string("error",NULL));
    ling_ble_list_item_set_name(LING_BLE_LIST_ITEM(item2),"item2");
    ling_ble_list_item_set_status(LING_BLE_LIST_ITEM(item2),"未连接");
    gtk_list_box_append(GTK_LIST_BOX(self->listbox),item2);

    GtkWidget * item3 = ling_ble_list_item_new();
    ling_ble_list_item_set_icon(LING_BLE_LIST_ITEM(item3),g_icon_new_for_string("error",NULL));
    ling_ble_list_item_set_name(LING_BLE_LIST_ITEM(item3),"item3");
    ling_ble_list_item_set_status(LING_BLE_LIST_ITEM(item3),"未连接");
    gtk_list_box_append(GTK_LIST_BOX(self->listbox),item3);

}

GtkWidget * ling_ble_list_new(){
    return g_object_new(LING_TYPE_BLE_LIST,NULL);
}
