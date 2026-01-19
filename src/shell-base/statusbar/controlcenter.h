#pragma once

#include <gtk/gtk.h>
#include <ling.h>
#include <mobileshell.h>
#include <desktop.h>
#include <statusbar.h>
#include <controlbutton.h>
#include <controlmedia.h>
#include <controlitem.h>

G_BEGIN_DECLS

#define CL_TYPE_CONTROL_CENTER (cl_control_center_get_type())
G_DECLARE_FINAL_TYPE(ClControlCenter,cl_control_center,CL,CONTROL_CENTER,GtkBox)

GtkWidget * cl_control_center_new(ClStatusBar * statusbar);

void cl_control_center_set_blur(ClControlCenter * self,uint px);

G_END_DECLS
