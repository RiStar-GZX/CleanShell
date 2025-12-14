#include <lingcontrolbutton.h>

struct _LingControlButton{
    GtkBox parent;
    GtkWidget * button;
    GtkWidget * image;
    GtkWidget * label;
};

G_DEFINE_FINAL_TYPE(LingControlButton,ling_control_button,GTK_TYPE_BOX);


static void ling_control_button_class_init(LingControlButtonClass * klass){

}

static void ling_control_button_init(LingControlButton * self){
    gtk_widget_add_css_class(GTK_WIDGET(self),"control-item");
    gtk_widget_set_vexpand(GTK_WIDGET(self),1);
    gtk_widget_set_hexpand(GTK_WIDGET(self),1);

    self->button = gtk_toggle_button_new();
    gtk_widget_set_vexpand(GTK_WIDGET(self->button),1);
    gtk_widget_set_hexpand(GTK_WIDGET(self->button),1);
    gtk_widget_add_css_class(GTK_WIDGET(self->button),"control-button");

    self->image = gtk_image_new();//_from_icon_name("network-wireless-100");
    gtk_image_set_pixel_size(GTK_IMAGE(self->image),30);
    gtk_button_set_child(GTK_BUTTON(self->button),self->image);

    self->label = gtk_label_new("");
    gtk_widget_set_vexpand(GTK_WIDGET(self->label),1);
    gtk_widget_set_hexpand(GTK_WIDGET(self->label),1);
    gtk_widget_add_css_class(GTK_WIDGET(self->label),"control-label");

    gtk_box_append(GTK_BOX(self),self->button);
    gtk_box_append(GTK_BOX(self),self->label);
}

GtkWidget * ling_control_button_new(const char * label,const char * icon_name){
    LingControlButton * self = LING_CONTROL_BUTTON(g_object_new(LING_TYPE_CONTROL_BUTTON,NULL));
    gtk_label_set_label(GTK_LABEL(self->label),label);
    gtk_image_set_from_icon_name(GTK_IMAGE(self->image),icon_name);
    return GTK_WIDGET(self);
}
