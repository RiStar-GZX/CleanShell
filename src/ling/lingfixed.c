#include "lingfixed.h"


struct _LingFixed{
    GtkFixed parent;
    GList * items_list; //LingFixedItem
};

G_DEFINE_FINAL_TYPE(LingFixed,ling_fixed,GTK_TYPE_FIXED)

void ling_fixed_class_init(LingFixedClass * self){

}

void ling_fixed_init(LingFixed * self){
    self->items_list = NULL;
}

GtkWidget * ling_fixed_new(){
    return g_object_new(LING_TYPE_FIXED,NULL);
}

static LingFixedChild * ling_fixed_get_child(LingFixed * self,GtkWidget * widget){
    // GtkWidget * w=gtk_widget_get_last_child(GTK_WIDGET(self));
    // int i;
    // for(i=0;w!=NULL;w=gtk_widget_get_prev_sibling(w),i++){
    //     if(w==widget)break;
    // }
    LingFixedChild * child=NULL;
    for(GList * l=self->items_list;l!=NULL;l=l->next){
        LingFixedChild * c = l->data;
        if(c->widget==widget){
            child=c;
            break;
        }
    }
    return child;
}

void ling_fixed_put(LingFixed * self,GtkWidget * widget,uint x,uint y,int level){
    LingFixedChild * item= malloc(sizeof(LingFixedChild));
    item->widget = widget;
    if(level==LING_FIXED_TOP){
        gtk_fixed_put(GTK_FIXED(self),item->widget,x,y);
    }
    else if(level==LING_FIXED_BOTTOM){
        gtk_fixed_put(GTK_FIXED(self),item->widget,x,y);
        gtk_widget_insert_before(widget,GTK_WIDGET(self),gtk_widget_get_first_child(GTK_WIDGET(self)));
    }
    self->items_list = g_list_append(self->items_list,item);
}

void ling_fixed_move(LingFixed * self,GtkWidget * widget,gdouble x,gdouble y){
    LingFixedChild * item=ling_fixed_get_child(self,widget);
    if(item==NULL)return;
    item->x = x;
    item->y = y;
    gtk_fixed_move(GTK_FIXED(self),widget,x,y);
}

void ling_fixed_remove(LingFixed * self,GtkWidget * widget){
    LingFixedChild * item;
    for(GList * l=self->items_list;l!=NULL;l=l->next){
        item = l->data;
        if(item->widget==widget){
            g_object_ref(widget);
            gtk_fixed_remove(GTK_FIXED(self),widget);
            self->items_list = g_list_remove_link(self->items_list,l);
            free(item);
        }
    }
}

void ling_fixed_set_child_size(LingFixed * self,GtkWidget * widget,int w,int h){
    LingFixedChild * item=ling_fixed_get_child(self,widget);
    if(item==NULL)return;
    if(w<0)w=0;
    if(h<0)h=0;
    item->w = w;
    item->h = h;
    gtk_widget_set_size_request(item->widget,w,h);
}

LingFixedChild * ling_fixed_get_child_info(LingFixed * self,GtkWidget * widget){
    GtkWidget * w=gtk_widget_get_last_child(GTK_WIDGET(self));
    int i;
    for(i=0;w!=NULL;w=gtk_widget_get_prev_sibling(w),i++){
        if(w==widget)break;
    }
    LingFixedChild * child=NULL;
    for(GList * l=self->items_list;l!=NULL;l=l->next){
        LingFixedChild * c = l->data;
        if(c->widget==widget){
            c->level=i;
            gtk_fixed_get_child_position(GTK_FIXED(self),widget,&c->x,&c->y);
            child=c;
            break;
        }
    }
    return child;
}
