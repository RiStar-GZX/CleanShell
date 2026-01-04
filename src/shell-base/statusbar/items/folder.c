#include <folder.h>

struct _ClFolder{
    GtkBox parent;
    GtkWidget * body_box;
    GtkWidget * title_label;
    int inout;
};

G_DEFINE_FINAL_TYPE(ClFolder,cl_folder,GTK_TYPE_BOX);


static void cl_folder_class_init(ClFolderClass * klass){

}

void in_pressed(GtkGestureClick* gesture,gint n_press,
  gdouble x,gdouble y,ClFolder * self){
    self->inout = IN_FOLDER;
    g_print("pressed\n");
}

void all_released(GtkGestureClick* gesture,gint n_press,
  gdouble x,gdouble y,ClFolder * self){
    g_print("released\n");
    if(self->inout==OUT_FOLDER){
        gtk_widget_set_visible(gtk_widget_get_parent(GTK_WIDGET(self)),FALSE);
    }
    self->inout = OUT_FOLDER;
}

static void cl_folder_init(ClFolder * self){
    self->body_box = gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
    gtk_box_append(GTK_BOX(self),self->body_box);
    gtk_widget_set_margin_top(self->body_box,200);
    gtk_widget_set_margin_bottom(self->body_box,200);
    gtk_widget_set_margin_start(self->body_box,60);
    gtk_widget_set_margin_end(self->body_box,60);
    self->title_label = gtk_label_new("");
    gtk_box_append(GTK_BOX(self->body_box),self->title_label);

    self->inout = OUT_FOLDER;
    GtkGesture * in = gtk_gesture_click_new();
    gtk_widget_add_controller(GTK_WIDGET(self->body_box),GTK_EVENT_CONTROLLER(in));
    g_signal_connect(in,"pressed",G_CALLBACK(in_pressed),self);
    GtkGesture * all = gtk_gesture_click_new();
    gtk_widget_add_controller(GTK_WIDGET(self),GTK_EVENT_CONTROLLER(all));
    g_signal_connect(all,"released",G_CALLBACK(all_released),self);
}

GtkWidget * cl_folder_new(const char * title,GtkWidget * content){
    ClFolder * self = CL_FOLDER(g_object_new(CL_TYPE_FOLDER,NULL));
    gtk_box_append(GTK_BOX(self->body_box),content);
    gtk_label_set_label(GTK_LABEL(self->title_label),title);
    return GTK_WIDGET(self);
}
