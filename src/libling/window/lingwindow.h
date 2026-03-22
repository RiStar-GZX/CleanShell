#pragma once

#include <gtk/gtk.h>
#include <ling.h>


G_BEGIN_DECLS

typedef enum{
    LING_WINDOW_TYPE_SINGAL = 0,
    LING_WINDOW_TYPE_MULTI,
}LING_WINDOW_TYPE;

typedef enum{
    LING_WINDOW_SIDE_NONE =0,
    LING_WINDOW_SIDE_LEFT ,
    LING_WINDOW_SIDE_RIGHT,
    LING_WINDOW_SIDE_TOP,
    LING_WINDOW_SIDE_BOTTOM,
}LING_WINDOW_SIDE;

typedef struct{
    GtkWidget * widget; //在main上显示的
    gpointer user_data; //存储该页面的信息
}WindowPage;

#define LING_TYPE_WINDOW_LAYOUT (ling_window_layout_get_type())
G_DECLARE_FINAL_TYPE(LingWindowLayout,ling_window_layout,LING,WINDOW_LAYOUT,GtkLayoutManager)

#define LING_TYPE_WINDOW (ling_window_get_type())
G_DECLARE_FINAL_TYPE(LingWindow,ling_window,LING,WINDOW,GtkWidget)

typedef void (*GUIDE_LOAD)(LingWindow * window,GtkWidget * widget,GList * pages,gpointer user_data);

GtkWidget * ling_window_new(LING_WINDOW_TYPE type);

void ling_window_set_main(LingWindow * window,GtkWidget * widget);

void ling_window_add_page(LingWindow * window,GtkWidget * widget,gpointer user_data);

void ling_window_remove_page(LingWindow * window,GtkWidget * widget);

void ling_window_page_set_pos(LingWindow * window,uint pos);

void ling_window_side_guide_update(LingWindow * window);

void ling_window_add_side(LingWindow * window,GtkWidget * widget,
                           LING_WINDOW_SIDE side,gdouble proportion);

void ling_window_side_set_side(LingWindow * window,GtkWidget * widget,LING_WINDOW_SIDE side);

void ling_window_remove(LingWindow * window,GtkWidget * widget);

LingOperate * ling_window_side_enable_ani(LingWindow * window,GtkWidget * widget);

void ling_window_side_set_show_progress(LingWindow * window,GtkWidget * widget,gdouble progress);

void ling_window_side_set_guide(LingWindow * window, GtkWidget * widget, GUIDE_LOAD load, gpointer load_data);

void ling_window_side_guide_update(LingWindow * window);

G_END_DECLS
