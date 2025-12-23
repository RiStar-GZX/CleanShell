#pragma once

#include <gtk/gtk.h>
#include <lingshell.h>
#include <lingoperate.h>
#include <lingviewpager.h>

G_BEGIN_DECLS

#define LING_DESKTOP_BODYBOX_OP_NAME "bodybox_layer"

#define LING_TYPE_DESKTOP (ling_desktop_get_type())
G_DECLARE_FINAL_TYPE(LingDesktop,ling_desktop,LING,DESKTOP,GtkBox)

enum drag_mode{
    DRAG_MODE_NONE,
    DRAG_MODE_PAGE,
    DRAG_MODE_DRAWER_UP,
    DRAG_MODE_DRAWER_DOWN,
    DRAG_MODE_APP_ITEM,
};

enum layer_now{
    LAYER_NOW_DESKTOP,
    LAYER_NOW_DRAWER,
};
/*
typedef struct share_info{
    GtkWidget * appview;
    GtkWidget * bodybox;
    GtkWidget * drawer;

    GtkWidget * statusbar;

    GtkWidget * overlay;

    LingOpControler * controler;

    GList * app_info;

    GtkWidget * drag_icon;

    //GtkWidget * drag_face;
}share_info;*/


typedef struct style_info{
    uint column_num;
    uint row_num;
    uint column_space;
    uint row_space;
    uint icon_size;
}style_info;

struct _LingDesktop{
    GtkBox parent;

    GtkWidget * overlay;
    //状态栏
    //GtkWidget * status_bar;
    //主体
    GtkWidget * bodybox;

    //页面
    GtkWidget * view_pager;

    //底栏
    GtkWidget * dock; //gtkgrid
    //GList * dock_app_items;


    GtkWidget * drawer; //抽屉



    uint app_num;   //只在ling_desktop_page_load_test使用(抛弃)

    style_info style_info;

    //share_info share_info; //共享信息

    GtkWidget * wallpaper;  //GtkPicture
    uint blur;

    //LingDataSaver * data_saver;
};


GtkWidget * ling_desktop_new();

void ling_desktop_set_opacity(GtkWidget * body,GtkWidget * up,uint win_height);

void drawerup_animation(gdouble velocity_x,gdouble velocity_y,gdouble progress,gpointer data);
void drawerup_finish(gpointer data);
void drawerdown_animation(gdouble velocity_x,gdouble velocity_y,gdouble progress,gpointer user_data);
void drawerdown_finish(gpointer data);

void ling_desktop_set_wallpaper_blur(LingDesktop *self,uint px);
uint ling_desktop_get_wallpaper_blur(LingDesktop *self);

void center_ani(GtkWidget * widget,LingActionArgs args,gpointer user_data);
LingOverlay * ling_desktop_get_layer_bodybox(LingDesktop * self,uint * level);
G_END_DECLS
