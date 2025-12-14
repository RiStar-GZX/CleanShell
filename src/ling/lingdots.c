#include <lingdots.h>

struct _LingDot{
    GtkButton parent;
};

G_DEFINE_FINAL_TYPE(LingDot,ling_dot,GTK_TYPE_BOX)

static void ling_dot_class_init(LingDotClass * klass){
}

static void ling_dot_init(LingDot * self){
    gtk_widget_set_size_request(GTK_WIDGET(self), 10, 10);
    gtk_widget_add_css_class(GTK_WIDGET(self), "dot");
}
//gtk_widget_remove_css_class(dot, "dot");
//gtk_widget_add_css_class(dot, "active-dot");



// void ling_dot_indicator_set_dot_pos(LingDotIndicator * self,uint pos){
//     if(pos>self->num)return;
//     GtkWidget * dot = gtk_widget_get_first_child(GTK_WIDGET(self));
//     for(int i=1;i<=self->num;i++){
//         if(dot==NULL)return;
//         if(i==pos){
//             gtk_widget_remove_css_class(dot, "dot");
//             gtk_widget_add_css_class(dot, "active-dot");
//         }
//         else{
//             gtk_widget_remove_css_class(dot, "active-dot");
//             gtk_widget_add_css_class(dot, "dot");
//         }
//         dot = gtk_widget_get_next_sibling(dot);
//     }
// }

// void ling_dot_indicator_set_dot_num(LingDotIndicator * self,uint num){
//     if(num>self->num){
//         for(int i=0;i<num-self->num;i++){
//             GtkWidget *dot = gtk_button_new();
//             gtk_widget_set_size_request(dot, 10, 10);
//             gtk_widget_add_css_class(dot, "dot");
//             gtk_box_append(GTK_BOX(self), dot);
//         }
//     }
//     else if(num<self->num){
//         for(int i=0;i<self->num-num;i++){
//             GtkWidget * t = gtk_widget_get_first_child(GTK_WIDGET(self));
//             gtk_box_remove(GTK_BOX(self),t);
//         }
//     }
//     self->num=num;
//     if(self->pos<num){
//         ling_dot_indicator_set_dot_pos(self,1);
//     }
// }

GtkWidget * ling_dot_new(void){
    return GTK_WIDGET(g_object_new(LING_TYPE_DOT,NULL));
}


enum{
    PROP_DOTS_NUM =1,
    PROP_DOTS_MODE ,
    N_PROPERTIES
};


static GParamSpec *properties[N_PROPERTIES] = {NULL};


struct _LingDots{
    GtkBox parent;
    uint mode;
    uint dots_num;
    uint pos;
};

G_DEFINE_FINAL_TYPE(LingDots,ling_dots,GTK_TYPE_BOX)

void ling_dots_ativate(LingDots * dots,uint pos){
    if(pos>dots->dots_num)return;
    GtkWidget * dot = gtk_widget_get_first_child(GTK_WIDGET(dots));
    for(int i=1;i<=dots->dots_num;i++){
        if((dots->mode==DOTS_MODE_MULTI&&i<=pos)||
            (dots->mode==DOTS_MODE_SINGAL&&i==pos)){
            gtk_widget_remove_css_class(dot, "dot");
            gtk_widget_add_css_class(dot, "active-dot");
        }
        else{
            gtk_widget_remove_css_class(dot, "active-dot");
            gtk_widget_add_css_class(dot, "dot");
        }
        dot=gtk_widget_get_next_sibling(dot);
    }
    dots->pos = pos;
}

int ling_dots_num(LingDots * dots){
    return dots->pos;
}


void ling_dots_set_num(LingDots * dots,uint num){
    int a = num - dots->dots_num;
    dots->dots_num = num;
    if(a>0){
        for(int i=0;i<a;i++){
            GtkWidget * dot = ling_dot_new();
            gtk_box_append(GTK_BOX(dots),dot);
        }
    }
    if(a<0){
        for(int i=0;i<a;i++){
            GtkWidget * last=gtk_widget_get_last_child(GTK_WIDGET(dots));
            gtk_box_remove(GTK_BOX(dots),last);
        }
        if(dots->pos>num)dots->pos=num;
    }
    ling_dots_ativate(dots,dots->pos);
}

void ling_dots_set_mode(LingDots * dots,uint mode){
    if(mode>=N_DOTS_MODE)return;
    dots->mode = mode;
    ling_dots_ativate(dots,dots->pos);
}

static void set_property(GObject *object,guint property_id,
                         const GValue *value,GParamSpec *pspec){
    LingDots * self = LING_DOTS(object);

    switch(property_id){
    case PROP_DOTS_NUM:
        ling_dots_set_num(self,g_value_get_uint(value));
        break;
    case PROP_DOTS_MODE:
        ling_dots_set_mode(self,g_value_get_uint(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object,property_id,pspec);
        break;
    }
}
static void get_property(GObject *object,guint property_id,
                         GValue *value,GParamSpec *pspec){
    LingDots * self = LING_DOTS(object);

    switch(property_id){
    case PROP_DOTS_NUM:
        g_value_set_uint(value,self->dots_num);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object,property_id,pspec);
        break;
    }
}

static void ling_dots_class_init(LingDotsClass * klass){
    GObjectClass * obj_class = (GObjectClass *)klass;
    obj_class->set_property = set_property;
    obj_class->get_property = get_property;
    properties[PROP_DOTS_NUM] = g_param_spec_uint("dots_num","dots num",
                                                    "number of dots",0,100,4,G_PARAM_READWRITE);
    properties[PROP_DOTS_MODE] = g_param_spec_uint("mode","dots mode",
                                                    "dots mode",0,100,DOTS_MODE_SINGAL,G_PARAM_READWRITE);
    g_object_class_install_properties(obj_class,N_PROPERTIES,properties);
}

static void ling_dots_init(LingDots * self){

}

GtkWidget * ling_dots_new(uint num,uint mode){
    return g_object_new(LING_TYPE_DOTS,"dots_num",num,"mode",mode,NULL);
}
