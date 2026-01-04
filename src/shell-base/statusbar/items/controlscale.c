#include <controlscale.h>



struct _ClControlScale{
    GtkScale parent;
};

G_DEFINE_FINAL_TYPE(ClControlScale,cl_control_scale,GTK_TYPE_SCALE);


static void cl_control_scale_class_init(ClControlScaleClass * klass){

}

static void cl_control_scale_init(ClControlScale * self){
    gtk_widget_add_css_class(GTK_WIDGET(self),"ling-scale");
}

GtkWidget * cl_control_scale_new(){
    return g_object_new(CL_TYPE_CONTROL_SCALE,NULL);
}
