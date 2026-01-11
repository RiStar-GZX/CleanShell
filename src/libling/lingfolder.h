#pragma once

#include <gtk/gtk.h>
#include <lingfixed.h>
#include <lingoperate.h>
#include <mobileshell.h>

G_BEGIN_DECLS

typedef gboolean (*FOLDER_OPEN_CB)(GtkWidget * widget,gdouble * x,gdouble * y,gpointer user_data);

typedef void (*FOLDER_ANI_CB)(GtkWidget * widget,gdouble progress,gpointer user_data);

typedef void (*FOLDER_END_CB)(GtkWidget * widget,gpointer user_data);

#define LING_FOLDER_NOT_OPEN LING_OPERATE_ANIMATION_DIR_BACK
#define LING_FOLDER_OPEN    LING_OPERATE_ANIMATION_DIR_FORWARD

#define LING_TYPE_FOLDER (ling_folder_get_type())
G_DECLARE_FINAL_TYPE(LingFolder,ling_folder,LING,FOLDER,LingFixed)

GtkWidget * ling_folder_new();

void ling_folder_set_pos(LingFolder * self,int x,int y);

LingOperate * ling_folder_operate(LingFolder * self,GtkWidget * widget,uint action_type,
                                 FOLDER_OPEN_CB open,gpointer open_data,
                                 FOLDER_ANI_CB ani,gpointer ani_data,
                                 FOLDER_END_CB open_finish,gpointer open_finish_data,
                                 FOLDER_END_CB close_finish,gpointer close_finish_data);

G_END_DECLS
