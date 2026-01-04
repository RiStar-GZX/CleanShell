#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define CL_TYPE_FOLDER (cl_folder_get_type())
G_DECLARE_FINAL_TYPE(ClFolder,cl_folder,CL,FOLDER,GtkBox)

GtkWidget * cl_folder_new(const char * title,GtkWidget * content);

#define IN_FOLDER 0
#define OUT_FOLDER 1

G_END_DECLS
