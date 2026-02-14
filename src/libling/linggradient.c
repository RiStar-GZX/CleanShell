#include "linggradient.h"
#include <lingoverlay.h>

enum{
    LAYER_TOP,
    LAYER_UP,
    LAYER_BOTTOM,
};

struct _LingGradient{
    GtkBox parent;
    GtkWidget * overlay;
    GtkWidget * top;
    GtkWidget * bottom;
};

G_DEFINE_FINAL_TYPE(LingGradient,ling_gradient,GTK_TYPE_BOX)

static void ling_gradient_class_init(LingGradientClass * klass){

}

static void ling_gradient_init(LingGradient * self){
    self->overlay = ling_overlay_new();
    gtk_box_append(GTK_BOX(self),self->overlay);
}

GtkWidget * ling_gradient_new(GtkWidget * top,GtkWidget * bottom){
    LingGradient * self = LING_GRADIENT(g_object_new(LING_TYPE_GRADIENT,NULL));
    self->top = top;
    self->bottom = bottom;
    ling_overlay_add_layer(LING_OVERLAY(self->overlay),self->top,LAYER_TOP);
    ling_overlay_add_layer(LING_OVERLAY(self->overlay),self->bottom,LAYER_BOTTOM);
    ling_gradient_progress(self,0.0);
    return GTK_WIDGET(self);
}

void ling_gradient_progress(LingGradient * gradient,gdouble progress){
    if(progress>1.0)progress=1.0;
    if(progress<0)progress=0;
    gtk_widget_set_opacity(gradient->top,progress);
    gtk_widget_set_opacity(gradient->bottom,1-progress);
}

GtkWidget * ling_gradient_get_top(LingGradient * gradient){
    return gradient->top;
}

GtkWidget * ling_gradient_get_bottom(LingGradient * gradient){
    return gradient->bottom;
}
