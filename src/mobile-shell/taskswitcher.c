#include "taskswitcher.h"
#include <lingviewpager.h>
#include <lingwallpaper.h>
#include <desktop.h>
#include <wm/wm.h>

enum{
    LAYER_TOP = 0,

    LAYER_WM,
    LAYER_BAR,

    LAYER_END = 255
};

struct _ClmTaskSwitcher{
    GtkBox parent;
    GtkWidget * overlay;

    //条
    GtkWidget * bar;
    //切换器
    GtkWidget * switcher;
    GtkWidget * viewpager;
    LingOperate * viewpager_op;
    GtkWidget * buttons_box;
    GtkWidget * clear_button;
    GtkWidget * add_page_button;

    gboolean button_clicked;
};

G_DEFINE_FINAL_TYPE(ClmTaskSwitcher,clm_task_switcher,GTK_TYPE_BOX);

struct _ClmTaskPage{
    GtkBox parent;
    GtkWidget * app_icon;
    GIcon * gicon;
    GdkPaintable * paintable;

    GtkWidget * app;
    ClmTaskSwitcher * task_switcher;
};

G_DEFINE_FINAL_TYPE(ClmTaskPage,clm_task_page,GTK_TYPE_BOX);

static void task_show_page(LingViewPager * self,uint pos);

void page_view_move(LingViewPager * self,gdouble offset_x){
    //uint pos;
    int page_num = ling_view_pager_get_page_num(self);
    int page_now_pos = ling_view_pager_get_page_now_pos(self);
    gboolean page_cycle = ling_view_pager_get_page_cycle(self);

    int w = 500;//gtk_widget_get_width(GTK_WIDGET(self));
    int h = 866;//gtk_widget_get_height(GTK_WIDGET(self));

    //g_print("%d %d %d %d\n",w,h,page_w,page_h);
    //ling_view_paper_hide_all(self);
    //ling_view_pager_fixed_set_page_pos(self,page_now_pos,offset_x,(h-page_h)/2);

    for(int i=1;i<=page_num;i++){
        GtkWidget * page = ling_view_pager_get_page_by_pos(self,i);
        int page_w = 300;//gtk_widget_get_width(page);
        int page_h = 600;//gtk_widget_get_height(page);

        int x=(i-page_now_pos)*(page_w+10)+offset_x+(w-page_w)/2;
        int y=(h-page_h)/2;
        // gdouble d = 1+(1.0000f/abs(w/2-(x+page_w/2)))*0.5;//*100;
        // if(d>1.2)d=1.2;
        // gtk_widget_set_size_request(page,300,600*d);
        // page_w=300*d;
        // page_h=600*d;
        ling_view_pager_fixed_set_page_pos(self,i,x,y,LING_FIXED_TOP);
    }
}

static void clm_task_page_class_init(ClmTaskPageClass * klass){

}

static gdouble task_page_progress(GtkWidget * widget,LingActionArgs args,gpointer user_data){

    ClmTaskPage * page = CLM_TASK_PAGE(widget);

    LingViewPager * vp = LING_VIEW_PAGER(page->task_switcher->viewpager);
    int w = gtk_widget_get_width(GTK_WIDGET(vp));
    int h = gtk_widget_get_height(GTK_WIDGET(vp));
    int page_w = gtk_widget_get_width(widget);
    int page_h = gtk_widget_get_height(widget);

    int pos = ling_view_pager_get_page_pos(vp,widget);
    gdouble x,y;
    ling_view_pager_fixed_get_page_pos(vp,pos,&x,&y);

    gdouble offset_y = y-(h-page_h)/2+args.offset_y;
    //if(offset_y>0)return 0;
    gdouble p = -offset_y/300*100;
    if(p>100)p=100;
    g_print("p:%f %f\n",p,offset_y);
    return p;
}

static gboolean task_page_release(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    if(args.progress<20)return LING_OPERATE_ANIMATION_DIR_BACK;
    return LING_OPERATE_ANIMATION_DIR_FORWARD;
}

static void task_page_animation(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    //gtk_widget_set_margin_top(widget,-(args.progress*3));
    ClmTaskPage * page = CLM_TASK_PAGE(widget);
    LingViewPager * vp = LING_VIEW_PAGER(page->task_switcher->viewpager);

    int pos = ling_view_pager_get_page_pos(vp,widget);
    gdouble x,y;
    ling_view_pager_fixed_get_page_pos(vp,pos,&x,&y);
    ling_view_pager_fixed_set_page_pos(vp,pos,x,y-(args.progress*3),LING_FIXED_TOP);
    gtk_widget_set_opacity(widget,1-args.progress/100);
}

static void  task_page_finish_s(GtkWidget * widget,LingActionArgs args,gpointer user_data){

}

