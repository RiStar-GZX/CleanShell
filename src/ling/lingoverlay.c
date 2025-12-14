#include "lingoverlay.h"

typedef struct LingLayer{
    GtkWidget * widget;
    uint8_t level;
    GtkGesture * drag;
    GtkGesture * swipe;
    GList * switchers;
}LingLayer;

typedef struct switcher{
    LingLayer * to;
    uint op_type;   //操作类型
    uint ani_type;  //动画类型
}switcher;

struct _LingOverlay{
    GtkBox parent;
    GtkWidget * overlay;
    GList * layers;
};

G_DEFINE_FINAL_TYPE(LingOverlay,ling_overlay,GTK_TYPE_BOX)

void ling_overlay_class_init(LingOverlayClass * class){

}

void ling_overlay_init(LingOverlay * self){
    self->overlay = gtk_overlay_new();
    gtk_widget_set_vexpand(self->overlay,TRUE);
    gtk_widget_set_hexpand(self->overlay,TRUE);
    gtk_box_append(GTK_BOX(self),self->overlay);
}

GtkWidget * ling_overlay_new(){
    return g_object_new(LING_TYPE_OVERLAY,NULL);
}


static LingLayer * get_layer(LingOverlay * overlay,uint lay){
    for(GList * l=overlay->layers;l!=NULL;l=l->next){
        LingLayer * layer = (LingLayer *)l->data;
        if(layer->level==lay)return layer;
    }
    return NULL;
}

static switcher * get_switcher(LingLayer * layer,uint op_type){
    for(GList * l=layer->switchers;l!=NULL;l=l->next){
        switcher * s = (switcher*)l->data;
        if(s->op_type==op_type)return s;
    }
    return NULL;
}

// static void layer_set_blur(LingLayer * layer,uint blur){
//     char str[100];
//     sprintf(str,"box { filter: blur(%dpx); }",blur);
//     GtkCssProvider * provider = gtk_css_provider_new();
//     gtk_css_provider_load_from_data(provider,
//                                     str, -1);
//     gtk_style_context_add_provider(gtk_widget_get_style_context(layer->widget),
//                                    GTK_STYLE_PROVIDER(provider),GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
// }

void drag_begin(GtkGestureDrag* self,
                   gdouble start_x,gdouble start_y,gpointer user_data){
}

static void fade_animate(LingLayer * main,LingLayer * sub,gdouble offset_x,gdouble offset_y){
    if(offset_y<-300){
        gtk_widget_set_visible(main->widget,FALSE);
        gtk_widget_set_visible(sub->widget,TRUE);
    }
    else if(offset_y<0){
        gtk_widget_set_margin_top(main->widget,offset_y*0.1);
        gtk_widget_set_margin_top(sub->widget,30+offset_y*0.1);
        gtk_widget_set_visible(main->widget,TRUE);
        gtk_widget_set_visible(sub->widget,TRUE);
        gtk_widget_set_opacity(main->widget,1-fabs(offset_y)/300);
        gtk_widget_set_opacity(sub->widget,fabs(offset_y)/300);
    }
    if(offset_y>300){
        gtk_widget_set_visible(main->widget,FALSE);
        gtk_widget_set_visible(sub->widget,TRUE);
    }
    else if(offset_y>0){
        gtk_widget_set_margin_top(main->widget,30+offset_y*0.1);
        gtk_widget_set_margin_top(sub->widget,-30+offset_y*0.1);
        gtk_widget_set_visible(main->widget,TRUE);
        gtk_widget_set_visible(sub->widget,TRUE);
        gtk_widget_set_opacity(main->widget,1-fabs(offset_y)/300);
        gtk_widget_set_opacity(sub->widget,fabs(offset_y)/300);
    }
}

void drag_update(GtkGestureDrag* self,
                    gdouble offset_x,gdouble offset_y,gpointer user_data){
    LingLayer * layer = (LingLayer*)user_data;
    if(offset_y<-10){
        switcher * s = get_switcher(layer,LING_OVERLAY_OP_SWIPE_UP);
        if(s&&s->ani_type==LING_OVERLAY_ANIMATE_FADE){
            fade_animate(layer,s->to,offset_x,offset_y);
        }
    }
    if(offset_y>10){
        switcher * s = get_switcher(layer,LING_OVERLAY_OP_SWIPE_DOWN);
        if(s&&s->ani_type==LING_OVERLAY_ANIMATE_FADE){
            fade_animate(layer,s->to,offset_x,offset_y);
        }
    }
}

void drag_end(GtkGestureDrag* self,
                 gdouble offset_x,gdouble offset_y,gpointer user_data){

}

int ling_overlay_add_layer(LingOverlay * self,GtkWidget * widget,uint8_t level){
    //从小到大排列，小在上，大在下
    LingLayer * layer_new = malloc(sizeof(LingLayer));
    layer_new->widget = widget;
    layer_new->level = level;
    layer_new->switchers=NULL;
    layer_new->drag = gtk_gesture_drag_new();
    gtk_widget_add_controller(GTK_WIDGET(layer_new->widget), GTK_EVENT_CONTROLLER(layer_new->drag));
    g_signal_connect(layer_new->drag, "drag-begin", G_CALLBACK(drag_begin), layer_new);
    g_signal_connect(layer_new->drag, "drag-update", G_CALLBACK(drag_update), layer_new);
    g_signal_connect(layer_new->drag, "drag-end", G_CALLBACK(drag_end), layer_new);


    int pos=0;
    for(GList * now = self->layers;now!=NULL;now = now->next){
        LingLayer * layer_now = now->data;
        if(layer_now->level==level){
            free(layer_new);
            return 0;
        }
        if(layer_now->level<level){
            self->layers = g_list_insert(self->layers,layer_new,pos);
            gtk_overlay_add_overlay(GTK_OVERLAY(self->overlay),layer_new->widget);
            gtk_widget_insert_before(layer_new->widget,self->overlay,layer_now->widget);
            return 1;
        }
        pos++;
    }
    self->layers = g_list_insert(self->layers,layer_new,pos);
    gtk_overlay_add_overlay(GTK_OVERLAY(self->overlay),layer_new->widget);
    self->layers = g_list_append(self->layers,layer_new);
    return 1;
}


//前主后从
void ling_overlay_add_switch(LingOverlay * overlay_m,uint lay_main,
                             uint op_type,uint ani_type,
                             LingOverlay * overlay_s,uint lay_sub){
    LingLayer * lay_m = get_layer(overlay_m,lay_main);
    LingLayer * lay_s  = get_layer(overlay_s,lay_sub);
    if(lay_m==NULL||lay_s==NULL)return;

    switcher * s;
    if((s=get_switcher(lay_m,op_type))!=NULL){
        //对已经有的进行修改
        s->op_type = op_type;
        s->ani_type =ani_type;
        s->to = lay_s;
        return;
    }

    s = malloc(sizeof(switcher));
    s->op_type = op_type;
    s->ani_type =ani_type;
    s->to = lay_s;
    lay_m->switchers = g_list_append(lay_m->switchers,s);
}

