#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define LING_TYPE_WALLPAPER (ling_wallpaper_get_type())

G_DECLARE_FINAL_TYPE(LingWallpaper,ling_wallpaper,LING,WALLPAPER,GtkBox)

GtkWidget * ling_wallpaper_new(const char * path);

G_END_DECLS
