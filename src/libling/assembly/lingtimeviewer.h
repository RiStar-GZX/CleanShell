#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define LING_TYPE_TIME_VIEWER (ling_time_viewer_get_type())
G_DECLARE_FINAL_TYPE(LingTimeViewer,ling_time_viewer,LING,TIME_VIEWER,GtkBox)

GtkWidget * ling_time_viewer_new();

void ling_time_viewer_set_time(LingTimeViewer * self,const char * time);

G_END_DECLS
