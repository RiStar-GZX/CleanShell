#pragma once

#include <gtk/gtk.h>
#include <ling.h>

G_BEGIN_DECLS

#define LING_TYPE_DY_GRID_LAYOUT (ling_dy_grid_layout_get_type())
G_DECLARE_FINAL_TYPE(LingDyGridLayout,ling_dy_grid_layout,LING,DY_GRID_LAYOUT,GtkLayoutManager)

#define LING_TYPE_DY_GRID (ling_dy_grid_get_type())
G_DECLARE_FINAL_TYPE(LingDyGrid,ling_dy_grid,LING,DY_GRID,GtkWidget)

GtkWidget * ling_dy_grid_new(int item_w,int item_h,int space_min,int space);

void ling_dy_grid_add_item(LingDyGrid * grid,GtkWidget * item);

void ling_dy_grid_remove_item(LingDyGrid * grid,GtkWidget * item);

G_END_DECLS

