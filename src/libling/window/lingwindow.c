#include "lingwindow.h"


struct LingGuide{
    GtkWidget * widget;
    LingOperate * show_ani;
    LING_WINDOW_GUIDE_SIDE side;
    gdouble proportion;
    gdouble show_progress;
};

/*
 * 窗口进行分层,分为主层(内容层),侧边(四个方向,底部常用于切换页面,左侧常用来切换文件夹)
*/
enum{
    LAYER_TOP = 0,

    LAYER_GUIDE,

    LAYER_MAIN,
    LAYER_END = 255,
};

struct _LingWindow{
    GtkBox parent;
    GtkLayoutManager * manager;

    GtkWidget * main;
    GList * guides;
};

G_DEFINE_FINAL_TYPE(LingWindow,ling_window,GTK_TYPE_BOX);


struct _LingWindowLayout{
    GtkLayoutManager parent;
};

G_DEFINE_FINAL_TYPE(LingWindowLayout,ling_window_layout,GTK_TYPE_LAYOUT_MANAGER);

static void ling_window_layout_allocate(GtkLayoutManager *manager,GtkWidget *widget,
                                       int width,int height,int baseline){
    GtkWidget * parent = gtk_widget_get_parent(widget);
    if(parent==NULL)return;

    if(width<0){
        width = gtk_widget_get_width(widget);
    }
    if(height<0){
        height = gtk_widget_get_height(widget);
    }

    graphene_rect_t rect;
    if(!gtk_widget_compute_bounds(parent,widget,&rect))return;
    int start = gtk_widget_get_margin_start(widget);
    int end = gtk_widget_get_margin_end(widget);
    int top = gtk_widget_get_margin_top(widget);
    int bottom = gtk_widget_get_margin_bottom(widget);
    rect.size.height-=top+bottom;
    rect.size.width-=start+end;
    LingWindow * window = LING_WINDOW(widget);

    GtkAllocation child_allocation;
    for(GList * l=window->guides;l!=NULL;l=l->next){
        LingGuide * guide = l->data;
        int rw,rh;
        gtk_widget_get_size_request(guide->widget,&rw,&rh);

        child_allocation.x = 0;
        child_allocation.y = 0;
        child_allocation.width = width;
        child_allocation.height = height;

        gdouble proportion = guide->proportion;
        if(proportion<0)proportion = 0;
        if(proportion>100)proportion = 100;

        if(guide->side==LING_WINDOW_GUIDE_LEFT||
            guide->side==LING_WINDOW_GUIDE_RIGHT){
            child_allocation.width *= proportion/100.0000f;
            if(rh>0)child_allocation.y = (height - rh)/2;
        }
        if(guide->side==LING_WINDOW_GUIDE_TOP||
            guide->side==LING_WINDOW_GUIDE_BOTTOM){
            child_allocation.height *= proportion/100.0000f;
            if(rw>0)child_allocation.x = (width - rw)/2;
        }

        if(rw>0){
            child_allocation.width = rw;
        }

        if(rh>0){
            child_allocation.height = rh;
        }

        if(guide->side==LING_WINDOW_GUIDE_LEFT){
            child_allocation.x = child_allocation.width*(guide->show_progress/100-1);
        }
        if(guide->side==LING_WINDOW_GUIDE_TOP){
            child_allocation.y = child_allocation.height*(guide->show_progress/100-1);
        }
        if(guide->side==LING_WINDOW_GUIDE_RIGHT){
            child_allocation.x = width-child_allocation.width*(guide->show_progress/100);
        }
        if(guide->side==LING_WINDOW_GUIDE_BOTTOM){
            child_allocation.y = height-child_allocation.height*(guide->show_progress/100);
        }


        gtk_widget_size_allocate(guide->widget, &child_allocation, baseline);
    }
    child_allocation.x = 0;
    child_allocation.y = 0;
    child_allocation.width = width;
    child_allocation.height = height;
    gtk_widget_size_allocate(GTK_WIDGET(window->main), &child_allocation, baseline);
}

void ling_window_layout_class_init(LingWindowLayoutClass * klass){
    GtkLayoutManagerClass * manager_class = GTK_LAYOUT_MANAGER_CLASS(klass);
    manager_class->allocate = ling_window_layout_allocate;
}

void ling_window_layout_init(LingWindowLayout * self){
}

GtkLayoutManager * ling_window_layout_new(){
    return g_object_new(LING_TYPE_WINDOW_LAYOUT,NULL);
}


static void ling_window_class_init(LingWindowClass * klass){

}

