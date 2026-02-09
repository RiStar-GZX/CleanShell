#pragma once

#include <gtk/gtk.h>
#include <gtk/gtkfixedlayout.h>

G_BEGIN_DECLS

typedef struct LingFixedItem LingFixedItem;

typedef GtkAllocation (*LING_FIXED_ADJUST)(GtkWidget * fixed,LingFixedItem *item,
                                            int width,int height,int baseline,graphene_rect_t rect);

struct LingFixedItem{
    GtkWidget * widget;
    uint w,h;
    gdouble x,y;
    uint level1;
    uint level2;
    LING_FIXED_ADJUST adjust;
    gpointer ex;    //暂时
};

#define LING_FIXED_TOP  0
#define LING_FIXED_BOTTOM  -1

#define LING_TYPE_FIXED_LAYOUT (ling_fixed_layout_get_type())
G_DECLARE_FINAL_TYPE(LingFixedLayout,ling_fixed_layout,LING,FIXD_LAYOUT, GtkLayoutManager)


struct _LingFixedClass{
    GtkFixedClass parent_class;
};



#define LING_TYPE_FIXED (ling_fixed_get_type())
// G_DECLARE_FINAL_TYPE(LingFixed,ling_fixed,LING,FIXED,GtkFixed)
G_DECLARE_DERIVABLE_TYPE(LingFixed, ling_fixed, LING, FIXED, GtkFixed)

// struct _LingFixed{
//     GtkFixed parent;
//     GtkLayoutManager * layout_manager;
//     GList * items_list;
// };

GtkWidget * ling_fixed_new();

GList * ling_fixed_get_items_list(LingFixed * fixed);

GtkAllocation ling_fixed_adjust_none(GtkWidget * fixed,LingFixedItem *item,
                                     int width,int height,int baseline,graphene_rect_t rect);

LingFixedItem * ling_fixed_put(LingFixed * fixed,GtkWidget * widget,LING_FIXED_ADJUST adjust,uint x,uint y,uint level1,uint level2);

LingFixedItem * ling_fixed_put_none(LingFixed * fixed,GtkWidget * widget,uint x,uint y,uint level1,uint level2);

LingFixedItem * ling_fixed_put_full(LingFixed * fixed,GtkWidget * widget,uint x,uint y,uint level1,uint level2);

void ling_fixed_set_child_size(LingFixed * fixed,GtkWidget * widget,int w,int h);

void ling_fixed_set_child_level(LingFixed * fixed,GtkWidget * widget,int level1,int level2);

LingFixedItem * ling_fixed_get_item_info(LingFixed * fixed,GtkWidget * widget);

void ling_fixed_move(LingFixed * fixed,GtkWidget * widget,gdouble x,gdouble y);

gpointer ling_fixed_remove(LingFixed * fixed,GtkWidget * widget);

void ling_fixed_item_change_adjust(LingFixed * fixed,GtkWidget * widget,LING_FIXED_ADJUST adjust);

G_END_DECLS
