#include "lingnotifycenter.h"
#include <lingsysinfo.h>


struct _LingNotifyCenter{
    GtkBox parent;

    GtkWidget * center_head;

    GtkWidget * notify_head_box;
    GtkWidget * ntf_clear;
    GtkWidget * ntf_able;

    GtkWidget * time_label;
    GtkWidget * date_label;
    GtkWidget * time_box;

    GtkWidget * notify_box;
    GtkWidget * list_box;

    GList * ntf_list;

};


G_DEFINE_FINAL_TYPE(LingNotifyCenter,ling_notify_center,GTK_TYPE_BOX);


static int ison;
static gdouble x;
static void on_drag_begin (GtkGestureDrag* gedture,gdouble start_x,
                           gdouble start_y,LingNotifyCenter * self){
    ison=0;
}

static void on_drag_update(GtkGestureDrag *gesture,
                           gdouble offset_x,gdouble offset_y,LingNotifyCenter * self) {
    if(fabs(offset_x)>fabs(offset_y)){
        ison=1;
    }else if(offset_y<0){
        ison=2;
    }
    else ison=3;

    if(ison==2){
        LingOperate * op=ling_operate_get(shell->controler,"status_bar_drop");
        if(ling_operate_start_operating(op))
        {
            gdouble a= 1.0-(-offset_y)/300;
            ling_status_bar_set_drop_degree(LING_STATUS_BAR(shell->statusbar),a);
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

static void on_drag_end(GtkGestureDrag *gesture,
                        gdouble offset_x,gdouble offset_y,LingNotifyCenter * self) {
    ling_operate_run_animation(ling_operate_get(shell->controler,"status_bar_drop"));
}


void ling_notify_view_notify_update(LingNotifyCenter * self){
    gtk_list_box_remove_all(GTK_LIST_BOX(self->list_box));
    char str[3][3][100]={"qq","钢钒\n"
                     "你只是个小丑罢了！","4:14",
                     "wechat","星歌\n"
                     "你想清楚你该走向何方了吗？","20:24",
                     "spotify","星歌\n"
                           "路在脚下，行则将至","20:25"};

    for(int i=0;i<3;i++){
        //for(GList * l=self->ntf_list;l!=NULL;l=l->next){
        //  sys_info_notify * notify =(sys_info_notify *) l->data;
        GtkWidget * item = ling_notify_item_new();
        ling_notify_item_set_image(LING_NOTIFY_ITEM(item),str[i][0]);
        ling_notify_item_set_text(LING_NOTIFY_ITEM(item),str[i][1]);
        ling_notify_item_set_time(LING_NOTIFY_ITEM(item),str[i][2]);
        gtk_list_box_append(GTK_LIST_BOX(self->list_box),item);
        //}
    }
}


static void ling_notify_center_class_init(LingNotifyCenterClass * klass){

}

static void ling_notify_center_init(LingNotifyCenter * self){
    self->center_head = ling_center_head_new();
    gtk_box_append(GTK_BOX(self),self->center_head);

    GtkBuilder * builder = gtk_builder_new_from_file("../../ui/notification center.ui");
    self->notify_box = GTK_WIDGET(gtk_builder_get_object(builder,"notify_box"));
    self->list_box = GTK_WIDGET(gtk_builder_get_object(builder,"main_list_box"));
    gtk_widget_add_css_class(self->list_box,"notify_list_box");
    self->notify_head_box = GTK_WIDGET(gtk_builder_get_object(builder,"notify_head_box"));
    self->ntf_clear = GTK_WIDGET(gtk_builder_get_object(builder,"notify_clear"));
    gtk_widget_add_css_class(self->ntf_clear,"notify_clear");

    ling_center_head_box_append(LING_CENTER_HEAD(self->center_head),self->notify_head_box);
    ling_center_head_append(LING_CENTER_HEAD(self->center_head),self->notify_box);

    ling_notify_view_notify_update(self);

    GtkGesture * drag = gtk_gesture_drag_new();
    gtk_widget_add_controller(self->center_head,GTK_EVENT_CONTROLLER(drag));
    g_signal_connect(drag, "drag-begin", G_CALLBACK(on_drag_begin), self);
    g_signal_connect(drag, "drag-update", G_CALLBACK(on_drag_update), self);
    g_signal_connect(drag, "drag-end", G_CALLBACK(on_drag_end), self);
}

GtkWidget * ling_notify_center_new(){
    return GTK_WIDGET(g_object_new(LING_TYPE_NOTIFY_CENTER,NULL));
}


