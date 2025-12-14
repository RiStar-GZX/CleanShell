#include "lingstatusbar.h"
#include <stdio.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/wireless.h>
#include <lingshell.h>
#include <lingnotifycenter.h>
#include <lingcontrolcenter.h>
#include <lingviewpager.h>

enum{
    LAYER_TOP = 0,

    LAYER_STATUSBAR,
    LAYER_CENTER,

    LAYER_END = 255
};

struct _LingCenterHead{
    GtkBox parent;
    GtkWidget * center_head;
    GtkWidget * time_label;
    GtkWidget * date_label;
    GtkWidget * center_head_box;
};

G_DEFINE_FINAL_TYPE(LingCenterHead,ling_center_head,GTK_TYPE_BOX);

static gboolean ling_center_head_status_update(gpointer user_data){
    LingCenterHead * self = LING_CENTER_HEAD(user_data);
    gtk_label_set_label(GTK_LABEL(self->time_label),ling_sys_info_get_time_label());
    GString * str = g_string_new("");
    g_string_printf(str,"%s\n %s",ling_sys_info_get_date_label(0,1,1,0),ling_sys_info_get_date_label(0,0,0,1));
    gtk_label_set_label(GTK_LABEL(self->date_label),str->str);
    g_free(str);
    return 1;
}

static void ling_center_head_class_init(LingCenterHeadClass * klass){

}

static void ling_center_head_init(LingCenterHead * self){
    gtk_orientable_set_orientation(GTK_ORIENTABLE(self),GTK_ORIENTATION_VERTICAL);
    GtkBuilder * builder = gtk_builder_new_from_file("../../ui/notification center.ui");
    self->center_head = GTK_WIDGET(gtk_builder_get_object(builder,"center_head"));
    self->center_head_box = GTK_WIDGET(gtk_builder_get_object(builder,"center_head_box"));
    self->time_label = GTK_WIDGET(gtk_builder_get_object(builder,"center_head_time"));
    self->date_label = GTK_WIDGET(gtk_builder_get_object(builder,"center_head_date"));
    gtk_box_append(GTK_BOX(self),self->center_head);

    guint timer_id = g_timeout_add(500, ling_center_head_status_update, self);
}

void ling_center_head_append(LingCenterHead * self,GtkWidget * widget){
    gtk_box_append(GTK_BOX(self->center_head),widget);
}

void ling_center_head_box_append(LingCenterHead * self,GtkWidget * widget){
    gtk_box_append(GTK_BOX(self->center_head_box),widget);
}

GtkWidget * ling_center_head_new(){
    return g_object_new(LING_TYPE_CENTER_HEAD,NULL);
}

struct _LingStatusBar{
    GtkBox parent;

    GtkWidget * overlay;
    GtkWidget * statusbar;
    GtkWidget * background;

    GtkWidget * viewpager;
    GtkWidget * page_notify;
    GtkWidget * page_control;


    GtkWidget * camera_space;
    GtkWidget * center_space;
    GtkWidget * wlan;
    GtkWidget * sim1;
    GtkWidget * sim2;
    GtkWidget * battery;
    GtkWidget * battery_label;
    GtkWidget * time;

    GtkCssProvider *provider;

    //LingOpControler * controler;

    //uint center_mode;

    //shell * shell;
};



G_DEFINE_FINAL_TYPE(LingStatusBar,ling_status_bar,GTK_TYPE_BOX);

void ling_status_bar_set_time(LingStatusBar * self);
gboolean ling_status_bar_get_battery_capacity(LingStatusBar * self);
gboolean ling_status_bar_get_wlan_quality(LingStatusBar * self,const char * interface);
int ling_status_bar_set_css_color(LingStatusBar * self);


