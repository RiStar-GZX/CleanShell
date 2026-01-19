#pragma once

#include <gtk/gtk.h>
#include <ling.h>

G_BEGIN_DECLS

#define CL_TYPE_CONTROL_ITEM (cl_control_item_get_type())
G_DECLARE_FINAL_TYPE(ClControlItem,cl_control_item,CL,CONTROL_ITEM,GtkBox)

GtkWidget * cl_control_item_new(GtkWidget * button,GtkWidget * folder);

GtkWidget * cl_control_item_get_button(ClControlItem * self);

GtkWidget * cl_control_item_get_folder(ClControlItem * self);

G_END_DECLS