static void ling_window_init(LingWindow * self){
    //self->overlay = LING_OVERLAY(ling_overlay_new());
    //gtk_box_append(GTK_BOX(self),GTK_WIDGET(self->overlay));
    self->manager = ling_window_layout_new();
    gtk_widget_set_layout_manager(GTK_WIDGET(self),self->manager);
}

GtkWidget * ling_window_new(){
    return g_object_new(LING_TYPE_WINDOW,NULL);
}

/********************main window*************************/

/* 页面类型的窗口 */
GtkWidget * ling_window_main_page_new(){
    return NULL;
}

void ling_window_set_main(LingWindow * window,GtkWidget * widget){
    if(widget == NULL)return;
    if(window->main!=NULL){
        //ling_overlay_remove_layer(window->overlay,window->main);
        gtk_widget_set_parent(window->main,NULL);
    }
    window->main = widget;
    gtk_widget_set_parent(window->main,GTK_WIDGET(window));
    //ling_overlay_add_layer(window->overlay,widget,LAYER_MAIN);

}

/********************导航*************************/
static GList * ling_window_get_guide(LingWindow * window,GtkWidget * widget){
    if(window==NULL||widget==NULL)return NULL;
    for(GList * l = window->guides;l!=NULL;l=l->next){
        LingGuide * guide = (LingGuide*)l->data;
        if(guide->widget==widget)return l;
    }
    return NULL;
}

LingGuide * ling_window_get_guide2(LingWindow * window,GtkWidget * widget){
    if(window==NULL||widget==NULL)return NULL;
    for(GList * l = window->guides;l!=NULL;l=l->next){
        LingGuide * guide = (LingGuide*)l->data;
        if(guide->widget==widget)return guide;
    }
    return NULL;
}

void ling_window_add_guide(LingWindow * window,GtkWidget * widget,
                           LING_WINDOW_GUIDE_SIDE side,gdouble proportion){
    LingGuide * guide = g_malloc0(sizeof(LingGuide));
    guide->widget = widget;
    guide->proportion = proportion;
    guide->side = side;
    guide->show_progress = 100;
    gtk_widget_set_parent(guide->widget,GTK_WIDGET(window));
    window->guides = g_list_append(window->guides,guide);
}

void ling_window_remove(LingWindow * window,GtkWidget * widget){
    GList * l = ling_window_get_guide(window,widget);
    if(l!=NULL){
        LingGuide * guide = (LingGuide *)l->data;
        gtk_widget_set_parent(guide->widget,NULL);
        window->guides = g_list_remove(window->guides,l->data);
        free(guide);
    }
}

void ling_window_set_guide_side(LingWindow * window,GtkWidget * widget,LING_WINDOW_GUIDE_SIDE side){
    LingGuide * guide = ling_window_get_guide2(window,widget);
    if(guide==NULL)return;
    guide->side = side;
}

void ling_window_set_guide_proportion(LingWindow * window,GtkWidget * widget,gdouble proportion){
    LingGuide * guide = ling_window_get_guide2(window,widget);
    if(guide==NULL)return;
    if(proportion<0||proportion>100)return;
    guide->proportion = proportion;
    //gtk_layout_manager_allocate(window->manager,guide->widget,-1,-1,-1);
}

static void guide_slide_animate(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    LingGuide * guide = user_data;
    guide->show_progress = args.progress;
    if(gtk_widget_get_realized(guide->widget)){
        GtkWidget * parent = gtk_widget_get_parent(guide->widget);
        if(parent==NULL)return;
        GtkLayoutManager * layout = gtk_widget_get_layout_manager(parent);
        if(layout==NULL)return;
        gtk_layout_manager_allocate(layout,parent,-1,-1,-1);
    }
}

LingOperate * ling_window_guide_enable_ani(LingWindow * window,GtkWidget * widget){
    LingGuide * guide = ling_window_get_guide2(window,widget);
    if(guide==NULL)return NULL;

    guide->show_ani = ling_operate_add_animate(shell->controler,"guide_slide",
                                                NULL,NULL,
                                                guide_slide_animate,guide,
                                                NULL,NULL,NULL);
    //默认是展开状态,也就是FINISH_DIR_END
    ling_operate_set_dir(guide->show_ani,LING_ACTION_ANIMATE,FINISH_DIR_END);

    return guide->show_ani;
}

void ling_window_guide_set_show_progress(LingWindow * window,GtkWidget * widget,gdouble progress){
    LingGuide * guide = ling_window_get_guide2(window,widget);
    if(guide==NULL)return;
    guide->show_progress = progress;
}
