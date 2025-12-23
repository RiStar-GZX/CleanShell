#pragma once

#include <gtk/gtk.h>
#include <lingoperate.h>
#include <lingwallpaper.h>
#include <lingshell.h>

G_BEGIN_DECLS

#define LING_LOCK_SCREEN_COVER_OP_NAME "lockscreen_cover"

#define LING_TYPE_LOCK_SCREEN (ling_lock_screen_get_type())
    G_DECLARE_FINAL_TYPE(LingLockScreen,ling_lock_screen,LING,LOCK_SCREEN,GtkBox)

GtkWidget * ling_lock_screen_new();

LingOverlay * ling_lock_screen_get_layer_cover(LingLockScreen * self,uint * level);

void ling_lock_screen_set_wallpaper_blur(LingLockScreen * self,uint blur);

G_END_DECLS
