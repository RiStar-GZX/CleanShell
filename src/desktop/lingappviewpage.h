#pragma once

#include <gtk/gtk.h>
#include <lingshell.h>
#include <lingdesktop.h>
#include <lingappitem.h>
#include <linggrid.h>
#include <lingdatasaver.h>

G_BEGIN_DECLS

#define LING_TYPE_APP_VIEW_PAGE (ling_app_view_page_get_type())
G_DECLARE_FINAL_TYPE(LingAppViewPage,ling_app_view_page,LING,APP_VIEW_PAGE,GtkBox)

GtkWidget * ling_app_view_page_new(style_info style_info);

//void ling_app_view_page_bind(LingAppViewPage * self,GList * app_list,share_info * drag);

//void ling_app_view_page_set_style(LingAppViewPage * page,drag_info * drag,style_info * style);

GtkWidget * ling_app_view_page_get_fixed_view(LingAppViewPage * self);

gboolean ling_app_view_page_add_item(LingAppViewPage * self,GtkWidget * content,int column,int row,int width,int height);

G_END_DECLS
