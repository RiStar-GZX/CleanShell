#include "linglockscreen.h"
#include <lingdots.h>
#include <lingkeyborad.h>
#include <lingtimeviewer.h>
#include <lingsysinfoupdater.h>
#include <lingstatusbar.h>

char passwd[]="000000";

enum{
    LAYER_TOP = 0,
    LAYER_COVER,
    LAYER_VERIFY,
    LAYER_WALLPAPER,
    LAYER_END = 255
};

enum {
    SIGNAL_PASSED,
    SIGNAL_NUM
};

static guint signals[SIGNAL_NUM];

struct _LingLockScreen{
    GtkBox parent;
    GtkWidget * overlay;
    GtkWidget * wallpaper;
    GtkWidget * cover_box;
    GtkWidget * verify_box;
    GtkWidget * keyboard;//密码输入键盘
    GtkWidget * dots;
    char passwd[100];
};

G_DEFINE_FINAL_TYPE(LingLockScreen,ling_lock_screen,GTK_TYPE_BOX);

void ling_lock_screen_class_init(LingLockScreenClass * klass){
    signals[SIGNAL_PASSED] = g_signal_new(
        "passed",                   // 信号名称
        G_TYPE_FROM_CLASS(klass),      // 发射此信号的类类型
        G_SIGNAL_RUN_FIRST,            // 在其他处理程序之前运行（可选）
        0,                              // 关闭的数据结构（通常为 NULL）
        NULL,                           // 不传递参数给下一个处理者（可选）
        NULL,                           // 不需要参数列表 (可选)
        g_cclosure_marshal_VOID__VOID, // 指定默认的回调数据转换器 (void -> void)
        G_TYPE_NONE,                   // 返回值类型（这里是 void）
        0);
}

void keyboard_clicked(LingKeyboard * keyboard,char c,gpointer user_data){
    LingLockScreen * self = user_data;
    int len=strlen(self->passwd);
    if(c!='X'){
        self->passwd[len]=c;
        self->passwd[len+1]='\0';
        ling_dots_ativate(LING_DOTS(self->dots),ling_dots_num(LING_DOTS(self->dots))+1);
    }
    else if(len>0){
        self->passwd[len-1]='\0';
        ling_dots_ativate(LING_DOTS(self->dots),ling_dots_num(LING_DOTS(self->dots))-1);
    }

    if(strcmp(self->passwd,passwd)==0){
        g_signal_emit_by_name(self,"passed");
    }
    else if(strlen(self->passwd)>=strlen(passwd)){
        strcpy(self->passwd,"");
        ling_dots_ativate(LING_DOTS(self->dots),0);
    }
}

void ling_lock_screen_init(LingLockScreen * self){
    gtk_orientable_set_orientation(GTK_ORIENTABLE(self),GTK_ORIENTATION_VERTICAL);
}



static void lks_ani(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    switcher * s = user_data;
    gtk_widget_set_visible(s->main->widget,TRUE);
    gtk_widget_set_visible(s->sub->widget,TRUE);
    gtk_widget_set_margin_top(s->main->widget,-(args.progress/100.00f)*30);
    gtk_widget_set_margin_top(s->sub->widget,30-(args.progress/100.00f)*30);
    gtk_widget_set_margin_bottom(s->sub->widget,(args.progress/100.00f)*30);

    gtk_widget_set_opacity(s->main->widget,1-(args.progress/100));
    gtk_widget_set_opacity(s->sub->widget,args.progress/100);
    gtk_widget_set_opacity(shell->statusbar,1-(args.progress/100));
    ling_lock_screen_set_wallpaper_blur(LING_LOCK_SCREEN(shell->lockscreen),(args.progress/100)*20);
}

