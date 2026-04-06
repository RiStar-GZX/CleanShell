#include "lingviewpager.h"
#include <lingdots.h>

struct _LingViewPager{
    GtkBox parent;
    gdouble offset;

    gboolean size_adapt;

    LingOperate * op;

    uint page_num;
    uint page_now_pos;
    gboolean page_cycle;

    GtkWidget * dots;
    gboolean dots_able;
};

G_DEFINE_FINAL_TYPE(LingViewPager,ling_view_pager,GTK_TYPE_BOX)

struct _LingViewPagerLayout{
    GtkLayoutManager manager;
};

G_DEFINE_FINAL_TYPE(LingViewPagerLayout,ling_view_pager_layout,GTK_TYPE_LAYOUT_MANAGER)

static void ling_view_pager_layout_allocate(GtkLayoutManager *manager,GtkWidget *widget,
                                        int width,int height,int baseline){
    GtkWidget * parent = gtk_widget_get_parent(widget);
    if(parent==NULL)return;

    if(width<=0){
        width = gtk_widget_get_width(gtk_widget_get_parent(widget));
    }
    if(height<=0){
        height = gtk_widget_get_height(gtk_widget_get_parent(widget));
    }

    graphene_rect_t rect;
    if(!gtk_widget_compute_bounds(parent,widget,&rect))return;
    int start = gtk_widget_get_margin_start(widget);
    int end = gtk_widget_get_margin_end(widget);
    int top = gtk_widget_get_margin_top(widget);
    int bottom = gtk_widget_get_margin_bottom(widget);
    rect.size.height-=top+bottom;
    rect.size.width-=start+end;
    LingViewPager * vp = LING_VIEW_PAGER(widget);
    GtkAllocation alloc;
    alloc.width = width;
    alloc.height = height;
    alloc.x=0;
    alloc.y=0;

    int i=1;
    GtkWidget * page_now = NULL;
    for(GtkWidget * w = gtk_widget_get_first_child(GTK_WIDGET(vp));w!=NULL;
         w=gtk_widget_get_next_sibling(w)){
        if(i==vp->page_now_pos)page_now = w;
        i++;
    }

    for(GtkWidget * w = page_now;w!=NULL;w=gtk_widget_get_prev_sibling(w)){
        if(w==page_now){
            alloc.x=vp->offset;
        }
        else alloc.x -= width;
        int ex = alloc.x+width;
        if((alloc.x<0||alloc.x>width)&&(ex<0||ex>width)){
            gtk_widget_set_visible(w,FALSE);
        }
        gtk_widget_set_visible(w,TRUE);
        gtk_widget_size_allocate(w, &alloc, baseline);
    }

    for(GtkWidget * w = page_now;w!=NULL;w=gtk_widget_get_next_sibling(w)){
        if(w==page_now){
            alloc.x=vp->offset;
            continue;
        }else alloc.x += width;
        int ex = alloc.x+width;
        if((alloc.x<0||alloc.x>width)&&(ex<0||ex>width)){
            gtk_widget_set_visible(w,FALSE);
        }
        gtk_widget_set_visible(w,TRUE);
        gtk_widget_size_allocate(w, &alloc, baseline);
    }
}

static void ling_view_pager_layout_class_init(LingViewPagerLayoutClass * klass){
    GtkLayoutManagerClass * manager_class = GTK_LAYOUT_MANAGER_CLASS(klass);
    manager_class->allocate = ling_view_pager_layout_allocate;
    //manager_class->measure = ling_view_pager_measure;
}

static void ling_view_pager_layout_init(LingViewPagerLayout * self){

}

GtkLayoutManager * ling_view_pager_layout_new(){
    return g_object_new(LING_TYPE_VIEW_PAGER_LAYOUT,NULL);
}

static void ling_view_pager_acllocate(LingViewPager * vp){
    GtkLayoutManager * layout = gtk_widget_get_layout_manager(GTK_WIDGET(vp));
    if(layout==NULL)return;
    gtk_layout_manager_allocate(layout,GTK_WIDGET(vp),
                                gtk_widget_get_width(GTK_WIDGET(vp)),
                                gtk_widget_get_height(GTK_WIDGET(vp)),
                                gtk_widget_get_baseline(GTK_WIDGET(vp)));
}

//-------------------------------------LingViewPage------------------------------------------//

enum ling_view_pager_animate_mode{
    LING_VIEW_PAGER_ANIMATE_MODE_LEFT,
    LING_VIEW_PAGER_ANIMATE_MODE_RIGHT,
    LING_VIEW_PAGER_ANIMATE_MODE_CENTER,
};

void ling_view_pager_set_offset(LingViewPager * self,gdouble offset){

}

static void ling_view_pager_class_init(LingViewPagerClass * klass){

}

static void ling_view_pager_init(LingViewPager * self){
    GtkLayoutManager * manager = ling_view_pager_layout_new();
    gtk_widget_set_layout_manager(GTK_WIDGET(self),manager);

    ling_view_pager_set_page_cycle(self,TRUE);
    //点指示器
    self->dots = ling_dots_new(0,DOTS_MODE_SINGAL);
    ling_view_pager_set_dot_indicator(self,TRUE);
    //gtk_box_append(GTK_BOX(self),self->dots);

    gtk_widget_set_halign(self->dots,GTK_ALIGN_CENTER);
    gtk_widget_set_valign(self->dots,GTK_ALIGN_END);

    self->page_now_pos = 1;
}

