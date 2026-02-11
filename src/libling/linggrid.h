#pragma once

#include <gtk/gtk.h>
#include <ling.h>

G_BEGIN_DECLS

#define LING_GRID_ASPECT_NONE 0;

#define LING_TYPE_GRID (ling_grid_get_type())
G_DECLARE_FINAL_TYPE(LingGrid,ling_grid,LING,GRID,LingFixed)

typedef void (*LING_GRID_RELEASE_ANI)(GtkWidget * item);

typedef void (*LING_GRID_RELEASE_END)(GtkWidget * item);

GtkWidget * ling_grid_new(uint column,uint row,uint column_space,uint row_space);//,uint start_space,uint top_space);

int ling_grid_attach(LingGrid * self,GtkWidget * content,uint column,uint row,uint width,uint height);

GtkWidget * ling_grid_get_child_at(LingGrid * self,int column,int row);

void ling_grid_remove_by_pos(LingGrid * self,int column,int row);

void ling_grid_remove(LingGrid * grid,GtkWidget * widget);

void ling_grid_query_child(LingGrid *self,GtkWidget *child,
                           int *column,int *row,int *width,int *height);

void ling_grid_set_aspect(LingGrid * grid,gdouble ratio);

void ling_grid_set_drop_target(LingGrid * grid,GType type,GdkDragAction action);

void ling_grid_add_frame(LingGrid * grid,uint column,uint row,uint w,uint h);

G_END_DECLS
