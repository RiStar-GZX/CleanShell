#include "controlitem.h"

enum{
    LAYER_TOP = 0,

    LAYER_FOLDER,
    LAYER_BUTTON,

    LAYER_END = 255
};

struct _ClControlItem
{
    GtkBox parent;
    GtkWidget * overlay;
    GtkWidget * folder;
    GtkWidget * button;
};

G_DEFINE_FINAL_TYPE(ClControlItem,cl_control_item,GTK_TYPE_BOX);

static void cl_control_item_class_init(ClControlItemClass * klass){

}

static void cl_control_item_init(ClControlItem * self){
    self->overlay = ling_overlay_new();
    gtk_widget_set_hexpand(self->overlay,TRUE);
    gtk_widget_set_vexpand(self->overlay,TRUE);
    gtk_box_append(GTK_BOX(self),self->overlay);
}

GtkWidget * cl_control_item_new(GtkWidget * button,GtkWidget * folder){
    ClControlItem * self =  CL_CONTROL_ITEM(g_object_new(CL_TYPE_CONTROL_ITEM,NULL));
    self->button = button;
    self->folder = folder;
    ling_overlay_add_layer(LING_OVERLAY(self->overlay),self->button,LAYER_BUTTON);
    ling_overlay_add_layer(LING_OVERLAY(self->overlay),self->folder,LAYER_FOLDER);
    gtk_widget_set_visible(self->folder,FALSE);
    return GTK_WIDGET(self);
}

GtkWidget * cl_control_item_get_button(ClControlItem * self){
    return self->button;
}

GtkWidget * cl_control_item_get_folder(ClControlItem * self){
    return self->folder;
}

