#pragma once

#include <gtk/gtk.h>
#include <lingoperate.h>
#include <mobileshell.h>

G_BEGIN_DECLS

#define LING_TYPE_GRID_PH (ling_grid_ph_get_type())
G_DECLARE_FINAL_TYPE(LingGridPh,ling_grid_ph,LING,GRID_PH,GtkBox)

#define LING_TYPE_GRID (ling_grid_get_type())
G_DECLARE_FINAL_TYPE(LingGrid,ling_grid,LING,GRID,GtkGrid)

GtkWidget * ling_grid_new(uint column,uint row,uint column_space,uint row_space);

int ling_grid_attach(LingGrid * self,GtkWidget * content,uint column,uint row,uint width,uint height);

GtkWidget * ling_grid_get_child_at(LingGrid * self,int column,int row);

void ling_grid_remove(LingGrid * self,int column,int row);

void ling_grid_query_child(LingGrid *self,GtkWidget *child,
                           int *column,int *row,int *width,int *height);

G_END_DECLS