static void  task_page_finish_e(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    ClmTaskPage * task_page = user_data;
    ClmTaskSwitcher * ts = task_page->task_switcher;
    GtkWidget * page = ling_view_pager_get_page_now(LING_VIEW_PAGER(ts->viewpager));
    if(page==NULL)return;

    ling_view_pager_remove_page(LING_VIEW_PAGER(ts->viewpager),page);
    uint pos = ling_view_pager_get_page_now_pos(LING_VIEW_PAGER(ts->viewpager));
    task_show_page(LING_VIEW_PAGER(ts->viewpager),pos);
}

static void clm_task_page_init(ClmTaskPage * self){
    gtk_orientable_set_orientation(GTK_ORIENTABLE(self),GTK_ORIENTATION_VERTICAL);
    gtk_box_set_spacing(GTK_BOX(self),10);
    self->app_icon = gtk_image_new();
    self->gicon = g_icon_new_for_string("firefox",NULL);
    GtkIconTheme *icon_theme = gtk_icon_theme_get_for_display(gdk_display_get_default());
    self->paintable =  GDK_PAINTABLE(gtk_icon_theme_lookup_by_gicon(icon_theme,self->gicon,128,1,
                                                                   GTK_TEXT_DIR_NONE,0));
    gtk_image_set_pixel_size(GTK_IMAGE(self->app_icon),64);
    gtk_image_set_from_paintable(GTK_IMAGE(self->app_icon), self->paintable);

    gtk_widget_set_margin_top(self->app_icon,0);

    self->app = /*gtk_button_new();//*/ling_wallpaper_new("/home/gzx/Pictures/wallpaper3.png");
    gtk_widget_set_vexpand(self->app,TRUE);
    gtk_box_append(GTK_BOX(self),self->app_icon);
    gtk_box_append(GTK_BOX(self),self->app);
    gtk_widget_set_size_request(GTK_WIDGET(self),300,600);
}

GtkWidget * clm_task_page_new(ClmTaskSwitcher * task_switcher){
    ClmTaskPage * self = g_object_new(CLM_TYPE_TASK_PAGE,NULL);
    self->task_switcher = task_switcher;
    LingOperate * op = ling_operate_add(shell->controler,"task_switcher_task",self);
    ling_operate_add_action(op,LING_ACTION_DRAG_UP,task_page_progress,self,
                            task_page_animation,self,
                            task_page_release,self,
                            task_page_finish_s,task_page_finish_e,self);
    return GTK_WIDGET(self);
}


/*----------------------------------------------------------------------------------------------------*/

static void clm_task_switcher_class_init(ClmTaskSwitcherClass * klass){
}

static void finish_s(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    ClmTaskSwitcher * self = user_data;
    gtk_widget_set_visible(GTK_WIDGET(self),FALSE);
    LingLayer * layer;
    clm_desktop_get_layer_bodybox(CLM_DESKTOP(shell->desktop),&layer);
    gtk_widget_set_visible(layer->widget,TRUE);
}

static void finish_e(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    ClmTaskSwitcher * self = user_data;


    /*gtk_widget_set_visible(shell->wm,TRUE);

    cl_wm_move_window_by_progress(cl_wm_add_window(CL_WM(shell->wm),"firefox","1"),
                                  100,100,LING_FIXED_TOP,50);
    LingLayer * layer;
    clm_desktop_get_layer_bodybox(CLM_DESKTOP(shell->desktop),&layer);
    gtk_widget_set_visible(layer->widget,FALSE);*/
    cl_wm_close_current_window(CL_WM(shell->wm));
}
static void pressed (GtkGestureClick* gesture,gint n_press,
                    gdouble x,gdouble y,gpointer user_data){
    ClmTaskSwitcher * self = user_data;
    //GtkWidget* widget = gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(gesture));
    //if(GTK_IS_BOX(widget))
    LingActionArgs args;
    finish_s(NULL,args,user_data);
    //ling_operate_run_animation(self->op2);
}

static gboolean task_switcher_close_release(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    ClmTaskSwitcher * self = user_data;
    if(self->button_clicked){
        self->button_clicked = FALSE;
        return LING_OPERATE_ANIMATION_DIR_BACK;
    }
    return LING_OPERATE_ANIMATION_DIR_FORWARD;
}

static void clear_button_clicked (GtkButton* button,gpointer user_data){
    ClmTaskSwitcher * self = user_data;
    self->button_clicked = TRUE;
    for(;;){
        GtkWidget * page = ling_view_pager_get_page_now(LING_VIEW_PAGER(self->viewpager));
        if(page==NULL)return;
        ling_view_pager_remove_page(LING_VIEW_PAGER(self->viewpager),page);
    }
}

