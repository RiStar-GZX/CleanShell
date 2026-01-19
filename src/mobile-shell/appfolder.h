#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define CLM_TYPE_APP_FOLDER (clm_app_folder_get_type())
G_DECLARE_FINAL_TYPE(ClmAppFolder,clm_app_folder,CLM,APP_FOLDER,GtkBox)

GtkWidget * clm_app_folder_new();

G_END_DECLS
