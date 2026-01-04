#pragma once

#include <gtk/gtk.h>
#include <ling.h>
#include <mobileshell.h>
#include <desktopfolder.h>

G_BEGIN_DECLS

enum {
    CLM_DESKTOP_ITEM_APP = 0,
    CLM_DESKTOP_ITEM_FOLDER,
    //桌面小组件
};

#define CLM_TYPE_DESKTOP_ITEM (clm_desktop_item_get_type())
G_DECLARE_FINAL_TYPE(ClmDesktopItem,clm_desktop_item,CLM,DESKTOP_ITEM,GtkBox)

void clm_desktop_item_set_label_visible(ClmDesktopItem * self,gboolean visible);

GtkWidget * clm_desktop_item_app_new(app_info * app,guint icon_size,gboolean label_visible);

app_info * clm_desktop_item_get_app_info(ClmDesktopItem * self);

void clm_desktop_item_app_set_runable(ClmDesktopItem * self,gboolean runable);

void clm_desktop_item_app_set_icon_size(ClmDesktopItem * self,int size);

void clm_desktop_item_folder_set_app_runable(ClmDesktopItem * self,gboolean runable);

void clm_desktop_item_folder_set_app_icon_size(ClmDesktopItem * self,int size);

GtkWidget * clm_desktop_item_folder_new(ClmDesktopFolder * folderlayer,
                                       uint column,uint row,GList * applist,
                                       const char * folder_name,gboolean label_visible);

G_END_DECLS
