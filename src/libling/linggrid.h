#pragma once

#include <gtk/gtk.h>
#include <ling.h>

G_BEGIN_DECLS

enum{
    LING_FIXED_ITEM_TYPE_GRID = LING_FIXED_ITEM_TYPE_FIXED +1,
};

#define LING_GRID_ASPECT_NONE 0;

#define LING_TYPE_GRID (ling_grid_get_type())
G_DECLARE_FINAL_TYPE(LingGrid,ling_grid,LING,GRID,LingFixed)

typedef void (*LING_GRID_RELEASE_ANI)(GtkWidget * item);

typedef void (*LING_GRID_RELEASE_END)(GtkWidget * item);

GtkWidget * ling_grid_new(uint column,uint row,uint column_space,uint row_space);//,uint start_space,uint top_space);

int ling_grid_attach(LingGrid * self,GtkWidget * content,int column,int row,int width,int height);

void ling_grid_attach_free(LingGrid * grid,GtkWidget * widget,gdouble x,gdouble y);

GtkWidget * ling_grid_get_child_at(LingGrid * self,int column,int row);

void ling_grid_remove_by_pos(LingGrid * self,int column,int row);

void ling_grid_remove(LingGrid * grid,GtkWidget * widget);

int ling_grid_query_child(LingGrid *grid,GtkWidget *child,
                          int *column,int *row,int *width,int *height);

void ling_grid_set_aspect(LingGrid * grid,gdouble ratio);

void ling_grid_set_drop_target(LingGrid * grid,GType *type,uint type_num,GdkDragAction action);

void ling_grid_set_drop_able(LingGrid * grid,gboolean drop_able);

void ling_grid_add_frame(LingGrid * grid,uint column,uint row,uint w,uint h);

G_END_DECLS