void task_view_pager_ani(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    gdouble offset_x;
    // uint pos;
    LingViewPager * self = LING_VIEW_PAGER(widget);
    // int page_num = ling_view_pager_get_page_num(self);
    // int page_now_pos = ling_view_pager_get_page_now_pos(self);
    // gboolean page_cycle = ling_view_pager_get_page_cycle(self);

    int w = 300;//gtk_widget_get_width(GTK_WIDGET(self));
    // int h = 866;//gtk_widget_get_height(GTK_WIDGET(self));
    // GtkWidget * page = ling_view_pager_get_page_by_pos(self,pos);
    // //if(page==NULL)return;
    // int page_w = 300;//gtk_widget_get_width(page);
    // int page_h = 600;//gtk_widget_get_height(page);

    // g_print("%d %d %d %d\n",w,h,page_w,page_h);
    if(args.action==LING_ACTION_DRAG_LEFT){
        offset_x=(-args.progress/100)*w;
    }
    if(args.action==LING_ACTION_DRAG_RIGHT){
        offset_x=(args.progress/100)*w;
    }
    // ling_view_paper_hide_all(self);
    // //ling_view_pager_fixed_set_page_pos(self,page_now_pos,offset_x,(h-page_h)/2);

    // for(int i=1;i<=page_num;i++){
    //     int x=(i-page_now_pos)*(page_w+10)+offset_x;
    //     int y=(h-page_h)/2;
    //     ling_view_pager_fixed_set_page_pos(self,i,x,y);
    // }
    page_view_move(self,offset_x);
}

gdouble task_view_pager_progress(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    gdouble p= fabs(args.offset_x)/300*100;
    LingViewPager * self = LING_VIEW_PAGER(widget);
    //g_print("p:%f\n",p);
    int page_num = ling_view_pager_get_page_num(self);
    int page_now_pos = ling_view_pager_get_page_now_pos(self);
    if((args.action == LING_ACTION_DRAG_LEFT&&page_now_pos==page_num||
         args.action == LING_ACTION_DRAG_RIGHT&&page_now_pos==1)&&(int)p%100>30)p=30;
    return p;
}

gboolean task_view_pager_release(GtkWidget * widget,LingActionArgs args,gpointer data){
    LingViewPager * self = LING_VIEW_PAGER(widget);

    int page_num = ling_view_pager_get_page_num(self);
    int page_now_pos = ling_view_pager_get_page_now_pos(self);
    if(args.action == LING_ACTION_DRAG_LEFT&&page_now_pos==page_num||
        args.action == LING_ACTION_DRAG_RIGHT&&page_now_pos==1)return LING_OPERATE_ANIMATION_DIR_BACK;
    gdouble t=fabs(args.velocity_x+args.offset_x*0.5);
    if(t<20)return LING_OPERATE_ANIMATION_DIR_BACK;
    int p=100;
    if(t>1500)p+=((int)((t-1500)/1000))*100;
    if(args.action == LING_ACTION_DRAG_LEFT){
        int a=(page_num-page_now_pos)*100;
        if(p>a)p=a;
    }
    if(args.action == LING_ACTION_DRAG_RIGHT){
        int a = (page_now_pos-1)*100;
        if(p>a)p=a;
    }
    g_print("PPP:%d %f\n",p,args.velocity_x);
    ling_operate_set_ani_progress_end(args.op,args.action,p);
    return LING_OPERATE_ANIMATION_DIR_FORWARD;
}

static void add_page_clicked (GtkButton* button,gpointer user_data){
    ClmTaskSwitcher * self = user_data;
    self->button_clicked = TRUE;
    ling_view_pager_add_page(LING_VIEW_PAGER(self->viewpager),clm_task_page_new(self));
    task_show_page(LING_VIEW_PAGER(self->viewpager),ling_view_pager_get_page_now_pos(LING_VIEW_PAGER(self->viewpager)));
}

static void task_show_page(LingViewPager * self,uint pos){
    GtkWidget * page = ling_view_pager_get_page_by_pos(self,pos);
    if(page==NULL)return;
    ling_view_pager_set_pos(self,pos);
    page_view_move(self,0);
}

static void task_view_pager_finish_left(GtkWidget * widget,LingActionArgs args,gpointer data){
    LingViewPager * self = LING_VIEW_PAGER(data);
    task_show_page(self,ling_view_pager_get_page_now_pos(self)-args.progress_end/100);

}

static void task_view_pager_finish_right(GtkWidget * widget,LingActionArgs args,gpointer data){
    LingViewPager * self = LING_VIEW_PAGER(data);
    task_show_page(self,ling_view_pager_get_page_now_pos(self)+args.progress_end/100);
}

