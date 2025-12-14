#pragma once

#include <gtk/gtk.h>
#include <lingshell.h>
#include <lingnotifyitem.h>
#include <lingdesktop.h>
#include <lingstatusbar.h>

G_BEGIN_DECLS

#define LING_TYPE_NOTIFY_CENTER (ling_notify_center_get_type())
G_DECLARE_FINAL_TYPE(LingNotifyCenter,ling_notify_center,LING,NOTIFY_CENTER,GtkBox)

GtkWidget * ling_notify_center_new();

G_END_DECLS
