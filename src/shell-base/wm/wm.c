#include "wm.h"

#define CL_WM_WINDOW_IMAGE_SIZE 168

// enum{
//     LAYER_TOP = 0,
//     LAYER_WINDOW,
//     LAYER_APP_ICON,
//     LAYER_END = 255
// };

typedef struct{
    ClWmWindow * window;
    GtkWidget * widget;
    WM_ANI_CB ani;
    gpointer ani_data;
    WM_OPEN_START_CB open_start;
    gpointer open_start_data;
    WM_ACT_CB open_finish;
    gpointer open_finish_data;
    WM_ACT_CB close_start;
    gpointer close_start_data;
    WM_ACT_CB close_finish;
    gpointer close_finish_data;
}window_args;

struct _ClWmWindow{
    GtkBox parent;

    ClWm * wm;

    GtkWidget * overlay;
    GtkWidget * gradient;
    GtkWidget * detail; //

    GString * window_name;
    gboolean showable;

    GtkWidget * window_box;
    GtkWidget * image;

    //icon
    GtkWidget * app_icon;

    //关闭的时候要用
    LingOperate * op;
    window_args * current_args;
};

struct _ClWm{
    LingFixed parent;
    GList * windows;
    ClWmWindow * current_win;
    ClWmWindow * focus_win; //聚焦的窗口
    gboolean cant_focus;
};

G_DEFINE_FINAL_TYPE(ClWmWindow,cl_wm_window,GTK_TYPE_BOX);

static ANI_DIR wm_close_start(GtkWidget * widget,LingActionArgs action,gpointer user_data){
    ClWmWindow * window = CL_WM_WINDOW(widget);
    window_args * wa=window->current_args;
    if(wa->close_start!=NULL){
        wa->close_start(wa->widget,wa->window,action,wa->close_start_data);
    }
    return ANI_DIR_FORWARD;
}

static void wm_close_animate(GtkWidget * widget,LingActionArgs action,gpointer user_data){
    ClWmWindow * window = CL_WM_WINDOW(widget);
    window_args * wa=window->current_args;
    action.progress = 100-action.progress;
    if(wa->ani!=NULL){
        wa->ani(wa->widget,wa->window,action,CL_WM_WINDOW_CLOSE,wa->ani_data);
    }
}

static void wm_close_finish(GtkWidget * widget,LingActionArgs action,gpointer user_data){
    ClWmWindow * window = CL_WM_WINDOW(widget);
    window_args * wa=window->current_args;
    if(wa->close_finish!=NULL){
        wa->close_finish(wa->widget,wa->window,action,wa->close_finish_data);
    }
    window->wm->current_win = NULL;
    //gtk_widget_set_visible(GTK_WIDGET(window->wm),FALSE);
}

/*****************************************************************************************************/

//用于taskswitcher向上滑动移除窗口

static void cl_wm_window_begin_focus(GtkWidget * widget,LingBeginArgs args,gpointer user_data){
    ClWmWindow * window = CL_WM_WINDOW(user_data);

    //有的窗口还按着就无法设置为新的窗口
    if(!window->wm->cant_focus){
        window->wm->focus_win=window;
    }
}

static void cl_wm_window_end_focus(GtkWidget * widget,LingEndArgs args,gpointer user_data){
    ClWmWindow * window = CL_WM_WINDOW(user_data);

    //松开手才允许focus为其他窗口
    if(window->wm->cant_focus&&window->wm->focus_win == window){
        window->wm->cant_focus=0;
    }
    // if(window->wm->focus_win == window){
    //     window->wm->focus_win = NULL;
    // }
}

/*****************************************************************************************************/

static void cl_wm_window_class_init(ClWmWindowClass * klass){

}

