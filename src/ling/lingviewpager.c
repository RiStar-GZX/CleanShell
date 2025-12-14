#include "lingviewpager.h"
#include <lingdots.h>
//-------------------------------------LingViewPage------------------------------------------//
struct _LingViewPager{
    GtkBox parent;

    GtkWidget * overlay;
    GtkWidget * fixed;

    LingOperate * op;

    GList * pages;  //Gtkwidget
    uint page_num;
    uint page_now_pos;
    gboolean page_cycle;

    GtkWidget * dots;
    gboolean dots_able;

    //拖拽
    gdouble start_x;
    gdouble start_y;
    int ison;

    gdouble offset_x;
};

G_DEFINE_FINAL_TYPE(LingViewPager,ling_view_pager,GTK_TYPE_BOX)

enum ling_view_pager_animate_mode{
    LING_VIEW_PAGER_ANIMATE_MODE_LEFT,
    LING_VIEW_PAGER_ANIMATE_MODE_RIGHT,
    LING_VIEW_PAGER_ANIMATE_MODE_CENTER,
};

gboolean gtk_widget_have_child(GtkWidget * parent,GtkWidget * child){
    for(GtkWidget * c=gtk_widget_get_first_child(parent);c!=NULL;c=gtk_widget_get_next_sibling(c)){
        if(c==child)return TRUE;
    }
    return FALSE;
}

static gboolean ling_view_pager_animate(gpointer data,uint mode){
    LingViewPager * self = LING_VIEW_PAGER(data);
    GtkWidget * old = ling_view_pager_get_page_now(self);
    double * old_x,old_y;
    int x= self->offset_x;
    int w = gtk_widget_get_width(old);

    if(mode==LING_VIEW_PAGER_ANIMATE_MODE_LEFT){
        x-=(w-abs(x))/10+3;
        ling_view_pager_page_move(self,x);
        if(x<=-gtk_widget_get_width(old)){
            return LING_OPERATE_ANIMATION_REMOVE;
        }
    }
    if(mode==LING_VIEW_PAGER_ANIMATE_MODE_RIGHT){
        x+=(w-abs(x))/10+3;
        ling_view_pager_page_move(self,x);
        if(x>=gtk_widget_get_width(old)){
            return LING_OPERATE_ANIMATION_REMOVE;
        }
        return LING_OPERATE_ANIMATION_CONTINUE;
    }
    if(mode==LING_VIEW_PAGER_ANIMATE_MODE_CENTER){
        if(x<0){
            x+=abs(x)/20+2;
            if(x>=0){
                ling_view_pager_page_move(self,0);
                return LING_OPERATE_ANIMATION_REMOVE;
            }
            ling_view_pager_page_move(self,x);
        }
        if(x>=0){
            x-=abs(x)/20+2;
            if(x<=0){
                ling_view_pager_page_move(self,0);
                return LING_OPERATE_ANIMATION_REMOVE;
            }
            ling_view_pager_page_move(self,x);
        }
    }

    return LING_OPERATE_ANIMATION_CONTINUE;
}

static gboolean ling_view_pager_animate_left(gdouble velocity_x,gdouble velocity_y,gpointer data){
    return ling_view_pager_animate(data,LING_VIEW_PAGER_ANIMATE_MODE_LEFT);
}

static gboolean ling_view_pager_animate_right(gdouble velocity_x,gdouble velocity_y,gpointer data){
    return ling_view_pager_animate(data,LING_VIEW_PAGER_ANIMATE_MODE_RIGHT);
}

static gboolean ling_view_pager_animate_center(gdouble velocity_x,gdouble velocity_y,gpointer data){
    return ling_view_pager_animate(data,LING_VIEW_PAGER_ANIMATE_MODE_CENTER);
}

static void ling_view_pager_operate_finish_left(gpointer data){
    LingViewPager * self = LING_VIEW_PAGER(data);
    ling_view_pager_next(self,FALSE);
}

static void ling_view_pager_operate_finish_right(gpointer data){
    LingViewPager * self = LING_VIEW_PAGER(data);
    ling_view_pager_prev(self,FALSE);
}

static void ling_view_pager_operate_finish_center(gpointer data){
    LingViewPager * self = LING_VIEW_PAGER(data);
    ling_view_pager_show_page(self,self->page_now_pos);
}

