#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS


#define LING_SYS_INFO_UPDATE_TIME 500

typedef void (*LING_SYS_INFO_UPDATE_CALLBACK)(GList * list,void * user_data);

#define LING_TYPE_SYS_INFO_UPDATER (ling_sys_info_updater_get_type())
G_DECLARE_FINAL_TYPE(LingSysInfoUpdater,ling_sys_info_updater,LING,SYS_INFO_UPDATER,GObject)

LingSysInfoUpdater * ling_sys_info_updater_new();

void ling_sys_info_updater_add_type(LingSysInfoUpdater * self,uint id,LING_SYS_INFO_UPDATE_CALLBACK func,gpointer user_data);

void ling_sys_info_updater_add_obj(LingSysInfoUpdater * self,uint type,gpointer obj);

LingSysInfoUpdater * ling_sys_info_updater_new();

G_END_DECLS