static void cl_wm_window_init(ClWmWindow * self){


    self->window_box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    gtk_widget_add_css_class(GTK_WIDGET(self->window_box),"cl_window");
    //ling_overlay_add_layer(LING_OVERLAY(self->overlay),self->window_box,LAYER_WINDOW);

    self->image = gtk_image_new();
    gtk_image_set_pixel_size(GTK_IMAGE(self->image),128);
    gtk_widget_set_vexpand(self->image,TRUE);
    gtk_widget_set_hexpand(self->image,TRUE);
    gtk_box_append(GTK_BOX(self->window_box),self->image);

    //self->app_icon_box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    self->app_icon = gtk_image_new();
    gtk_image_set_pixel_size(GTK_IMAGE(self->image),128);
    //ling_overlay_add_layer(LING_OVERLAY(self->overlay),self->app_icon,LAYER_APP_ICON);

    self->gradient = ling_gradient_new(self->window_box,self->app_icon);
    gtk_box_append(GTK_BOX(self),self->gradient);


    self->op = ling_operate_add(shell->controler,"wm_window",GTK_WIDGET(self));
    ling_operate_add_begin(self->op,cl_wm_window_begin_focus,self);
    ling_operate_add_end(self->op,cl_wm_window_end_focus,self);

    ling_operate_add_action(self->op,LING_ACTION_EMIT,
                            NULL,NULL,
                            wm_close_animate,self,
                            wm_close_start,self,
                            NULL,wm_close_finish,self);

    ling_operate_set_force_run(self->op,TRUE);

}

static GtkWidget * cl_wm_window_new(const char * name,const char * icon_name){
    ClWmWindow *  self = CL_WM_WINDOW(g_object_new(CL_TYPE_WM_WINDOW,NULL));
    gtk_image_set_from_icon_name(GTK_IMAGE(self->image),icon_name);
    gtk_image_set_from_icon_name(GTK_IMAGE(self->app_icon),icon_name);
    self->window_name = g_string_new(name);
    return GTK_WIDGET(self);
}



G_DEFINE_FINAL_TYPE(ClWm,cl_wm,LING_TYPE_FIXED);

static void cl_wm_class_init(ClWmClass * klass){

}

static void cl_wm_init(ClWm * self){
}

GtkWidget * cl_wm_new(){
    return g_object_new(CL_TYPE_WM,NULL);
}

// GtkAllocation adjust(GtkWidget * fixed,LingFixedItem *item,
//                      int width,int height,int baseline,graphene_rect_t rect){

// }

ClWmWindow * cl_wm_get_window_by_name(ClWm * self,const char * window_name){
    GList * list = ling_fixed_get_items_list(LING_FIXED(self));
    for(;list!=NULL;list=list->next){
        ClWmWindow * win = CL_WM_WINDOW(list->data);
        if(strcmp(win->window_name->str,window_name)==0){
            return win;
        }
    }
    return NULL;
}

ClWmWindow * cl_wm_add_window(ClWm * self,const char * icon_name,const char * name){
    char * new_name=malloc(sizeof(char)*100);
    GList * list = ling_fixed_get_items_list(LING_FIXED(self));
    strcpy(new_name,name);
    for(int i=1;;i++){
        gboolean have_same=0;
        for(GList * l=list;l!=NULL;l=l->next){
            LingFixedItem * item = (LingFixedItem*)l->data;
            ClWmWindow * win = CL_WM_WINDOW(item->widget);
            if(strcmp(win->window_name->str,new_name)==0){
                return NULL;    //不允许重名(去掉允许重名)
                sprintf(new_name,"%s_%d",name,i);
                have_same=1;
            }
        }
        if(!have_same)break;
    }

    ClWmWindow * win = CL_WM_WINDOW(cl_wm_window_new(new_name,icon_name));
    self->windows = g_list_append(self->windows,win);
    ling_fixed_put_none(LING_FIXED(self),GTK_WIDGET(win),0,0,LING_FIXED_TOP,0);
    win->wm = self;
    return win;
}

int cl_wm_remove_window(ClWm * wm,ClWmWindow * window){
    GList * list = g_list_find(wm->windows,window);
    if(list==NULL)return FALSE;
    ling_fixed_remove(LING_FIXED(wm),GTK_WIDGET(window));
    wm->windows = g_list_remove(wm->windows,window);
    return TRUE;
}

