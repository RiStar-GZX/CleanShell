#pragma once

#include <gtk/gtk.h>
#include <ling.h>
#include <mobileshell.h>

G_BEGIN_DECLS

#define CL_LOCK_SCREEN_COVER_OP_NAME "lockscreen_cover"

#define CL_TYPE_LOCK_SCREEN (cl_lock_screen_get_type())
G_DECLARE_FINAL_TYPE(ClLockScreen,cl_lock_screen,CL,LOCK_SCREEN,GtkBox)

GtkWidget * cl_lock_screen_new();

LingOverlay * cl_lock_screen_get_layer_cover(ClLockScreen * self,LingLayer ** layer);

void cl_lock_screen_set_wallpaper_blur(ClLockScreen * self,uint blur);

G_END_DECLS