static void task_view_pager_finish_center(GtkWidget * widget,LingActionArgs args,gpointer data){
    LingViewPager * self = LING_VIEW_PAGER(data);
    task_show_page(self,ling_view_pager_get_page_now_pos(self));
}

/*-------------------------------------------------------------------------------------------------------------------------------*/

static void clm_task_switcher_init(ClmTaskSwitcher * self){
    gtk_orientable_set_orientation(GTK_ORIENTABLE(self),GTK_ORIENTATION_VERTICAL);
    //条
    self->bar = gtk_label_new("");
    gtk_widget_add_css_class(self->bar,"task_switcher_bar");
    gtk_widget_set_halign(self->bar,GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(self->bar,100,1);

    LingOperate * op = ling_operate_add(shell->controler,"task_switcher_bar_click",self->bar);
    ling_operate_add_action(op,LING_ACTION_CLICK,NULL,NULL,
                            NULL,NULL,
                            NULL,NULL,
                            finish_s,finish_e,self);


    gtk_widget_set_valign(self->bar,GTK_ALIGN_END);
    //ling_overlay_add_layer(LING_OVERLAY(self->overlay),self->bar,LAYER_BAR);

    //页面
    self->viewpager = ling_view_pager_new(FALSE);
    self->viewpager_op = ling_operate_add(shell->controler,"view_pager",self->viewpager);
    ling_operate_add_action(self->viewpager_op,LING_ACTION_DRAG_LEFT,
                            task_view_pager_progress,NULL,
                            task_view_pager_ani,NULL,
                            task_view_pager_release,NULL,
                            task_view_pager_finish_center,task_view_pager_finish_right,self->viewpager);

    ling_operate_add_action(self->viewpager_op,LING_ACTION_DRAG_RIGHT,
                            task_view_pager_progress,NULL,
                            task_view_pager_ani,NULL,
                            task_view_pager_release,NULL,
                            task_view_pager_finish_center,task_view_pager_finish_left,self->viewpager);

    ling_view_pager_set_page_cycle(LING_VIEW_PAGER(self->viewpager),FALSE);
    ling_view_pager_set_dot_indicator(LING_VIEW_PAGER(self->viewpager),FALSE);


    ling_view_pager_add_page(LING_VIEW_PAGER(self->viewpager),clm_task_page_new(self));
    ling_view_pager_add_page(LING_VIEW_PAGER(self->viewpager),clm_task_page_new(self));
    ling_view_pager_add_page(LING_VIEW_PAGER(self->viewpager),clm_task_page_new(self));
    ling_view_pager_add_page(LING_VIEW_PAGER(self->viewpager),clm_task_page_new(self));
    task_show_page(LING_VIEW_PAGER(self->viewpager),1);

    //下方按钮
    self->switcher = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    self->buttons_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,10);
    gtk_widget_set_halign(self->buttons_box,GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(self->buttons_box,FALSE);
    gtk_widget_set_margin_bottom(self->buttons_box,100);

    self->clear_button = gtk_button_new_with_label("clear");
    g_signal_connect(self->clear_button,"clicked",G_CALLBACK(clear_button_clicked),self);
    self->add_page_button = gtk_button_new_with_label("add page");
    g_signal_connect(self->add_page_button,"clicked",G_CALLBACK(add_page_clicked),self);
    gtk_box_append(GTK_BOX(self->buttons_box),self->clear_button);
    gtk_box_append(GTK_BOX(self->buttons_box),self->add_page_button);

    gtk_box_append(GTK_BOX(self->switcher),self->viewpager);
    gtk_box_append(GTK_BOX(self->switcher),self->buttons_box);

    //空白区域退出
    LingOperate * op2 = ling_operate_add(shell->controler,"task_switcher_close",self);
    ling_operate_add_action(op2,LING_ACTION_CLICK,NULL,NULL,
                            NULL,NULL,
                            task_switcher_close_release,self,
                            finish_e,finish_s,self);

    gtk_event_controller_set_propagation_phase(GTK_EVENT_CONTROLLER(op2->drag), GTK_PHASE_BUBBLE);

    //ling_overlay_add_layer(LING_OVERLAY(self->overlay),self->switcher,LAYER_TASK_SWITCHER);
    gtk_box_append(GTK_BOX(self),self->switcher);


    // GtkWidget * page = ling_view_pager_get_page_now(LING_VIEW_PAGER(self->viewpager));
    // ling_view_pager_remove_page(LING_VIEW_PAGER(self->viewpager),page);
    // task_show_page(LING_VIEW_PAGER(self->viewpager),2);
}

GtkWidget * clm_task_switcher_new(){
    return g_object_new(CLM_TYPE_TASK_SWITCHER,NULL);
}

GtkWidget * clm_task_switcher_get_bar(ClmTaskSwitcher * self){
    return self->bar;
}