void cl_wm_set_window_showable(ClWmWindow * window,gboolean showable){
    if(window->wm==NULL)return;
    GList * list = g_list_find(window->wm->windows,window);
    if(list==NULL)return;
    LingFixedItem * info=ling_fixed_get_item_info(LING_FIXED(window->wm),GTK_WIDGET(window));
    if(info==NULL&&showable==TRUE){
        ling_fixed_put_none(LING_FIXED(window->wm),GTK_WIDGET(window),0,0,LING_FIXED_TOP,0);
    }
    if(info!=NULL&&showable==FALSE){
        ling_fixed_remove(LING_FIXED(window->wm),GTK_WIDGET(window));
    }
    window->showable = showable;
}

void cl_wm_move_window(ClWmWindow * window,gdouble x,gdouble y){
    if(!CL_IS_WM_WINDOW(window))return;
    if(!window->showable)return;
    ling_fixed_move(LING_FIXED(window->wm),GTK_WIDGET(window),x,y);
}

void cl_wm_set_window_size(ClWmWindow * window,uint w,uint h){
    gdouble size = w;
    if(w>h)size = h;
    //if(size>300)gtk_image_set_pixel_size(GTK_IMAGE(window->image),CL_WM_WINDOW_IMAGE_SIZE);
    //else {
        gtk_image_set_pixel_size(GTK_IMAGE(window->image),CL_WM_WINDOW_IMAGE_SIZE*size/300);
        gtk_image_set_pixel_size(GTK_IMAGE(window->app_icon),CL_WM_WINDOW_IMAGE_SIZE*size/300);
    //}
    ling_fixed_set_child_size(LING_FIXED(window->wm),GTK_WIDGET(window),w,h);
}

void cl_wm_set_window_level(ClWmWindow * window,int level){
    ling_fixed_set_child_level(LING_FIXED(window->wm),GTK_WIDGET(window),level,0);
}

void cl_wm_set_window_radis(ClWmWindow * window,uint px){
    ling_widget_border_radis(window->window_box,px);
}

// void cl_wm_move_window_by_progress(ClWmWindow * window,int x,int y,int level,gdouble progress){
//     if(!CL_IS_WM_WINDOW(window))return;
//     // GList * list = g_list_find(window->wm->windows,window);
//     // if(list==NULL){
//     //     window->wm->windows = g_list_append(window->wm->windows,window);
//     // }

//     int w = gtk_widget_get_width(GTK_WIDGET(window->wm));
//     int h = gtk_widget_get_height(GTK_WIDGET(window->wm));
//     LingFixedItem * info=ling_fixed_get_item_info(LING_FIXED(window->wm),GTK_WIDGET(window));
//     if(info!=NULL){
//         //ling_fixed_put(LING_FIXED(window->wm),,GTK_WIDGET(window),x,y,level,0);
//         ling_fixed_put_none(LING_FIXED(window->wm),GTK_WIDGET(window),x,y,level,0);
//     }
//     else ling_fixed_move(LING_FIXED(window->wm),GTK_WIDGET(window),x,y);
//     gdouble n_w = (progress*w/100.0000f);
//     gdouble n_h = (progress*h/100.0000f);
//     gtk_widget_set_size_request(GTK_WIDGET(window),n_w,n_h);
// }

static ANI_DIR cl_wm_open_start(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    window_args * arg = (window_args*)user_data;
    gdouble x,y;
    ClWmWindow * window=NULL;
    if(arg->open_start!=NULL)window = arg->open_start(widget,&x,&y,arg->open_start_data);
    if(window==NULL)return ANI_DIR_BACK;

    arg->window = window;
    arg->window->wm->current_win = arg->window;

    //移动到表尾
    arg->window->wm->windows = g_list_remove(arg->window->wm->windows,arg->window);
    arg->window->wm->windows = g_list_append(arg->window->wm->windows,arg->window);

    cl_wm_set_window_showable(arg->window,TRUE);
    return ANI_DIR_FORWARD;
}

static void cl_wm_open_ani(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    window_args * wa = (window_args*)user_data;
    if(wa->ani!=NULL)wa->ani(widget,wa->window,args,CL_WM_WINDOW_OPEN,wa->ani_data);
}

static void cl_wm_open_finish_start(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    window_args * wa = (window_args*)user_data;
    if(wa->close_finish!=NULL)wa->close_finish(widget,wa->window,args,wa->close_finish_data);
}

