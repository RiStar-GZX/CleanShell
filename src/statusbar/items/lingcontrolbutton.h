#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define LING_TYPE_CONTROL_BUTTON (ling_control_button_get_type())
G_DECLARE_FINAL_TYPE(LingControlButton,ling_control_button,LING,CONTROL_BUTTON,GtkBox)

GtkWidget * ling_control_button_new(const char * label,const char * icon_name);

G_END_DECLS