/*--------------------------------------动画部分-----------------------------------------------------------------------------------------*/
void ling_view_pager_finish_start(GtkWidget * widget,LingActionArgs args,gpointer data){
    LingViewPager * self = LING_VIEW_PAGER(data);
    if(args.progress>0)
        ling_view_pager_show_page(self,self->page_now_pos-1);
    else if(args.progress<0)
        ling_view_pager_show_page(self,self->page_now_pos+1);
    else ling_view_pager_show_page(self,self->page_now_pos);
}

void ling_view_pager_finish_end(GtkWidget * widget,LingActionArgs args,gpointer data){
    LingViewPager * self = LING_VIEW_PAGER(data);
    if(args.progress>0)
        ling_view_pager_show_page(self,self->page_now_pos-1);
    else if(args.progress<0)
        ling_view_pager_show_page(self,self->page_now_pos+1);
    else ling_view_pager_show_page(self,self->page_now_pos);

}

gdouble ling_view_pager_progress(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    gdouble p= args.offset_x;

    int w = gtk_widget_get_width(widget);
    int limit = w*0.1;
    int next = w*0.2;

    LingViewPager * self = LING_VIEW_PAGER(widget);
    if(self->page_now_pos==1&&p>limit){
        p=limit;
    }
    if(self->page_now_pos==self->page_num&&p<-limit){
        p=-limit;
    }
    ling_operate_set_ani_progress_lenth(args.op,LING_ACTION_DRAG_HORIZONTAL,w);
    if(p>next){
        ling_operate_set_ani_progress_end(args.op,LING_ACTION_DRAG_HORIZONTAL,w);
    }
    else if(p<-next){
        ling_operate_set_ani_progress_end(args.op,LING_ACTION_DRAG_HORIZONTAL,-w);
    }
    else
    {
        ling_operate_set_ani_progress_end(args.op,LING_ACTION_DRAG_HORIZONTAL,0);
    }
    return p;
}

void ling_view_pager_ani(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    LingViewPager * self = LING_VIEW_PAGER(widget);
    self->offset = args.progress;
    ling_view_pager_acllocate(self);
}

/*-----------------------------------------------------------------------------------------------------------------------------------*/

GtkWidget * ling_view_pager_new_with_op(gboolean size_adapt,const char * op_name){
    LingViewPager * self = LING_VIEW_PAGER(g_object_new(LING_TYPE_VIEW_PAGER,NULL));

    self->op = ling_operate_add(op_name,GTK_WIDGET(self));
    ling_operate_add_action(self->op,LING_ACTION_DRAG_HORIZONTAL,
                            ling_view_pager_progress,NULL,
                            ling_view_pager_ani,NULL,
                            NULL,NULL,
                            ling_view_pager_finish_start,ling_view_pager_finish_end,self);
    self->size_adapt = size_adapt;

    return GTK_WIDGET(self);
}

GtkWidget * ling_view_pager_new(gboolean size_adapt){
    LingViewPager * self =LING_VIEW_PAGER(g_object_new(LING_TYPE_VIEW_PAGER,NULL));
    self->size_adapt = size_adapt;
    return GTK_WIDGET(self);
}

void ling_view_pager_add_page(LingViewPager * self,GtkWidget * page){
    if(self==NULL||page==NULL)return;
    gtk_widget_set_parent(page,GTK_WIDGET(self));
    self->page_num++;
}

void ling_view_pager_remove_page(LingViewPager * self,GtkWidget * page){
    if(self==NULL||page==NULL)return;
    for(GtkWidget * w = gtk_widget_get_first_child(GTK_WIDGET(self));w!=NULL;
         w=gtk_widget_get_next_sibling(w)){
        if(page==w){

            if(self->page_now_pos-1>=1){
                self->page_now_pos--;
            }
            else if(self->page_now_pos+1<=self->page_num-1){
                self->page_now_pos++;
            }else{
                self->page_now_pos=0;
            }
            gtk_widget_unparent(w);
            self->page_num--;
            return;
        }
    }
}

void ling_view_pager_set_dot_indicator(LingViewPager * self,gboolean able){
    self->dots_able=able;
    gtk_widget_set_visible(self->dots,able);
}

GtkWidget * ling_view_pager_get_page_by_pos(LingViewPager * self,uint pos){
    if(pos>self->page_num||pos<=0)return NULL;
    int i=1;
    for(GtkWidget * w=gtk_widget_get_first_child(GTK_WIDGET(self));w!=NULL;
         w=gtk_widget_get_next_sibling(w)){
        if(pos==i)return w;
        i++;
    }
    return NULL;
}

uint ling_view_pager_get_page_pos(LingViewPager * self,GtkWidget * page){
    if(self==NULL||page==NULL)return 0;
    int i=1;
    for(GtkWidget * w=gtk_widget_get_first_child(GTK_WIDGET(self));w!=NULL;
         w=gtk_widget_get_next_sibling(w)){
        if(page==w)return i;
        i++;
    }
    return 0;
}

GtkWidget * ling_view_pager_get_page_now(LingViewPager * self){
    return ling_view_pager_get_page_by_pos(self,self->page_now_pos);
}

uint ling_view_pager_get_page_now_pos(LingViewPager * self){
    return self->page_now_pos;
}

uint ling_view_pager_get_page_num(LingViewPager * self){
    return self->page_num;
}

gboolean ling_view_pager_get_page_cycle(LingViewPager * self){
    return self->page_cycle;
}

int ling_view_pager_get_pos(LingViewPager * self){
    return self->page_now_pos;
}

void ling_view_pager_show_page(LingViewPager * self,uint pos){
    if(pos>self->page_num||pos<1)return;
    self->page_now_pos = pos;
    self->offset = 0;
    ling_view_pager_acllocate(self);
}

void ling_view_pager_set_page_cycle(LingViewPager * self,gboolean able){
    self->page_cycle = able;
}
