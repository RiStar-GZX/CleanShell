#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define LING_TYPE_FOLDER (ling_folder_get_type())
G_DECLARE_FINAL_TYPE(LingFolder,ling_folder,LING,FOLDER,GtkBox)

GtkWidget * ling_folder_new(const char * title,GtkWidget * content);

#define IN_FOLDER 0
#define OUT_FOLDER 1

G_END_DECLS