GtkWidget * ling_lock_screen_new(){
    LingLockScreen * self = LING_LOCK_SCREEN(g_object_new(LING_TYPE_LOCK_SCREEN,NULL));
    strcpy(self->passwd,"");

    //层叠
    self->overlay=ling_overlay_new();
    gtk_box_append(GTK_BOX(self),self->overlay);

    //锁屏壁纸
    self->wallpaper = ling_wallpaper_new("/home/gzx/Pictures/wallpaper3.png");
    ling_overlay_add_layer(LING_OVERLAY(self->overlay),GTK_WIDGET(self->wallpaper),LAYER_WALLPAPER);

    //封面
    self->cover_box = gtk_box_new(GTK_ORIENTATION_VERTICAL,200);
    GtkWidget * time = ling_time_viewer_new();
    ling_sys_info_updater_add_obj(shell->updater,INFO_UPDATER_TIME,time);
    gtk_box_append(GTK_BOX(self->cover_box),time);
    gtk_widget_set_halign(time,GTK_ALIGN_START);
    gtk_widget_set_margin_start(time,50);
    gtk_widget_set_margin_top(time,50);
    ling_overlay_add_layer(LING_OVERLAY(self->overlay),GTK_WIDGET(self->cover_box),LAYER_COVER);

    //验证页
    self->verify_box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);

    self->dots = ling_dots_new(6,DOTS_MODE_MULTI);
    gtk_widget_set_halign(self->dots,GTK_ALIGN_BASELINE_CENTER);
    gtk_widget_set_margin_top(self->dots,240);
    ling_dots_ativate(LING_DOTS(self->dots),0);
    self->keyboard = ling_keyboard_new();
    gtk_widget_set_margin_top(self->keyboard,80);
    gtk_widget_set_margin_bottom(self->keyboard,220);
    gtk_widget_set_margin_end(self->keyboard,80);
    gtk_widget_set_margin_start(self->keyboard,80);
    g_signal_connect(self->keyboard, "clicked",
                     G_CALLBACK(keyboard_clicked), self);
    gtk_box_append(GTK_BOX(self->verify_box),self->dots);
    gtk_box_append(GTK_BOX(self->verify_box),self->keyboard);
    ling_overlay_add_layer(LING_OVERLAY(self->overlay),GTK_WIDGET(self->verify_box),LAYER_VERIFY);
    gtk_widget_set_visible(self->verify_box,FALSE);

    gtk_widget_add_css_class(self->verify_box,"layer_back");


    //添加切换
    LingLayer * cover=ling_overlay_get_layer(LING_OVERLAY(self->overlay),LAYER_COVER);
    LingLayer * verify=ling_overlay_get_layer(LING_OVERLAY(self->overlay),LAYER_VERIFY);
    LingOperate * cover_op = ling_operate_add(shell->controler,LING_LOCK_SCREEN_COVER_OP_NAME,cover->widget);
    LingOperate * verify_op = ling_operate_add(shell->controler,"lockscreen_verify",verify->widget);
    ling_layer_add_switch(cover_op,LING_OVERLAY(self->overlay),LAYER_COVER,
                          verify_op,LING_OVERLAY(self->overlay),LAYER_VERIFY,
                          LING_ACTION_DRAG_UP,lks_ani,ling_layer_progress,ling_layer_release,
                          ling_layer_main_finish,ling_layer_sub_finish);

    return GTK_WIDGET(self);
}

LingOverlay * ling_lock_screen_get_layer_cover(LingLockScreen * self,LingLayer ** layer){
    *layer = ling_overlay_get_layer(LING_OVERLAY(self->overlay),LAYER_COVER);
    return LING_OVERLAY(self->overlay);
}


void ling_lock_screen_set_wallpaper_blur(LingLockScreen * self,uint blur){
    GString * str=g_string_new("");
    g_string_printf(str,"picture,box { filter: blur(%dpx); }",blur);
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
                                    str->str, -1);
    gtk_style_context_add_provider(gtk_widget_get_style_context(self->wallpaper),
                                   GTK_STYLE_PROVIDER(provider),GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    gtk_style_context_add_provider(gtk_widget_get_style_context(GTK_WIDGET(self->cover_box)),
                                   GTK_STYLE_PROVIDER(provider),GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_free(str);
    g_object_unref(provider);
}
