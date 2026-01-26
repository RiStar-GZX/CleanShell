#pragma once

#include <gtk/gtk.h>
#include <ling.h>

G_BEGIN_DECLS

typedef GtkWidget *(*ITEM_DRAG_START_CB)(GtkWidget * widget,gpointer user_data);   //COPY

typedef void (*ITEM_DRAG_ANI_CB)(GtkWidget * widget,gdouble progress,int sx,int sy,int tx,int ty);
//类似grid变化的在ani中写emit

typedef void (*ITEM_DRAG_END_CB)(GtkWidget * widget,gpointer user_data);

#define LING_TYPE_DRAG_FACE (ling_drag_face_get_type())
G_DECLARE_FINAL_TYPE(LingDragFace,ling_drag_face,LING,DRAG_FACE,LingFixed)

LingOperate * ling_drag_item_add_operate(LingDragFace * face,GtkWidget * widget,LING_ACTION action,
                                        ITEM_DRAG_START_CB start,gpointer start_data,
                                        ITEM_DRAG_ANI_CB ani,gpointer ani_data,
                                        ITEM_DRAG_END_CB end,gpointer end_data);

G_END_DECLS
