#include "lingfixed.h"

struct _LingFixedLayout{
    GtkLayoutManager parent;
};

G_DEFINE_FINAL_TYPE(LingFixedLayout,ling_fixed_layout,GTK_TYPE_LAYOUT_MANAGER)

static void ling_fixed_layout_allocate(GtkLayoutManager *manager,GtkWidget *widget,
                int width,int height,int baseline){
    GtkWidget * parent = gtk_widget_get_parent(widget);
    if(parent==NULL)return;

    graphene_rect_t rect;
    if(!gtk_widget_compute_bounds(parent,widget,&rect))return;
    LingFixed * fixed = LING_FIXED(widget);
    for(GList * l=fixed->items_list;l!=NULL;l=l->next){
        LingFixedItemInfo * item = l->data;
        GtkAllocation child_allocation;
        if(item->adjust!=NULL){
            child_allocation = item->adjust(widget,item,width,height,baseline,rect);
            if(child_allocation.width<=0)child_allocation.width=width;
            if(child_allocation.height<=0)child_allocation.height=height;

            gtk_widget_size_allocate(item->widget, &child_allocation, baseline);
        }
    }
}

static void ling_fixed_layout_measure(GtkLayoutManager *manager,GtkWidget *widget,
                GtkOrientation orientation,int for_size,
                int *minimum,int *natural,
                int *minimum_baseline,int *natural_baseline){
    //GtkAllocation alloc;
    //gtk_widget_get_allocation(widget, &alloc);
    //设置子部件尺寸为容器尺寸
    // *minimum = (orientation == GTK_ORIENTATION_HORIZONTAL) ? alloc.width : alloc.height;
    // *natural = *minimum;

    // LingFixedLayout *self = LING_FIXED_LAYOUT(manager);
    // gint min = 0, nat = 0;

    // // 遍历所有子组件计算尺寸
    // GtkWidget *child;
    //GtkRequisition child_min, child_nat;
    // GList *children = gtk_widget_get_children(container);
    // for (GList *l = children; l; l = l->next) {
    //     child = l->data;
    //     if (!gtk_widget_get_visible(child)) continue;
    //gtk_widget_get_preferred_size(widget, &child_min, &child_nat);
    //     min += child_min.width + self->spacing;
    //     nat += child_nat.width + self->spacing;
    // }
    // if (min > 0) min -= self->spacing; // 移除最后一次的间距
    // if (nat > 0) nat -= self->spacing;
    //*minimum = (orientation == GTK_ORIENTATION_HORIZONTAL) ? child_min.width : child_min.height;
    //*natural = (orientation == GTK_ORIENTATION_HORIZONTAL) ? child_nat.width : child_nat.height;
    *minimum = 1000;
    *natural = 1000;
    *minimum_baseline = GTK_BASELINE_POSITION_CENTER;
    *natural_baseline = GTK_BASELINE_POSITION_CENTER;
}

void ling_fixed_layout_class_init(LingFixedLayoutClass * klass){
    GtkLayoutManagerClass * manager_class = GTK_LAYOUT_MANAGER_CLASS(klass);
    manager_class->allocate = ling_fixed_layout_allocate;
    manager_class->measure = ling_fixed_layout_measure;
}

void ling_fixed_layout_init(LingFixedLayout * self){
}

GtkLayoutManager * ling_fixed_layout_new(){
    return g_object_new(LING_TYPE_FIXED_LAYOUT,NULL);
}

/***************************fixed****************************************/

G_DEFINE_FINAL_TYPE(LingFixed,ling_fixed,GTK_TYPE_FIXED)

void ling_fixed_class_init(LingFixedClass * self){
}

void ling_fixed_init(LingFixed * self){
    self->items_list = NULL;
    self->layout_manager = ling_fixed_layout_new();
    gtk_widget_set_layout_manager(GTK_WIDGET(self),self->layout_manager);
}

GtkWidget * ling_fixed_new(){
    return g_object_new(LING_TYPE_FIXED,NULL);
}

LingFixedItemInfo * ling_fixed_get_item_info(LingFixed * self,GtkWidget * widget){
    for(GList * l=self->items_list;l!=NULL;l=l->next){
        LingFixedItemInfo * item = l->data;
        if(item->widget==widget){
            return item;
        }
    }
    return NULL;
}

void ling_fixed_set_child_size(LingFixed * self,GtkWidget * widget,int w,int h){
    LingFixedItemInfo * item=ling_fixed_get_item_info(self,widget);
    if(item==NULL)return;
    if(w<0)w=0;
    if(h<0)h=0;
    item->w = w;
    item->h = h;
    // if(gtk_widget_get_realized(widget)){
    //     GtkWidget * parent = gtk_widget_get_parent(item->widget);
    //     if(parent==NULL)return;
    //     GtkLayoutManager * layout = gtk_widget_get_layout_manager(parent);
    //     if(layout==NULL)return;
    //     gtk_layout_manager_allocate(layout,widget,w,h,-1);
    // }
    gtk_widget_set_size_request(widget,w,h);
}

