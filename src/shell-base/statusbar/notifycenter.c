#include "notifycenter.h"

struct _ClNotifyCenter{
    GtkBox parent;

    GtkWidget * center_head;

    GtkWidget * notify_head_box;
    GtkWidget * ntf_clear;
    GtkWidget * ntf_able;

    GtkWidget * time_label;
    GtkWidget * date_label;
    GtkWidget * time_box;

    GtkWidget * notify_box;
    GtkWidget * list_box;

    GList * ntf_list;

};


G_DEFINE_FINAL_TYPE(ClNotifyCenter,cl_notify_center,GTK_TYPE_BOX);

void cl_notify_view_notify_update(ClNotifyCenter * self){
    gtk_list_box_remove_all(GTK_LIST_BOX(self->list_box));
    char str[3][3][100]={"qq","钢钒\n"
                     "你只是个小丑罢了！","4:14",
                     "wechat","星歌\n"
                     "你想清楚你该走向何方了吗？","20:24",
                     "spotify","星歌\n"
                           "路在脚下，行则将至","20:25"};

    for(int i=0;i<3;i++){
        //for(GList * l=self->ntf_list;l!=NULL;l=l->next){
        //  sys_info_notify * notify =(sys_info_notify *) l->data;
        GtkWidget * item = ling_notify_item_new();
        ling_notify_item_set_image(LING_NOTIFY_ITEM(item),str[i][0]);
        ling_notify_item_set_text(LING_NOTIFY_ITEM(item),str[i][1]);
        ling_notify_item_set_time(LING_NOTIFY_ITEM(item),str[i][2]);
        gtk_list_box_append(GTK_LIST_BOX(self->list_box),item);
        //}
    }
}


static void cl_notify_center_class_init(ClNotifyCenterClass * klass){

}

static void cl_notify_center_init(ClNotifyCenter * self){
    self->center_head = cl_center_head_new();
    gtk_box_append(GTK_BOX(self),self->center_head);

    GtkBuilder * builder = gtk_builder_new_from_file("../../ui/notification center.ui");
    self->notify_box = GTK_WIDGET(gtk_builder_get_object(builder,"notify_box"));
    self->list_box = GTK_WIDGET(gtk_builder_get_object(builder,"main_list_box"));
    gtk_widget_add_css_class(self->list_box,"notify_list_box");
    self->notify_head_box = GTK_WIDGET(gtk_builder_get_object(builder,"notify_head_box"));
    self->ntf_clear = GTK_WIDGET(gtk_builder_get_object(builder,"notify_clear"));
    gtk_widget_add_css_class(self->ntf_clear,"notify_clear");

    cl_center_head_box_append(CL_CENTER_HEAD(self->center_head),self->notify_head_box);
    cl_center_head_append(CL_CENTER_HEAD(self->center_head),self->notify_box);

    cl_notify_view_notify_update(self);
}

GtkWidget * cl_notify_center_new(){
    return GTK_WIDGET(g_object_new(CL_TYPE_NOTIFY_CENTER,NULL));
}