static gboolean update_status(gpointer user_data){
    LingStatusBar * self = (LingStatusBar*)user_data;

    gtk_label_set_label(GTK_LABEL(self->time),ling_sys_info_get_time_label());

    //更改电池图标

    uint persent;
    gboolean is_charging = ling_sys_get_battery_capacity(&persent);
    GString * str = g_string_new("");
    g_string_printf(str,"%d%%",persent);
    gtk_label_set_label(GTK_LABEL(self->battery_label),str->str);
    GString * icon_name = g_string_new("");
    if(persent==100)g_string_printf(icon_name,"battery-100");
    else if(persent>90)g_string_printf(icon_name,"battery-090");
    else if(persent>80)g_string_printf(icon_name,"battery-080");
    else if(persent>70)g_string_printf(icon_name,"battery-070");
    else if(persent>60)g_string_printf(icon_name,"battery-060");
    else if(persent>50)g_string_printf(icon_name,"battery-050");
    else if(persent>40)g_string_printf(icon_name,"battery-040");
    else if(persent>30)g_string_printf(icon_name,"battery-030");
    else if(persent>20)g_string_printf(icon_name,"battery-020");
    else if(persent>10)g_string_printf(icon_name,"battery-010");
    else g_string_printf(icon_name,"battery-000");
    if(is_charging)g_string_append_printf(icon_name,"-charging");

    gtk_image_set_from_icon_name(GTK_IMAGE(self->battery),icon_name->str);
    g_free(icon_name);
    g_free(str);

    //更新信号图标
    uint wlan_quality = ling_sys_info_get_wlan_quality("wlan0");
    double signal = (double)wlan_quality/70;
    if(signal>0.8)gtk_image_set_from_icon_name(GTK_IMAGE(self->wlan),"network-wireless-100");
    else if(signal>0.6&&signal<=0.8)gtk_image_set_from_icon_name(GTK_IMAGE(self->wlan),"network-wireless-80");
    else if(signal>0.4&&signal<=0.6)gtk_image_set_from_icon_name(GTK_IMAGE(self->wlan),"network-wireless-60");
    else if(signal>0.2&&signal<=0.4)gtk_image_set_from_icon_name(GTK_IMAGE(self->wlan),"network-wireless-40");
    else if(signal>=0.0&&signal<=0.2)gtk_image_set_from_icon_name(GTK_IMAGE(self->wlan),"network-wireless-20");
    return 1;
}

gboolean drop_up_animation(gdouble velocity_x,gdouble velocity_y,gpointer data){
    LingStatusBar * self = LING_STATUS_BAR(data);

    double opacity = gtk_widget_get_opacity(self->viewpager);

    if(opacity<=0)return LING_OPERATE_ANIMATION_REMOVE;
    opacity-=0.01;
    //opacity-=velocity_y;
    ling_status_bar_set_drop_degree(self,opacity);
    return LING_OPERATE_ANIMATION_CONTINUE;
}

gboolean drop_down_animation(gdouble velocity_x,gdouble velocity_y,gpointer data){
    LingStatusBar * self = LING_STATUS_BAR(data);

    double opacity = gtk_widget_get_opacity(self->viewpager);

    if(opacity>=1.0)return LING_OPERATE_ANIMATION_REMOVE;
    opacity+=0.01;
    //opacity+=velocity_y;
    ling_status_bar_set_drop_degree(self,opacity);
    return LING_OPERATE_ANIMATION_CONTINUE;
}

void drop_down_finish(gpointer data){
    LingStatusBar * self = LING_STATUS_BAR(data);
    gtk_widget_set_visible(self->viewpager,1);
}

void drop_up_finish(gpointer data){
    LingStatusBar * self = LING_STATUS_BAR(data);
//    gtk_widget_set_visible(self->page_control,0);
//    gtk_widget_set_visible(self->page_notify,0);
    gtk_widget_set_visible(LING_DESKTOP(shell->desktop)->bodybox,1);
    gtk_widget_set_visible(self->viewpager,0);
    gtk_widget_set_opacity(self->statusbar,1);

    //self->center_mode=CENTER_MODE_NONE;
}

static int ison,x,y;
static void ling_status_bar_drag_begin (GtkGestureDrag* drag,gdouble start_x,
                                        gdouble start_y,LingStatusBar * self){
    ison=0;
    x=start_x;
    //if(self->center_mode!=CENTER_MODE_NONE)return;
}

static void ling_status_bar_drag_update (GtkGestureDrag* drag,
                                         gdouble offset_x,gdouble offset_y,LingStatusBar * self){
    g_print("update\n");
    //if(ison==0&&self->center_mode!=CENTER_MODE_NONE)return;
    if(ison!=0);
    else if(offset_y>0){
        ison=1;
        if(x<gtk_widget_get_width(GTK_WIDGET(self))/2){
            //self->center_mode = CENTER_MODE_NOTICE;
            ling_view_pager_show_page(LING_VIEW_PAGER(self->viewpager),1);
        }else{
            //self->center_mode = CENTER_MODE_CONTROL;
            ling_view_pager_show_page(LING_VIEW_PAGER(self->viewpager),2);
        }
    }else{
        ison=2;
    }


    if(ison==1){
        LingOperate * op=ling_operate_get(shell->controler,"status_bar_drop");
        if(ling_operate_start_operating(op))
        {
            gdouble a= offset_y/300;
            ling_status_bar_set_drop_degree(self,a);
            if(offset_y>100){
                ling_operate_set_animation_cb(op,drop_down_animation,op->animation_data);
                ling_operate_set_finish_cb(op,drop_down_finish,op->finish_data);
            }else{
                ling_operate_set_animation_cb(op,drop_up_animation,op->animation_data);
                ling_operate_set_finish_cb(op,drop_up_finish,op->finish_data);
            }
        }
    }
}

