#include "lingtimeviewer.h"

struct _LingTimeViewer{
    GtkBox parent;
    GtkWidget * label;
};

G_DEFINE_FINAL_TYPE(LingTimeViewer,ling_time_viewer,GTK_TYPE_BOX);

void ling_time_viewer_class_init(LingTimeViewerClass * klass){

}

void ling_time_viewer_init(LingTimeViewer * self){
    gtk_orientable_set_orientation(GTK_ORIENTABLE(self),GTK_ORIENTATION_VERTICAL);
    self->label = gtk_label_new("00:00");
    gtk_widget_add_css_class(self->label,"time_viewer");
    gtk_box_append(GTK_BOX(self),self->label);
}

GtkWidget * ling_time_viewer_new(){
    return GTK_WIDGET(g_object_new(LING_TYPE_TIME_VIEWER,NULL));
}

void ling_time_viewer_set_time(LingTimeViewer * self,const char * time){
    gtk_label_set_label(GTK_LABEL(self->label),time);
}
