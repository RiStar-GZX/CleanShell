#include "lingnotifyitem.h"

struct _LingNotifyItem{
    GtkBox parent;

    GtkWidget * box;
    GtkWidget * image;
    GtkWidget * text;
    GtkWidget * time;
};


G_DEFINE_FINAL_TYPE(LingNotifyItem,ling_notify_item,GTK_TYPE_BOX);

static void ling_notify_item_class_init(LingNotifyItemClass * klass){

}

static void ling_notify_item_init(LingNotifyItem * self){
    GtkBuilder * builder = gtk_builder_new_from_file("../../ui/notification center.ui");
    self->box  =  GTK_WIDGET(gtk_builder_get_object(builder,"notification_item"));
    gtk_widget_add_css_class(GTK_WIDGET(self->box),"notify_item");
    gtk_widget_add_css_class(GTK_WIDGET(self),"notify_item_box");
    gtk_box_append(GTK_BOX(self),self->box);
    self->image =  GTK_WIDGET(gtk_builder_get_object(builder,"item_image"));
    gtk_image_set_pixel_size(GTK_IMAGE(self->image),35);
    self->text =  GTK_WIDGET(gtk_builder_get_object(builder,"item_text"));
    gtk_widget_add_css_class(self->text,"notify_item_text");
    gtk_widget_set_halign(self->text,GTK_ALIGN_START);
    gtk_label_set_lines(GTK_LABEL(self->text),3);
    self->time =  GTK_WIDGET(gtk_builder_get_object(builder,"item_time"));
}

GtkWidget * ling_notify_item_new(){
    return g_object_new(LING_TYPE_NOTIFY_ITEM,NULL);
}

void ling_notify_item_set_image(LingNotifyItem * self,const char * icon_name){
    gtk_image_set_from_icon_name(GTK_IMAGE(self->image),icon_name);
}

void ling_notify_item_set_text(LingNotifyItem * self,const char * text){
    gtk_label_set_label(GTK_LABEL(self->text),text);
}

void ling_notify_item_set_time(LingNotifyItem * self,const char * text){
    gtk_label_set_label(GTK_LABEL(self->time),text);
}