static void ling_status_bar_drag_end (GtkGestureDrag* drag,gdouble offset_x,
                                      gdouble offset_y,LingStatusBar * self){
    ling_operate_run_animation(ling_operate_get(shell->controler,"status_bar_drop"));
}


static void ling_status_bar_page_drag_begin (GtkGestureDrag* drag,gdouble start_x,
                                        gdouble start_y,LingStatusBar * self){
    ison=0;
}

static void ling_status_bar_page_drag_update (GtkGestureDrag* drag,
                                         gdouble offset_x,gdouble offset_y,LingStatusBar * self){
    if(ison!=0);
    else if(offset_y<0){
        ison=1;
    }else{
        ison=2;
    }

    if(ison==1){
        LingOperate * op=ling_operate_get(shell->controler,"status_bar_drop");
        if(ling_operate_start_operating(op))
        {
            gdouble a= 1+offset_y/300;
            ling_status_bar_set_drop_degree(self,a);
            if(offset_y>-100){
                ling_operate_set_animation_cb(op,drop_down_animation,op->animation_data);
                ling_operate_set_finish_cb(op,drop_down_finish,op->finish_data);
            }else{
                ling_operate_set_animation_cb(op,drop_up_animation,op->animation_data);
                ling_operate_set_finish_cb(op,drop_up_finish,op->finish_data);
            }
        }
    }
}

static void ling_status_bar_page_drag_end (GtkGestureDrag* drag,gdouble offset_x,
                                      gdouble offset_y,LingStatusBar * self){
    ling_operate_run_animation(ling_operate_get(shell->controler,"status_bar_drop"));
}



static void ling_status_bar_class_init(LingStatusBarClass * kalss){

}

static void ling_status_bar_init(LingStatusBar * self){
    gtk_orientable_set_orientation(GTK_ORIENTABLE(self),GTK_ORIENTATION_HORIZONTAL);

    //层叠
    self->overlay = ling_overlay_new();
    gtk_widget_set_vexpand(GTK_WIDGET(self->overlay),TRUE);
    gtk_widget_set_hexpand(GTK_WIDGET(self->overlay),TRUE);
    gtk_box_append(GTK_BOX(self),self->overlay);


    self->statusbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,3);
    gtk_widget_set_margin_top(self->statusbar,3);
    ling_overlay_add_layer(LING_OVERLAY(self->overlay),self->statusbar,LAYER_STATUSBAR);


    //添加控件
    int icon_size =20;
    self->wlan = gtk_image_new_from_icon_name("network-wireless-100");
    gtk_image_set_pixel_size(GTK_IMAGE(self->wlan),icon_size);
    self->sim1 = gtk_image_new_from_icon_name("network-mobile-80-5g");
    gtk_image_set_pixel_size(GTK_IMAGE(self->sim1),icon_size);
    self->sim2 = gtk_image_new_from_icon_name("network-mobile-40-5g");
    gtk_image_set_pixel_size(GTK_IMAGE(self->sim2),icon_size);
    self->battery = gtk_image_new_from_icon_name("battery-100");
    gtk_image_set_pixel_size(GTK_IMAGE(self->battery),icon_size);
    self->battery_label = gtk_label_new("100%");
    self->time = gtk_label_new("00:00");
    self->center_space = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_set_hexpand(self->center_space,1);
    gtk_widget_set_opacity(self->center_space,0);

    ling_status_bar_set_css_color(self);

    gtk_box_append(GTK_BOX(self->statusbar),self->sim1);
    gtk_widget_set_margin_start(self->sim1,5);
    gtk_box_append(GTK_BOX(self->statusbar),self->sim2);
    gtk_box_append(GTK_BOX(self->statusbar),self->time);
    gtk_box_append(GTK_BOX(self->statusbar),self->center_space);
    gtk_box_append(GTK_BOX(self->statusbar),self->wlan);
    gtk_box_append(GTK_BOX(self->statusbar),self->battery_label);
    gtk_box_append(GTK_BOX(self->statusbar),self->battery);
    gtk_widget_set_margin_end(self->battery,5);

    gtk_widget_set_hexpand(GTK_WIDGET(self),1);
    gtk_widget_set_vexpand(GTK_WIDGET(self),1);
    gtk_widget_set_hexpand(self->overlay,1);
    gtk_widget_set_vexpand(self->overlay,1);


    //状态刷新
    guint timer_id = g_timeout_add(500, update_status, self);

}

