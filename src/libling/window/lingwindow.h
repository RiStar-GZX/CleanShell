#pragma once

#include <gtk/gtk.h>
#include <ling.h>

typedef enum{
    LING_WINDOW_GUIDE_NONE =0,
    LING_WINDOW_GUIDE_LEFT ,
    LING_WINDOW_GUIDE_RIGHT,
    LING_WINDOW_GUIDE_TOP,
    LING_WINDOW_GUIDE_BOTTOM,
}LING_WINDOW_GUIDE_SIDE;

typedef struct LingGuide LingGuide;

// typedef GtkAllocation (*LING_WINDOW_ADJUST)(GtkWidget * window,LingGuide *guide,
//                                             int width,int height,int baseline,graphene_rect_t rect);



G_BEGIN_DECLS

#define LING_TYPE_WINDOW_LAYOUT (ling_window_layout_get_type())
G_DECLARE_FINAL_TYPE(LingWindowLayout,ling_window_layout,LING,WINDOW_LAYOUT,GtkLayoutManager)

#define LING_TYPE_WINDOW (ling_window_get_type())
G_DECLARE_FINAL_TYPE(LingWindow,ling_window,LING,WINDOW,GtkBox)

GtkWidget * ling_window_new();

void ling_window_set_main(LingWindow * window,GtkWidget * widget);

void ling_window_add_guide(LingWindow * window,GtkWidget * widget,
                           LING_WINDOW_GUIDE_SIDE side,gdouble proportion);

void ling_window_remove(LingWindow * window,GtkWidget * widget);

LingOperate * ling_window_guide_enable_ani(LingWindow * window,GtkWidget * widget);

void ling_window_guide_set_show_progress(LingWindow * window,GtkWidget * widget,gdouble progress);

G_END_DECLS
