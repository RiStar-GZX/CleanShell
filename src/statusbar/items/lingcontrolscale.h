#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define LING_TYPE_CONTROL_SCALE (ling_control_scale_get_type())
G_DECLARE_FINAL_TYPE(LingControlScale,ling_control_scale,LING,CONTROL_SCALE,GtkScale)

GtkWidget * ling_control_scale_new();

G_END_DECLS
