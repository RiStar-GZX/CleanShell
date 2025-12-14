#include <lingcontrolmedia.h>

struct _LingControlMedia{
    GtkScale parent;
};

G_DEFINE_FINAL_TYPE(LingControlMedia,ling_control_media,GTK_TYPE_BUTTON);


static void ling_control_media_class_init(LingControlMediaClass * klass){

}

static void ling_control_media_init(LingControlMedia * self){
    gtk_widget_set_vexpand(GTK_WIDGET(self),1);
    gtk_widget_set_hexpand(GTK_WIDGET(self),1);
    gtk_widget_add_css_class(GTK_WIDGET(self),"control-item");
    gtk_widget_add_css_class(GTK_WIDGET(self),"control-media");
}

GtkWidget * ling_control_media_new(){
    return g_object_new(LING_TYPE_CONTROL_MEDIA,NULL);
}