// void ling_fixed_act(LingFixed * self){
//     self
// }

void ling_fixed_move(LingFixed * self,GtkWidget * widget,gdouble x,gdouble y){
    LingFixedItemInfo * item=ling_fixed_get_item_info(self,widget);
    if(item==NULL)return;
    item->x = x;
    item->y = y;
    //gtk_widget_set_size_request(widget,item->w,item->h);
    if(gtk_widget_get_realized(widget)){
        GtkWidget * parent = gtk_widget_get_parent(item->widget);
        if(parent==NULL)return;
        GtkLayoutManager * layout = gtk_widget_get_layout_manager(GTK_WIDGET(self));
        if(layout==NULL)return;
        gtk_layout_manager_allocate(layout,parent,item->w,item->h,-1);
    }
    //gtk_fixed_move(GTK_FIXED(self),widget,x,y);
}

void ling_fixed_remove(LingFixed * self,GtkWidget * widget){
    LingFixedItemInfo * item;
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

void ling_fixed_set_child_level(LingFixed * fixed,GtkWidget * widget,int level1,int level2){
    LingFixedItemInfo * item = ling_fixed_get_item_info(fixed,widget);
    if(item==NULL)return;
    if(level1>=0)item->level1 = level1;
    if(level2>=0)item->level2 = level2;
    for(GList * l = fixed->items_list;l!=NULL;l=l->next){
        LingFixedItemInfo * item_now = (LingFixedItemInfo *)l->data;
        if(item_now->level1>level1);
        else if(item_now->level1==level1){
            if(item_now->level2>level2);
            else{
                gtk_widget_insert_before(widget,GTK_WIDGET(fixed),item_now->widget);
                fixed->items_list = g_list_insert_before(fixed->items_list,l,item);
                return;
            }
        }
        else{
            gtk_widget_insert_before(widget,GTK_WIDGET(fixed),item_now->widget);
            fixed->items_list = g_list_insert_before(fixed->items_list,l,item);
            return;
        }
    }
}



void ling_fixed_put(LingFixed * fixed,GtkWidget * widget,LING_FIXED_ADJUST adjust,uint x,uint y,uint level1,uint level2){
    LingFixedItemInfo * item=ling_fixed_get_item_info(fixed,widget);
    //没有child创建
    if(item==NULL){
        item = g_malloc0(sizeof(LingFixedItemInfo));
        item->widget = widget;
        item->level1 = level1;
        item->level2 = level2;
        item->w = -1;  //未分配长度
        item->h = -1;
        item->x = x;
        item->y = y;
        item->adjust = adjust;

        gtk_widget_set_parent(widget,GTK_WIDGET(fixed));
        //gtk_widget_set_layout_manager(item->widget,GTK_LAYOUT_MANAGER(item));
    }

    //添加
    for(GList * l = fixed->items_list;l!=NULL;l=l->next){
        LingFixedItemInfo * item_now = (LingFixedItemInfo *)l->data;
        if(item_now->level1>level1);
        else if(item_now->level1==level1){
            if(item_now->level2>level2);
            else{
                gtk_widget_insert_before(widget,GTK_WIDGET(fixed),item_now->widget);
                fixed->items_list = g_list_insert_before(fixed->items_list,l,item);
                return;
            }
        }
        else{
            gtk_widget_insert_before(widget,GTK_WIDGET(fixed),item_now->widget);
            fixed->items_list = g_list_insert_before(fixed->items_list,l,item);
            return;
        }
    }
    gtk_widget_insert_after(widget,GTK_WIDGET(fixed),gtk_widget_get_last_child(GTK_WIDGET(fixed)));
    fixed->items_list = g_list_append(fixed->items_list,item);
}

GtkAllocation ling_fixed_adjust_none(GtkWidget * fixed,LingFixedItemInfo *item,
                                     int width,int height,int baseline,graphene_rect_t rect){
    GtkAllocation alloc;
    alloc.width=item->w;
    alloc.height=item->h;
    alloc.x=item->x;
    alloc.y=item->y;
    return alloc;
}

void ling_fixed_put_none(LingFixed * fixed,GtkWidget * widget,uint x,uint y,uint level1,uint level2){
    ling_fixed_put(fixed,widget,ling_fixed_adjust_none,x,y,level1,level2);
}

GtkAllocation ling_fixed_adjust_full(GtkWidget * fixed,LingFixedItemInfo *item,
                                     int width,int height,int baseline,graphene_rect_t rect){
    GtkAllocation alloc;
    alloc.width=rect.size.width;
    alloc.height=rect.size.height;
    alloc.x=item->x;
    alloc.y=item->y;
    return alloc;
}

void ling_fixed_put_full(LingFixed * fixed,GtkWidget * widget,uint x,uint y,uint level1,uint level2){
    ling_fixed_put(fixed,widget,ling_fixed_adjust_full,x,y,level1,level2);
}
