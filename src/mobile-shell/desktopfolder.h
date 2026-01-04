#pragma once

#include <gtk/gtk.h>
#include <ling.h>

G_BEGIN_DECLS

#define CLM_TYPE_DESKTOP_FOLDER (clm_desktop_folder_get_type())
G_DECLARE_FINAL_TYPE(ClmDesktopFolder,clm_desktop_folder,CLM,DESKTOP_FOLDER,LingFixed)

GtkWidget * clm_desktop_folder_new();

void clm_desktop_folder_show(ClmDesktopFolder * self,LingGrid * grid,GtkWidget * folder,uint column,uint row,uint width,uint height);

void clm_desktop_folder_set_size(ClmDesktopFolder * self,int w,int h);

G_END_DECLS