static void on_drag_begin (GtkGestureDrag* gedture,gdouble start_x,
                          gdouble start_y,LingViewPager * self){
    self->start_x = start_x;
    self->start_y = start_y;
    self->ison = 0;
}

static void on_drag_update(GtkGestureDrag *gesture,
                           gdouble offset_x,gdouble offset_y,LingViewPager * self) {
    if(self->ison!=0);
    else if((fabs(offset_x)>=fabs(offset_y))){
        self->ison=1;
    }

    if(self->ison==1){
        LingOperate * op = ling_operate_get(shell->controler,"view_pager");
        if(ling_operate_start_operating(op)){
            ling_view_pager_page_move(self,offset_x);
            if(offset_x<-100){
                if((!self->page_cycle||self->page_num==1)&&self->page_now_pos==self->page_num){
                    ling_operate_set_animation_cb(op,ling_view_pager_animate_center,self);
                    ling_operate_set_finish_cb(op,ling_view_pager_operate_finish_center,self);
                }
                else{
                    ling_operate_set_animation_cb(op,ling_view_pager_animate_left,self);
                    ling_operate_set_finish_cb(op,ling_view_pager_operate_finish_left,self);
                }
            }
            else if(offset_x>100){
                if((!self->page_cycle||self->page_num==1)&&self->page_now_pos==1){
                    ling_operate_set_animation_cb(op,ling_view_pager_animate_center,self);
                    ling_operate_set_finish_cb(op,ling_view_pager_operate_finish_center,self);
                }
                else{
                    ling_operate_set_animation_cb(op,ling_view_pager_animate_right,self);
                    ling_operate_set_finish_cb(op,ling_view_pager_operate_finish_right,self);
                }
            }
            else{
                ling_operate_set_animation_cb(op,ling_view_pager_animate_center,self);
                ling_operate_set_finish_cb(op,ling_view_pager_operate_finish_center,self);
            }
        }
    }
}

static void on_drag_end(GtkGestureDrag *gesture,
                        gdouble offset_x,gdouble offset_y,LingViewPager * self) {
    ling_operate_run_animation(ling_operate_get(shell->controler,"view_pager"));
    int i=1;
}

static void ling_view_pager_class_init(LingViewPagerClass * klass){

}

static void ling_view_pager_init(LingViewPager * self){
    gtk_orientable_set_orientation(GTK_ORIENTABLE(self),GTK_ORIENTATION_VERTICAL);
    self->overlay = gtk_overlay_new();
    self->fixed = gtk_fixed_new();
    gtk_widget_set_hexpand(self->overlay, TRUE);
    gtk_widget_set_vexpand(self->overlay, TRUE);
    gtk_widget_set_hexpand(self->fixed, TRUE);
    gtk_widget_set_vexpand(self->fixed, TRUE);
    gtk_widget_set_halign(self->fixed,GTK_ALIGN_START);
    gtk_widget_set_valign(self->fixed,GTK_ALIGN_START);

    gtk_overlay_add_overlay(GTK_OVERLAY(self->overlay),self->fixed);

    gtk_box_append(GTK_BOX(self),self->overlay);


    ling_view_pager_set_page_cycle(self,TRUE);
    //点指示器
    self->dots = ling_dots_new(0,DOTS_MODE_SINGAL);
    ling_view_pager_set_dot_indicator(self,TRUE); //@@
    gtk_box_append(GTK_BOX(self),self->dots);

    gtk_widget_set_halign(self->dots,GTK_ALIGN_CENTER);
    gtk_widget_set_valign(self->dots,GTK_ALIGN_END);
}

GtkWidget * ling_view_pager_new(){
    LingViewPager * self = LING_VIEW_PAGER(g_object_new(LING_TYPE_VIEW_PAGER,NULL));
    //gtk_widget_set_size_request(GTK_WIDGET(self->fixed), 500, 850);
    self->op = ling_operate_add(shell->controler,"view_pager",NULL,self,NULL,self,NULL,self);

    GtkGesture * drag = gtk_gesture_drag_new();
    gtk_widget_add_controller(GTK_WIDGET(self), GTK_EVENT_CONTROLLER(drag));
    g_signal_connect(drag, "drag-begin", G_CALLBACK(on_drag_begin), self);
    g_signal_connect(drag, "drag-update", G_CALLBACK(on_drag_update), self);
    g_signal_connect(drag, "drag-end", G_CALLBACK(on_drag_end), self);

    return GTK_WIDGET(self);
}

