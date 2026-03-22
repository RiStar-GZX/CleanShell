#include "lingwindow.h"

/****************侧边栏***************************************************/
typedef struct{
    GtkWidget * widget;
    LingOperate * show_ani;
    LING_WINDOW_SIDE side;
    gdouble proportion;
    gdouble show_progress;

    GUIDE_LOAD load;
    gpointer load_data;
}LingSide;

struct _LingWindow{
    GtkWidget parent;
    LING_WINDOW_TYPE type;

    GtkLayoutManager * manager;
    GtkWidget * main;
    GList * pages;  //WindowPage

    GList * sides;  //LingSide
};

G_DEFINE_FINAL_TYPE(LingWindow,ling_window,GTK_TYPE_WIDGET);


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
    for(GList * l=window->sides;l!=NULL;l=l->next){
        LingSide * side = l->data;
        int rw,rh;
        gtk_widget_get_size_request(side->widget,&rw,&rh);

        child_allocation.x = 0;
        child_allocation.y = 0;
        child_allocation.width = width;
        child_allocation.height = height;

        gdouble proportion = side->proportion;
        if(proportion<0)proportion = 0;
        if(proportion>100)proportion = 100;

        if(side->side==LING_WINDOW_SIDE_LEFT||
            side->side==LING_WINDOW_SIDE_RIGHT){
            child_allocation.width *= proportion/100.0000f;
            if(rh>0)child_allocation.y = (height - rh)/2;
        }
        if(side->side==LING_WINDOW_SIDE_TOP||
            side->side==LING_WINDOW_SIDE_BOTTOM){
            child_allocation.height *= proportion/100.0000f;
            if(rw>0)child_allocation.x = (width - rw)/2;
        }

        if(rw>0){
            child_allocation.width = rw;
        }

        if(rh>0){
            child_allocation.height = rh;
        }

        if(side->side==LING_WINDOW_SIDE_LEFT){
            child_allocation.x = child_allocation.width*(side->show_progress/100-1);
        }
        if(side->side==LING_WINDOW_SIDE_TOP){
            child_allocation.y = child_allocation.height*(side->show_progress/100-1);
        }
        if(side->side==LING_WINDOW_SIDE_RIGHT){
            child_allocation.x = width-child_allocation.width*(side->show_progress/100);
        }
        if(side->side==LING_WINDOW_SIDE_BOTTOM){
            child_allocation.y = height-child_allocation.height*(side->show_progress/100);
        }


        gtk_widget_size_allocate(side->widget, &child_allocation, baseline);
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
    self->manager = ling_window_layout_new();
    gtk_widget_set_layout_manager(GTK_WIDGET(self),self->manager);
}

GtkWidget * ling_window_new(LING_WINDOW_TYPE type){
    LingWindow * win =  LING_WINDOW(g_object_new(LING_TYPE_WINDOW,NULL));
    win->type = type;
    if(win->type==LING_WINDOW_TYPE_MULTI){
        win->main = ling_view_pager_new(TRUE);
        gtk_widget_set_parent(win->main,GTK_WIDGET(win));
    }
    return GTK_WIDGET(win);
}

/*****************************单页********************************/
void ling_window_set_main(LingWindow * window,GtkWidget * widget){
    if(widget == NULL || window == NULL)return;
    if(window->type!=LING_WINDOW_TYPE_SINGAL)return;
    if(window->main!=NULL){
        gtk_widget_set_parent(window->main,NULL);
    }
    window->main = widget;
    gtk_widget_set_parent(window->main,GTK_WIDGET(window));
}

/*****************************多页********************************/

void ling_window_add_page(LingWindow * window,GtkWidget * widget,gpointer user_data){
    if(widget == NULL || window == NULL)return;
        if(window->type!=LING_WINDOW_TYPE_MULTI ||
            window->main==NULL)return;

    WindowPage * page = g_malloc0(sizeof(WindowPage));
    page->widget = widget;
    page->user_data = user_data;
    ling_view_pager_add_page(LING_VIEW_PAGER(window->main),widget);
    window->pages = g_list_append(window->pages,page);
}

void ling_window_remove_page(LingWindow * window,GtkWidget * widget){
    if(widget == NULL || window == NULL)return;
    if(window->type!=LING_WINDOW_TYPE_MULTI ||
        window->main==NULL)return;

    ling_view_pager_remove_page(LING_VIEW_PAGER(window->main),widget);
    for(GList * l = window->pages;l!=NULL;l=l->next){
        WindowPage * page = l->data;
        window->pages = g_list_remove(window->pages,page);
    }
}

