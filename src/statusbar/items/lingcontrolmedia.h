#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define LING_TYPE_CONTROL_MEDIA (ling_control_media_get_type())
G_DECLARE_FINAL_TYPE(LingControlMedia,ling_control_media,LING,CONTROL_MEDIA,GtkButton)

GtkWidget * ling_control_media_new();

G_END_DECLS