static void realize (GtkWidget* page,LingViewPager * self){
    gtk_widget_set_size_request(page,gtk_widget_get_width(GTK_WIDGET(self)),gtk_widget_get_height(GTK_WIDGET(self)));
}
static gboolean resize_timeout(gpointer data){
    GtkWidget * widget = data;
    GtkWidget * parent = gtk_widget_get_parent(widget);
    gtk_widget_set_size_request(widget,gtk_widget_get_width(parent),gtk_widget_get_height(parent));
    return G_SOURCE_REMOVE;
}

void ling_view_pager_add_page(LingViewPager * self,GtkWidget * page){
    //gtk_widget_set_size_request(GTK_WIDGET(page), 500, 900);
    self->pages = g_list_append(self->pages,page);
    self->page_num++;
    ling_dots_set_num(LING_DOTS(self->dots),self->page_num);
    if(self->page_now_pos==0){
        ling_view_pager_show_page(self,1);
    }
    g_signal_connect(page, "realize", G_CALLBACK(realize), self);
    //g_timeout_add(100,resize_timeout,page);
}

void ling_view_pager_add_page_before(LingViewPager * self,GtkWidget * page,GtkWidget * before){

}

void ling_view_pager_add_page_after(LingViewPager * self,GtkWidget * page,GtkWidget * after){

}

void ling_view_pager_remove_page(LingViewPager * self,GtkWidget * page){
    if(self->page_num==0){
        return;
    }
    GtkWidget * page_now = ling_view_pager_get_page_now(self);
    if(page_now==page){
        gtk_fixed_remove(GTK_FIXED(self->fixed),page);
        self->pages = g_list_remove(self->pages,page);
        self->page_num--;
        ling_dots_set_num(LING_DOTS(self->dots),self->page_num);
        if(self->page_num!=0){
            ling_view_pager_show_page(self,1);
            ling_dots_ativate(LING_DOTS(self->dots),1);
        }
    }
    else{
        gtk_fixed_remove(GTK_FIXED(self->fixed),page);
        self->pages = g_list_remove(self->pages,page);
        self->page_num--;
        GList * l;
        int pos=1;
        for(l=self->pages;l->data!=page_now;l=l->next){
            if(l==NULL)return;
            pos++;
        }
        ling_dots_set_num(LING_DOTS(self->dots),self->page_num);
        ling_dots_ativate(LING_DOTS(self->dots),pos);
    }
}


void ling_view_pager_set_dot_indicator(LingViewPager * self,gboolean able){
    self->dots_able=able;
    gtk_widget_set_visible(self->dots,able);
}

GtkWidget * ling_view_pager_get_page_by_pos(LingViewPager * self,uint pos){
    if(pos>self->page_num||pos<=0)return NULL;
    GList * l=self->pages;
    for(int i=1;i<pos;i++){
        l=l->next;
    }
    GtkWidget * page = GTK_WIDGET(l->data);
    return page;
}

GtkWidget * ling_view_pager_get_page_now(LingViewPager * self){
    return ling_view_pager_get_page_by_pos(self,self->page_now_pos);
}

uint ling_view_pager_get_page_num(LingViewPager * self){
    return self->page_num;
}

int ling_view_pager_get_pos(LingViewPager * self){
    return self->page_now_pos;
}

void ling_view_pager_show_page(LingViewPager * self,uint pos){
    //去除旧的页面
    GtkWidget * old_page = ling_view_pager_get_page_by_pos(self,self->page_now_pos);
    if(old_page!=NULL){
        g_object_ref(old_page);
        gtk_fixed_remove(GTK_FIXED(self->fixed),old_page);
    }
    //换上新的页面
    GtkWidget * new_page = ling_view_pager_get_page_by_pos(self,pos);
    if(gtk_widget_have_child(self->fixed,new_page)){
        g_object_ref(new_page);
        gtk_fixed_remove(GTK_FIXED(self->fixed),new_page);
    }
    //重要
    //gtk_widget_set_size_request(new_page,gtk_widget_get_width(GTK_WIDGET(self)),gtk_widget_get_height(GTK_WIDGET(self)));
    gtk_fixed_put(GTK_FIXED(self->fixed),new_page,0,0);
    self->page_now_pos = pos;
    ling_dots_ativate(LING_DOTS(self->dots),pos);
}