static void ling_status_bar_set_operate(LingStatusBar * self){
    //拖拽
    LingOperate * op_notice=ling_operate_add(shell->controler,"status_bar_drop",
                                             drop_down_animation,self,
                                             NULL,NULL,
                                             drop_down_finish,self);
    GtkGesture * drag = gtk_gesture_drag_new();
    gtk_widget_add_controller(GTK_WIDGET(self->statusbar),GTK_EVENT_CONTROLLER(drag));
    g_signal_connect(drag, "drag-begin", G_CALLBACK(ling_status_bar_drag_begin), self);
    g_signal_connect(drag, "drag-update", G_CALLBACK(ling_status_bar_drag_update), self);
    g_signal_connect(drag, "drag-end", G_CALLBACK(ling_status_bar_drag_end), self);

    GtkGesture * swipe = gtk_gesture_swipe_new();
    gtk_widget_add_controller(GTK_WIDGET(self->statusbar), GTK_EVENT_CONTROLLER(swipe));
    g_signal_connect(swipe,"swipe",G_CALLBACK(ling_operate_swipe_cb),op_notice);

    self->viewpager = ling_view_pager_new();
    ling_view_pager_set_page_cycle(LING_VIEW_PAGER(self->viewpager),FALSE);
    ling_view_pager_set_dot_indicator(LING_VIEW_PAGER(self->viewpager),FALSE);
    gtk_widget_set_visible(self->viewpager,0);
    ling_overlay_add_layer(LING_OVERLAY(self->overlay),self->viewpager,LAYER_CENTER);
    //gtk_widget_insert_before(self->viewpager,self->overlay,self->statusbar);

    GtkGesture * drag_up = gtk_gesture_drag_new();
    gtk_widget_add_controller(GTK_WIDGET(self->viewpager),GTK_EVENT_CONTROLLER(drag_up));
    g_signal_connect(drag_up, "drag-begin", G_CALLBACK(ling_status_bar_page_drag_begin), self);
    g_signal_connect(drag_up, "drag-update", G_CALLBACK(ling_status_bar_page_drag_update), self);
    g_signal_connect(drag_up, "drag-end", G_CALLBACK(ling_status_bar_page_drag_end), self);

    self->page_notify = ling_notify_center_new();
    gtk_widget_set_hexpand(self->page_notify,1);
    gtk_widget_set_vexpand(self->page_notify,1);
    ling_view_pager_add_page(LING_VIEW_PAGER(self->viewpager),self->page_notify);

    self->page_control = ling_control_center_new(self->overlay);
    gtk_widget_set_hexpand(self->page_control,1);
    gtk_widget_set_vexpand(self->page_control,1);
    ling_view_pager_add_page(LING_VIEW_PAGER(self->viewpager),self->page_control);
}

GtkWidget * ling_status_bar_new(){
    LingStatusBar * self = LING_STATUS_BAR(g_object_new(LING_TYPE_STATUS_BAR,NULL));
    ling_status_bar_set_operate(self);
    return GTK_WIDGET(self);
}


int ling_status_bar_set_css_color(LingStatusBar * self){
    gtk_widget_add_css_class(self->battery_label,"light-style");
    gtk_widget_add_css_class(self->time,"light-style");
    return 1;
}

void ling_status_bar_set_drop_degree(LingStatusBar *self,gdouble opacity){
    if(opacity<=0)opacity=0;
    if(opacity>1.0)opacity=1.0;
    //self->center_mode = center_mode;
    //GtkWidget * show,*hide;
    int pos=ling_view_pager_get_pos(LING_VIEW_PAGER(self->viewpager));
//    if(self->center_mode==CENTER_MODE_NOTICE){
//        pos=1;
//        //show=self->page_notify;
//        //hide=self->page_control;
//    }
//    else{
//        pos=2;
//        //show=self->page_control;
//        //hide=self->page_notify;
//    }
    if(opacity==0){
        gtk_widget_set_opacity(self->statusbar,1);
    }
    else{
        gtk_widget_set_opacity(self->statusbar,0);
    }
    gtk_widget_set_size_request(self->viewpager,gtk_widget_get_width(shell->bodybox),
                                gtk_widget_get_height(shell->bodybox));
    gtk_widget_set_visible(self->viewpager,TRUE);
    ling_view_pager_show_page(LING_VIEW_PAGER(self->viewpager),pos);
    //gtk_widget_set_visible(show,TRUE);
    //gtk_widget_set_visible(hide,FALSE);
    //gtk_widget_set_visible(shell->bodybox,0);

    gtk_widget_set_opacity(self->viewpager,opacity);
    uint blur = (uint)(opacity*CENTER_BLUR_PX);
    ling_desktop_set_wallpaper_blur(LING_DESKTOP(shell->desktop),blur);
}

void ling_status_bar_set_page(LingStatusBar * self,uint center_mode){
    ling_view_pager_show_page(LING_VIEW_PAGER(self->viewpager),center_mode);
}

LingOverlay * ling_status_bar_get_layer_center(LingStatusBar * self,uint * level){
//测试用，以后找更好的办法
    *level = LAYER_CENTER;
    return LING_OVERLAY(self->overlay);
}
