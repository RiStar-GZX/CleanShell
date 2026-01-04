#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define CL_TYPE_CONTROL_SCALE (cl_control_scale_get_type())
G_DECLARE_FINAL_TYPE(ClControlScale,cl_control_scale,CL,CONTROL_SCALE,GtkScale)

GtkWidget * cl_control_scale_new();

G_END_DECLS