void ling_view_pager_set_page_cycle(LingViewPager * self,gboolean able){
    self->page_cycle = able;
}

gboolean ling_view_pager_next(LingViewPager * self,gboolean animate){
    if(self->page_num==0)return FALSE;
    //LingOperate * op = ling_operate_get(self->controler,"view_pager");
    if(self->page_now_pos==self->page_num){
        if(self->page_cycle&&self->page_num>1){
            if(!animate)ling_view_pager_show_page(self,1);
            else{

                g_print("animate\n");
                ling_operate_set_animation_cb(self->op,ling_view_pager_animate_left,self);
                ling_operate_set_finish_cb(self->op,ling_view_pager_operate_finish_left,self);
                ling_operate_start_operating(self->op);
                ling_operate_run_animation(self->op);
            }
        }
        else return FALSE;
    }
    else {
        if(!animate)ling_view_pager_show_page(self,self->page_now_pos+1);
        else{
            g_print("animate\n");
            ling_operate_set_animation_cb(self->op,ling_view_pager_animate_left,self);
            ling_operate_set_finish_cb(self->op,ling_view_pager_operate_finish_left,self);
            ling_operate_start_operating(self->op);
            ling_operate_run_animation(self->op);
        }
    }
    return TRUE;
}

gboolean ling_view_pager_prev(LingViewPager * self,gboolean animate){
    if(self->page_num==0)return FALSE;
    //LingOperate * op = ling_operate_get(self->controler,"view_pager");
    if(self->page_now_pos==1){
        if(self->page_cycle&&self->page_num>1){
            if(!animate)ling_view_pager_show_page(self,self->page_num);
            else{

                self->op->state=LING_OPERATE_STATE_WAITTING;
                ling_operate_set_animation_cb(self->op,ling_view_pager_animate_right,self);
                ling_operate_set_finish_cb(self->op,ling_view_pager_operate_finish_right,self);
                ling_operate_start_operating(self->op);
                ling_operate_run_animation(self->op);
            }
        }
        else return FALSE;
    }
    else {
        if(!animate)ling_view_pager_show_page(self,self->page_now_pos-1);
        else{

            self->op->state=LING_OPERATE_STATE_WAITTING;
            ling_operate_set_animation_cb(self->op,ling_view_pager_animate_right,self);
            ling_operate_set_finish_cb(self->op,ling_view_pager_operate_finish_right,self);
            ling_operate_start_operating(self->op);
            ling_operate_run_animation(self->op);
        }
    }
    return TRUE;
}

void ling_view_pager_page_move(LingViewPager * self,gdouble x){
    GtkWidget * page = ling_view_pager_get_page_now(self);
    GtkWidget * page_new=NULL;
    gtk_fixed_move(GTK_FIXED(self->fixed),page,x,0);
    int w = gtk_widget_get_width(page);
    for(GtkWidget * c = gtk_widget_get_first_child(self->fixed);c!=NULL;c=gtk_widget_get_next_sibling(c)){
        if(c!=page){
            g_object_ref(c);
            gtk_fixed_remove(GTK_FIXED(self->fixed),c);
        }
    }
    if(self->page_num<=1);
    else if(x<0){
        if(self->page_now_pos==self->page_num&&self->page_cycle){
            page_new = ling_view_pager_get_page_by_pos(self,1);
        }
        else {
            page_new = ling_view_pager_get_page_by_pos(self,self->page_now_pos+1);
        }

        if(page_new!=NULL){
            if(!gtk_widget_have_child(self->fixed,page_new))gtk_fixed_put(GTK_FIXED(self->fixed),page_new,x+w,0);
            gtk_fixed_move(GTK_FIXED(self->fixed),page_new,x+w,0);
        }
    }
    else if(x>0){
        if(self->page_now_pos==1&&self->page_cycle){
            page_new = ling_view_pager_get_page_by_pos(self,self->page_num);
        }
        else {
            page_new = ling_view_pager_get_page_by_pos(self,self->page_now_pos-1);
        }

        if(page_new!=NULL){
            if(!gtk_widget_have_child(self->fixed,page_new))gtk_fixed_put(GTK_FIXED(self->fixed),page_new,x-w,0);
            gtk_fixed_move(GTK_FIXED(self->fixed),page_new,x-w,0);
        }
    }
    self->offset_x = x;
}
