#pragma once

#include <gtk/gtk.h>
#include <mobileshell.h>
#include <desktop.h>
#include <desktopitem.h>
#include <ling.h>

G_BEGIN_DECLS

#define CLM_TYPE_APP_VIEW_PAGE (clm_app_view_page_get_type())
G_DECLARE_FINAL_TYPE(ClmAppViewPage,clm_app_view_page,CLM,APP_VIEW_PAGE,GtkBox)

GtkWidget * clm_app_view_page_new(style_info style_info);

GtkWidget * clm_app_view_page_get_fixed_view(ClmAppViewPage * self);

gboolean clm_app_view_page_add_item(ClmAppViewPage * self,GtkWidget * content,int column,int row,int width,int height);

G_END_DECLS