void ling_window_page_set_pos(LingWindow * window,uint pos){
    if( window == NULL||
        window->type!=LING_WINDOW_TYPE_MULTI ||
        window->main==NULL)return;

    ling_view_pager_show_page(LING_VIEW_PAGER(window->main),pos);
}



/********************侧栏*************************/
static GList * ling_window_get_side(LingWindow * window,GtkWidget * widget){
    if(window==NULL||widget==NULL)return NULL;
    for(GList * l = window->sides;l!=NULL;l=l->next){
        LingSide * side = (LingSide*)l->data;
        if(side->widget==widget)return l;
    }
    return NULL;
}

LingSide * ling_window_get_side2(LingWindow * window,GtkWidget * widget){
    if(window==NULL||widget==NULL)return NULL;
    for(GList * l = window->sides;l!=NULL;l=l->next){
        LingSide * side = (LingSide*)l->data;
        if(side->widget==widget)return side;
    }
    return NULL;
}


void ling_window_add_side(LingWindow * window,GtkWidget * widget,
                           LING_WINDOW_SIDE side,gdouble proportion){
    LingSide * new_side = g_malloc0(sizeof(LingSide));
    new_side->widget = widget;
    new_side->proportion = proportion;
    new_side->side = side;
    new_side->show_progress = 100;
    gtk_widget_set_parent(new_side->widget,GTK_WIDGET(window));
    window->sides = g_list_append(window->sides,new_side);
}

void ling_window_update_allocate(LingSide * side){
    if(gtk_widget_get_realized(side->widget)){
        GtkWidget * parent = gtk_widget_get_parent(side->widget);
        if(parent==NULL)return;
        GtkLayoutManager * layout = gtk_widget_get_layout_manager(parent);
        if(layout==NULL)return;
        gtk_layout_manager_allocate(layout,parent,-1,-1,-1);
    }
}

void ling_window_remove(LingWindow * window,GtkWidget * widget){
    GList * l = ling_window_get_side(window,widget);
    if(l!=NULL){
        LingSide * side = (LingSide *)l->data;
        gtk_widget_set_parent(side->widget,NULL);
        window->sides = g_list_remove(window->sides,l->data);
        free(side);
    }
}

void ling_window_side_set_side(LingWindow * window,GtkWidget * widget,LING_WINDOW_SIDE side){
    LingSide * get_side = ling_window_get_side2(window,widget);
    if(get_side==NULL)return;
    get_side->side = side;
    ling_window_update_allocate(get_side);
}

void ling_window_set_side_proportion(LingWindow * window,GtkWidget * widget,gdouble proportion){
    LingSide * side = ling_window_get_side2(window,widget);
    if(side==NULL)return;
    if(proportion<0||proportion>100)return;
    side->proportion = proportion;
    ling_window_update_allocate(side);
}

static void side_slide_animate(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    LingSide * side = user_data;
    side->show_progress = args.progress;
    ling_window_update_allocate(side);
}

LingOperate * ling_window_side_enable_ani(LingWindow * window,GtkWidget * widget){
    LingSide * side = ling_window_get_side2(window,widget);
    if(side==NULL)return NULL;

    side->show_ani = ling_operate_add_animate(shell->controler,"side_slide",
                                                NULL,NULL,
                                                side_slide_animate,side,
                                                NULL,NULL,NULL);
    //默认是展开状态,也就是FINISH_DIR_END
    ling_operate_set_dir(side->show_ani,LING_ACTION_ANIMATE,FINISH_DIR_END);

    return side->show_ani;
}

void ling_window_side_set_show_progress(LingWindow * window,GtkWidget * widget,gdouble progress){
    LingSide * side = ling_window_get_side2(window,widget);
    if(side==NULL)return;
    side->show_progress = progress;
}

void ling_window_side_set_guide(LingWindow * window,GtkWidget * widget,GUIDE_LOAD load,gpointer load_data){
    LingSide * side = ling_window_get_side2(window,widget);
    if(side==NULL)return;
    side->load = load;
    side->load_data = load_data;
}

void ling_window_side_guide_update(LingWindow * window){
    for(GList * l = window->sides;l!=NULL;l=l->next){
        LingSide * side = (LingSide*)l->data;
        if(side->load==NULL)continue;
        side->load(window,side->widget,window->pages,side->load_data);
    }
}
