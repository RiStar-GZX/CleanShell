#include "lingoverlay.h"


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


LingLayer * ling_overlay_get_layer(LingOverlay * overlay,uint lay){
    for(GList * l=overlay->layers;l!=NULL;l=l->next){
        LingLayer * layer = (LingLayer *)l->data;
        if(layer->level==lay)return layer;
    }
    return NULL;
}


int ling_overlay_add_layer(LingOverlay * self,GtkWidget * widget,uint8_t level){
    //从小到大排列，小在上，大在下
    LingLayer * layer_new = malloc(sizeof(LingLayer));
    layer_new->widget = widget;
    layer_new->level = level;

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
            //gtk_overlay_set_clip_overlay(GTK_OVERLAY(self->overlay),layer_new->widget,TRUE);
            gtk_widget_insert_before(layer_new->widget,self->overlay,layer_now->widget);
            return 1;
        }
        pos++;
    }
    self->layers = g_list_insert(self->layers,layer_new,pos);

    gtk_overlay_add_overlay(GTK_OVERLAY(self->overlay),layer_new->widget);
    //gtk_overlay_set_clip_overlay(GTK_OVERLAY(self->overlay),layer_new->widget,TRUE);
    self->layers = g_list_append(self->layers,layer_new);
    return 1;
}


gdouble ling_layer_progress(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    //y正方向起点为100，终点为0
    gdouble p;
    gdouble y = args.offset_y;
    if(y<0){
        if(y<-300)y=-300;
        p=-(y/300)*100;
    }
    else{
        if(y>300)y=300;
        p=100-(y/300)*100;
    }
    //g_print("p:%f\n",p);
    return p;
}

gboolean ling_layer_release(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    gdouble t=fabs(args.offset_y)+fabs(args.velocity_y);
    if(t<20){
        if(args.action==LING_ACTION_DRAG_DOWN)return LING_OPERATE_ANIMATION_DIR_FORWARD;
        else return LING_OPERATE_ANIMATION_DIR_BACK;
    }
    else{
        if(args.action==LING_ACTION_DRAG_DOWN)return LING_OPERATE_ANIMATION_DIR_BACK;
        else return LING_OPERATE_ANIMATION_DIR_FORWARD;
    }
}


// void ling_layer_add_switch(LingOperate * op_m,LingOverlay * overlay_m,uint lay_main,
//                            LingOperate * op_s,LingOverlay * overlay_s,uint lay_sub,
//                            uint m_op_type,
//                            ANIMATION ani,PROGRESS progress,RELEASE release,FINISH main_f,FINISH sub_f){
//     switcher * s = malloc(sizeof(switcher));
//     LingLayer * main=ling_overlay_get_layer(overlay_m,lay_main);
//     LingLayer * sub=ling_overlay_get_layer(overlay_s,lay_sub);
//     s->main = main;
//     s->sub = sub;

//     ling_operate_add_action(op_m,m_op_type,
//                             progress,NULL,       //0->100
//                             ani,s,
//                             release,NULL,
//                             main_f,sub_f,s);

//     uint s_op_type=LING_ACTION_CLICK;
//     if(m_op_type==LING_ACTION_DRAG_UP)s_op_type = LING_ACTION_DRAG_DOWN;
//     if(m_op_type==LING_ACTION_DRAG_DOWN)s_op_type = LING_ACTION_DRAG_UP;
//     ling_operate_add_action(op_s,s_op_type,
//                             progress,NULL,       //100->0
//                             ani,s,
//                             release,NULL,
//                             main_f,sub_f,s);
// }

void ling_layer_main_finish(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    switcher * s = user_data;
    gtk_widget_set_visible(s->main->widget,TRUE);
    gtk_widget_set_visible(s->sub->widget,FALSE);
    gtk_widget_set_margin_top(s->main->widget,0);
    gtk_widget_set_margin_bottom(s->main->widget,0);
    gtk_widget_set_margin_start(s->main->widget,0);
    gtk_widget_set_margin_end(s->main->widget,0);
    gtk_widget_set_margin_top(s->sub->widget,0);
    gtk_widget_set_opacity(s->main->widget,1);
    gtk_widget_set_opacity(s->sub->widget,1);
}

void ling_layer_sub_finish(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    switcher * s = user_data;
    gtk_widget_set_visible(s->main->widget,FALSE);
    gtk_widget_set_visible(s->sub->widget,TRUE);
    gtk_widget_set_margin_top(s->main->widget,0);
    gtk_widget_set_margin_top(s->sub->widget,0);
    gtk_widget_set_opacity(s->main->widget,1);
    gtk_widget_set_opacity(s->sub->widget,1);
}
