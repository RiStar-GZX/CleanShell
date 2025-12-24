#include "lingtaskswitcher.h"
#include <lingviewpager.h>

enum{
    LAYER_TOP = 0,

    LAYER_TASK_SWITCHER,
    LAYER_BAR,

    LAYER_END = 255
};

struct _LingTaskSwitcher{
    GtkBox parent;
    GtkWidget * overlay;

    //条
    GtkWidget * bar;
    //切换器
    GtkWidget * viewpager;
    GtkWidget * clear_button;
};

G_DEFINE_FINAL_TYPE(LingTaskSwitcher,ling_task_switcher,GTK_TYPE_BOX);

static void ling_task_switcher_class_init(LingTaskSwitcherClass * klass){
}

static void ling_task_switcher_init(LingTaskSwitcher * self){
    gtk_orientable_set_orientation(GTK_ORIENTABLE(self),GTK_ORIENTATION_VERTICAL);

    gtk_widget_set_valign(GTK_WIDGET(self),GTK_ALIGN_END);

    self->overlay = ling_overlay_new();
    //条
    self->bar = gtk_button_new();
    gtk_widget_set_halign(self->bar,GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(self->bar,100,1);

    //gtk_widget_set_margin_top(self->bar,100);
    gtk_widget_set_valign(self->bar,GTK_ALIGN_END);
    ling_overlay_add_layer(LING_OVERLAY(self->overlay),self->bar,LAYER_BAR);

    //self->viewpager = ling_view_pager_new_with_op();


    //self->clear_button = gtk_button_new();

    gtk_box_append(GTK_BOX(self),self->overlay);
}

GtkWidget * ling_task_switcher_new(){
    return g_object_new(LING_TYPE_TASK_SWITCHER,NULL);
}
