#include "taskswitcher.h"
#include <lingviewpager.h>
#include <lingwallpaper.h>
#include <desktop.h>
#include <wm/wm.h>

enum {
    BACK,
    SIGNAL_NUM
};

static guint signals[SIGNAL_NUM];

struct _LingTaskSwitcher{
    GObject parent_class;
    void (*clicked)(LingKeyboard *keyboard,const char * c);
};

struct _ClmTaskSwitcher{
    GtkBox parent;
    //条
    GtkWidget * bar;
};

G_DEFINE_FINAL_TYPE(ClmTaskSwitcher,clm_task_switcher,GTK_TYPE_BOX);

/*----------------------------------------------------------------------------------------------------*/

static void clm_task_switcher_class_init(ClmTaskSwitcherClass * klass){
    signals[BACK] = g_signal_new(
        "back",                         // 信号名称
        G_TYPE_FROM_CLASS(klass),       // 发射此信号的类类型
        G_SIGNAL_RUN_FIRST,             // 在其他处理程序之前运行（可选）
        0,                              // 关闭的数据结构（通常为 NULL）
        NULL,                           // 不传递参数给下一个处理者（可选）
        NULL,                           // 不需要参数列表 (可选)
        NULL,
        G_TYPE_NONE,                    // 返回值类型（这里是 void）
        4,
        G_TYPE_DOUBLE,
        G_TYPE_DOUBLE,
        G_TYPE_DOUBLE,
        G_TYPE_DOUBLE
        );
}

static void finish_e(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    ClmTaskSwitcher * self = user_data;
    g_signal_emit_by_name(self,"back",args.offset_x,args.offset_y,args.velocity_x,args.velocity_y);

    //cl_wm_close_current_window(CL_WM(shell->wm));
}

static void clm_task_switcher_init(ClmTaskSwitcher * self){
    gtk_orientable_set_orientation(GTK_ORIENTABLE(self),GTK_ORIENTATION_VERTICAL);
    //条
    self->bar = gtk_label_new("");
    gtk_widget_add_css_class(self->bar,"task_switcher_bar");
    gtk_widget_set_halign(self->bar,GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(self->bar,120,10);

    LingOperate * op = ling_operate_add(shell->controler,"task_switcher_bar_click",self->bar);
    ling_operate_add_action(op,LING_ACTION_ALL,NULL,NULL,
                            NULL,NULL,
                            NULL,NULL,
                            NULL,finish_e,self);

    gtk_widget_set_valign(GTK_WIDGET(self),GTK_ALIGN_END);
    gtk_widget_set_halign(GTK_WIDGET(self),GTK_ALIGN_CENTER);

    gtk_box_append(GTK_BOX(self),self->bar);
}

GtkWidget * clm_task_switcher_new(){
    return g_object_new(CLM_TYPE_TASK_SWITCHER,NULL);
}


