#pragma once

#include <gtk/gtk.h>
#include <mobileshell.h>
#include <ling.h>
#include <desktop.h>
#include <statusbar.h>

G_BEGIN_DECLS

#define CL_TYPE_NOTIFY_CENTER (cl_notify_center_get_type())
G_DECLARE_FINAL_TYPE(ClNotifyCenter,cl_notify_center,CL,NOTIFY_CENTER,GtkBox)

GtkWidget * cl_notify_center_new();

G_END_DECLS
