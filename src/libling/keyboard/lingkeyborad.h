#pragma once

#include <gtk/gtk.h>
#include <mobileshell.h>

G_BEGIN_DECLS

#define LING_TYPE_KEYBOARD (ling_keyboard_get_type())
G_DECLARE_FINAL_TYPE(LingKeyboard,ling_keyboard,LING,KEYBOARD,GtkBox)

GtkWidget * ling_keyboard_new();

G_END_DECLS