static void cl_wm_open_finish_end(GtkWidget * widget,LingActionArgs action,gpointer user_data){
    window_args * wa = (window_args*)user_data;
    if(wa->open_finish!=NULL)wa->open_finish(widget,wa->window,action,wa->open_finish_data);
    //wa->window->wm->current_win = wa->window;
    wa->window->current_args = wa;
}

void cl_wm_add_operate(LingOperate * op,ClWm * wm,const char * window_name,const char * icon_name,
                       WM_ANI_CB ani,gpointer ani_data,
                       WM_OPEN_START_CB open_start,gpointer open_start_data,
                       WM_ACT_CB open_finish,gpointer open_finish_data,
                       WM_ACT_CB close_start,gpointer close_start_data,
                       WM_ACT_CB close_finish,gpointer close_finish_data){
    window_args * args = g_malloc0(sizeof(window_args));
    args->window = NULL;//window;
    args->ani = ani;
    args->ani_data = ani_data;
    args->open_start = open_start;
    args->open_start_data = open_start_data;
    args->open_finish = open_finish;
    args->open_finish_data = open_finish_data;
    args->close_start = close_start;
    args->close_start_data = close_start_data;
    args->close_finish = close_finish;
    args->close_finish_data = close_finish_data;
    args->widget = op->widget;

    ling_operate_add_action(op,LING_ACTION_CLICK,
                            NULL,NULL,
                            cl_wm_open_ani,args,
                            cl_wm_open_start,args,
                            cl_wm_open_finish_start,cl_wm_open_finish_end,args);
}

void cl_wm_window_close(ClWmWindow * window,gdouble offset_x,gdouble offset_y,
                        gdouble velocity_x,gdouble velocity_y){
    if(window==NULL||window->current_args==NULL)return;
    ClWmBack * back = g_malloc0(sizeof(ClWmBack));
    back->offset_x=offset_x;
    back->offset_y=offset_y;
    back->velocity_x=velocity_x;
    back->velocity_y=velocity_y;
    ling_operate_emit_end(window->op,LING_ACTION_EMIT,back,TRUE);
}

void cl_wm_close_current_window(ClWm * wm,gdouble offset_x,gdouble offset_y,
                                gdouble velocity_x,gdouble velocity_y){
    if(wm->current_win==NULL)return;
    cl_wm_window_close(wm->current_win,offset_x,offset_y,velocity_x,velocity_y);
}

GtkWidget * cl_wm_window_get_layer_icon(ClWmWindow * self){
    return self->app_icon;
}

GtkWidget * cl_wm_window_get_layer_window(ClWmWindow * self){
    return self->window_box;
}

void cl_wm_window_set_gradient_progress(ClWmWindow * self,gdouble progress){
    ling_gradient_progress(LING_GRADIENT(self->gradient),progress);
    ling_widget_border_radis(self->window_box,500*(1-progress));
}

ClWmWindow * cl_wm_get_current_window(ClWm * wm){
    return wm->current_win;
}

void cl_wm_window_get_info(ClWmWindow * win,gdouble * x,gdouble * y,int * w,int *h){
    if(win==NULL)return;
    LingFixedItem * item = ling_fixed_get_item_info(LING_FIXED(win->wm),GTK_WIDGET(win));
    if(item==NULL)return;
    if(x!=NULL)*x = item->x;
    if(y!=NULL)*y = item->y;
    if(w!=NULL)*w = item->w;
    if(h!=NULL)*h = item->h;
}

void cl_wm_move_current_window(ClWm * wm,gdouble x,gdouble y){
    cl_wm_move_window(wm->current_win,x,y);
}

void cl_wm_set_current_window_size(ClWm * wm,int w,int h){
    cl_wm_set_window_size(wm->current_win,w,h);
}

GList * cl_wm_get_window_list(ClWm * wm){
    return wm->windows;
}

void cl_wm_set_current_window(ClWm * wm,ClWmWindow * win){
    wm->current_win = win;
}

ClWmWindow * cl_wm_get_focus_window(ClWm * wm){
    return wm->focus_win;
}
