#include <lingcontrolscale.h>



struct _LingControlScale{
    GtkScale parent;
};

G_DEFINE_FINAL_TYPE(LingControlScale,ling_control_scale,GTK_TYPE_SCALE);


static void ling_control_scale_class_init(LingControlScaleClass * klass){

}

static void ling_control_scale_init(LingControlScale * self){
    gtk_widget_add_css_class(GTK_WIDGET(self),"ling-scale");
}

GtkWidget * ling_control_scale_new(){
    return g_object_new(LING_TYPE_CONTROL_SCALE,NULL);
}
