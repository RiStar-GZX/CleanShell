#pragma once

#include <gtk/gtk.h>
#include <lingoperate.h>
#include <mobileshell.h>

G_BEGIN_DECLS

#define LING_TYPE_NOTIFY_VIEWER (ling_notify_viewer_get_type())
G_DECLARE_FINAL_TYPE(LingNotifyViewer,ling_notify_viewer,LING,NOTIFY_VIEWER,GtkBox)

GtkWidget * ling_notify_view_new();

G_END_DECLS
