#pragma once

#include <gtk/gtk.h>
#include <lingoperate.h>
#include <lingshell.h>

G_BEGIN_DECLS

#define LING_TYPE_FIXED_VIEW_ITEM (ling_fixed_view_item_get_type())
G_DECLARE_FINAL_TYPE(LingFixedViewItem,ling_fixed_view_item,LING,FIXED_VIEW_ITEM,GtkBox)

typedef GtkWidget * (*CONTENT_COPY)(GtkWidget * content);

GtkWidget * ling_fixed_view_item_new(GtkWidget * content);

void ling_fixed_view_item_set_remove_on_drag(LingFixedViewItem * self,gboolean remove_on_drag);

void ling_fixed_view_item_set_content_copy_func(LingFixedViewItem * self,CONTENT_COPY func);

#define LING_TYPE_FIXED_VIEW (ling_fixed_view_get_type())
G_DECLARE_FINAL_TYPE(LingFixedView,ling_fixed_view,LING,FIXED_VIEW,GtkBox)

GtkWidget * ling_fixed_view_item_get_content(LingFixedViewItem * item);

void ling_fixed_view_item_get_size(LingFixedViewItem * item,uint * width,uint * height);

void ling_fixed_view_get_grid_size(LingFixedView * self,int * width,int *height);

GtkWidget * ling_fixed_view_new(uint arrange,uint column_num,uint row_num,uint column_space,uint row_space);

typedef void (*PRESS_CALLBACK)(LingFixedViewItem * item,gpointer user_data);

typedef void (*ITEM_DRAG_CALLBACK)(LingFixedViewItem * item,gpointer user_data);
typedef void (*ITEM_DROP_CALLBACK)(LingFixedViewItem * item,gpointer item_data,gpointer fv_data);

void ling_fixed_view_item_set_middle_press_cb(LingFixedViewItem * self,ITEM_DRAG_CALLBACK start,ITEM_DROP_CALLBACK end,gpointer data);

void ling_fixed_view_set_item_drop_data(LingFixedView * self,gpointer data);

void ling_fixed_view_item_set_long_press_cb(LingFixedViewItem * self,ITEM_DRAG_CALLBACK start,ITEM_DROP_CALLBACK end,gpointer data);

enum ling_fixed_view_arrange{
    LING_FIXED_VIEW_ARRANGE_FREE,
    LING_FIXED_VIEW_ARRANGE_GRID
};

void ling_fixed_view_item_set_drag_source_content(LingFixedViewItem * item);

int ling_fixed_view_item_set_grid_pos(LingFixedViewItem * item,uint column,uint row);
int ling_fixed_view_item_get_grid_pos(LingFixedViewItem * item,uint * column,uint * row);
int ling_fixed_view_item_set_free_pos(LingFixedViewItem * item,gdouble x,gdouble y);
void ling_fixed_view_item_get_free_pos(LingFixedViewItem * item,gdouble * x,gdouble * y);

GtkWidget * ling_fixed_view_get_item_by_grid_pos(LingFixedView * self,uint column,uint row);
int ling_fixed_view_item_adsorb_to_grid(LingFixedViewItem * item);

int ling_fixed_view_add_item_free(LingFixedView * fixed_view,LingFixedViewItem * item,uint width,uint height,gdouble x,gdouble y);
int ling_fixed_view_add_grid(LingFixedView * fixed_view,LingFixedViewItem * item,uint width,uint height,uint column,uint row);
GtkWidget * ling_fixed_view_add_content_grid(LingFixedView * fixed_view,GtkWidget * content,
                                             uint width,uint height,uint column,uint row);
void ling_fixed_view_remove_item(LingFixedView * self,LingFixedViewItem * item);

void ling_fixed_view_update(LingFixedView * self);

void ling_fixed_view_set_margin(LingFixedView * self,int top,int bottom,int start,int end);

//#define LING_TYPE_FIXED_VIEW_FOLDER (ling_fixed_view_folder_get_type())
//G_DECLARE_FINAL_TYPE(LingFixedViewFolder,ling_fixed_view_folder,LING,FIXED_VIEW_FOLDER,GtkBox)

G_END_DECLS
