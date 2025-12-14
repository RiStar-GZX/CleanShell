#include <lingnotifyviewer.h>

struct _LingNotifyViewer{
    GtkBox parent;
    GtkWidget * listbox;
};

G_DEFINE_FINAL_TYPE(LingNotifyViewer,ling_notify_viewer,GTK_TYPE_BOX);

void ling_notify_viewer_class_init(LingNotifyViewerClass * klass){

}

void ling_notify_viewer_init(LingNotifyViewer * self){
    self->listbox=gtk_list_box_new();
}

GtkWidget * ling_notify_view_new(){
    LingNotifyViewer * self =LING_NOTIFY_VIEWER(g_object_new(LING_TYPE_NOTIFY_VIEWER,NULL));
    return GTK_WIDGET(self);
}
