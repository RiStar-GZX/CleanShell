#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define LING_TYPE_WINDOW (ling_window_get_type())
G_DECLARE_FINAL_TYPE(LingWindow,ling_window,LING,WINDOW,GtkBox)

GtkWindow * ling_window_new();

G_END_DECLS
