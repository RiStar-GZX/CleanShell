#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define CL_TYPE_CONTROL_MEDIA (cl_control_media_get_type())
G_DECLARE_FINAL_TYPE(ClControlMedia,cl_control_media,CL,CONTROL_MEDIA,GtkButton)

GtkWidget * cl_control_media_new();

G_END_DECLS
