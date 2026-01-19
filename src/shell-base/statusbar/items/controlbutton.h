#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define CL_TYPE_CONTROL_BUTTON (cl_control_button_get_type())
G_DECLARE_FINAL_TYPE(ClControlButton,cl_control_button,CL,CONTROL_BUTTON,GtkBox)

GtkWidget * cl_control_button_new(const char * label,const char * icon_name);

GtkWidget * cl_control_button_get_sub_button(ClControlButton * self);

G_END_DECLS
