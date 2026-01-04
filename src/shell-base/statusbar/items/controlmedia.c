#include <controlmedia.h>

struct _ClControlMedia{
    GtkScale parent;
};

G_DEFINE_FINAL_TYPE(ClControlMedia,cl_control_media,GTK_TYPE_BUTTON);


static void cl_control_media_class_init(ClControlMediaClass * klass){

}

static void cl_control_media_init(ClControlMedia * self){
    gtk_widget_set_vexpand(GTK_WIDGET(self),1);
    gtk_widget_set_hexpand(GTK_WIDGET(self),1);
    gtk_widget_add_css_class(GTK_WIDGET(self),"control-item");
    gtk_widget_add_css_class(GTK_WIDGET(self),"control-media");
}

GtkWidget * cl_control_media_new(){
    return g_object_new(CL_TYPE_CONTROL_MEDIA,NULL);
}
