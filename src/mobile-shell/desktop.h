#pragma once

#include <gtk/gtk.h>
#include <ling.h>
#include <mobileshell.h>
#include <desktopitem.h>


G_BEGIN_DECLS

#define CLM_DESKTOP_BODYBOX_OP_NAME "desktop_bodybox"

#define CLM_DESKTOP_VIEWPAGER_OP_NAME "desktop_viewpager"

#define CLM_TYPE_DESKTOP (clm_desktop_get_type())
G_DECLARE_FINAL_TYPE(ClmDesktop,clm_desktop,CLM,DESKTOP,GtkBox)

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

typedef struct style_info{
    uint top_space;
    uint column_num;
    uint row_num;
    uint column_space;
    uint row_space;
    uint icon_size;
}style_info;



GtkWidget * clm_desktop_new();

void drawerup_animation(gdouble velocity_x,gdouble velocity_y,gdouble progress,gpointer data);
void drawerup_finish(gpointer data);
void drawerdown_animation(gdouble velocity_x,gdouble velocity_y,gdouble progress,gpointer user_data);
void drawerdown_finish(gpointer data);

void clm_desktop_set_wallpaper_blur(ClmDesktop *self,uint px);
uint clm_desktop_get_wallpaper_blur(ClmDesktop *self);

void clm_desktop_hide_body_and_set_blur(ClmDesktop *self,gdouble progress,uint blur);

void center_ani(GtkWidget * widget,LingActionArgs args,gpointer user_data);

LingOverlay * clm_desktop_get_layer_bodybox(ClmDesktop * self,LingLayer ** layer);

LingOverlay * clm_desktop_get_layer_task_switcher(ClmDesktop * self,LingLayer ** layer);

LingOverlay * clm_desktop_get_layer_task_switch_bar(ClmDesktop * self,LingLayer ** layer);

// void clm_desktop_folder_show(ClmDesktop * desktop,ClmDesktopItem * folder,gdouble x,gdouble y);

// void clm_desktop_folder_set_visible(ClmDesktop * desktop,gboolean visible);

G_END_DECLS
