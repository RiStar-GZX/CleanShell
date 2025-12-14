#include <lingwlanlist.h>

struct _LingWlanList{
    GtkBox parent;
    GtkWidget * folder;
    GtkWidget * scrollwin;
    GtkWidget * listbox;
};

G_DEFINE_FINAL_TYPE(LingWlanList,ling_wlan_list,GTK_TYPE_BOX);

struct _LingWlanListItem{
    GtkBox box;
    GtkWidget * image;
    GtkWidget * name;
    GtkWidget * status;
};

G_DEFINE_FINAL_TYPE(LingWlanListItem,ling_wlan_list_item,GTK_TYPE_BOX);

static void ling_wlan_list_item_class_init(LingWlanListItemClass * klass){

}

static void ling_wlan_list_item_init(LingWlanListItem * self){
    gtk_orientable_set_orientation(GTK_ORIENTABLE(self),GTK_ORIENTATION_HORIZONTAL);
    self->image = gtk_image_new();
    self->name = gtk_label_new("");
    self->status = gtk_label_new("");
    gtk_box_append(GTK_BOX(self),self->image);
    gtk_box_append(GTK_BOX(self),self->name);
    gtk_box_append(GTK_BOX(self),self->status);
}

void ling_wlan_list_item_set_name(LingWlanListItem * self,const char * name){
    gtk_label_set_label(GTK_LABEL(self->name),name);
}

void ling_wlan_list_item_set_status(LingWlanListItem * self,const char * status){
    gtk_label_set_label(GTK_LABEL(self->status),status);
}

void ling_wlan_list_item_set_icon(LingWlanListItem * self,GIcon * icon){
    gtk_image_set_from_gicon(GTK_IMAGE(self->image),icon);
}

GtkWidget * ling_wlan_list_item_new(){
    return  g_object_new(LING_TYPE_WLAN_LIST_ITEM,NULL);
}

static void ling_wlan_list_class_init(LingWlanListClass * klass){

}

static void ling_wlan_list_init(LingWlanList * self){
    self->scrollwin = gtk_scrolled_window_new();


    self->folder = ling_folder_new("wlan",self->scrollwin);
    gtk_box_append(GTK_BOX(self),self->folder);
    self->listbox = gtk_list_box_new();
    gtk_widget_set_hexpand(self->listbox,TRUE);
    gtk_widget_set_vexpand(self->listbox,TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(self->scrollwin),self->listbox);

    GtkWidget * item1 = ling_wlan_list_item_new();
    ling_wlan_list_item_set_icon(LING_WLAN_LIST_ITEM(item1),g_icon_new_for_string("error",NULL));
    ling_wlan_list_item_set_name(LING_WLAN_LIST_ITEM(item1),"item1");
    ling_wlan_list_item_set_status(LING_WLAN_LIST_ITEM(item1),"未连接");
    gtk_list_box_append(GTK_LIST_BOX(self->listbox),item1);

    GtkWidget * item2 = ling_wlan_list_item_new();
    ling_wlan_list_item_set_icon(LING_WLAN_LIST_ITEM(item2),g_icon_new_for_string("error",NULL));
    ling_wlan_list_item_set_name(LING_WLAN_LIST_ITEM(item2),"item2");
    ling_wlan_list_item_set_status(LING_WLAN_LIST_ITEM(item2),"未连接");
    gtk_list_box_append(GTK_LIST_BOX(self->listbox),item2);

    GtkWidget * item3 = ling_wlan_list_item_new();
    ling_wlan_list_item_set_icon(LING_WLAN_LIST_ITEM(item3),g_icon_new_for_string("error",NULL));
    ling_wlan_list_item_set_name(LING_WLAN_LIST_ITEM(item3),"item3");
    ling_wlan_list_item_set_status(LING_WLAN_LIST_ITEM(item3),"未连接");
    gtk_list_box_append(GTK_LIST_BOX(self->listbox),item3);

}

GtkWidget * ling_wlan_list_new(){
    return  g_object_new(LING_TYPE_WLAN_LIST,NULL);
    //ling_wlan_list_init(LING_WLAN_LIST(self));
    //return self;
}
