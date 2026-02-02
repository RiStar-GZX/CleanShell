#pragma once

#include <gtk/gtk.h>
#include <gtk/gtkfixedlayout.h>

G_BEGIN_DECLS

typedef struct LingFixedItemInfo LingFixedItemInfo;

typedef GtkAllocation (*LING_FIXED_ADJUST)(GtkWidget * fixed,LingFixedItemInfo *item,
                                            int width,int height,int baseline,graphene_rect_t rect);

struct LingFixedItemInfo{
    GtkWidget * widget;
    uint w,h;
    gdouble x,y;
    uint level1;
    uint level2;
    LING_FIXED_ADJUST adjust;
};

#define LING_FIXED_TOP  0
#define LING_FIXED_BOTTOM  -1

#define LING_TYPE_FIXED_LAYOUT (ling_fixed_layout_get_type())
G_DECLARE_FINAL_TYPE(LingFixedLayout,ling_fixed_layout,LING,FIXD_LAYOUT, GtkLayoutManager)

struct _LingFixed{
    GtkFixed parent;
    GtkLayoutManager * layout_manager;
    GList * items_list;
};

struct _LingFixedClass{
    GtkFixedClass parent_class;
};

#define LING_TYPE_FIXED (ling_fixed_get_type())
G_DECLARE_FINAL_TYPE(LingFixed,ling_fixed,LING,FIXED,GtkFixed)


GtkWidget * ling_fixed_new();

GtkAllocation ling_fixed_adjust_none(GtkWidget * fixed,LingFixedItemInfo *item,
                                     int width,int height,int baseline,graphene_rect_t rect);

void ling_fixed_put(LingFixed * fixed,GtkWidget * widget,LING_FIXED_ADJUST adjust,uint x,uint y,uint level1,uint level2);

void ling_fixed_put_none(LingFixed * fixed,GtkWidget * widget,uint x,uint y,uint level1,uint level2);

void ling_fixed_put_full(LingFixed * fixed,GtkWidget * widget,uint x,uint y,uint level1,uint level2);

void ling_fixed_set_child_size(LingFixed * self,GtkWidget * widget,int w,int h);

void ling_fixed_set_child_level(LingFixed * fixed,GtkWidget * widget,int level1,int level2);

LingFixedItemInfo * ling_fixed_get_item_info(LingFixed * self,GtkWidget * widget);

void ling_fixed_move(LingFixed * self,GtkWidget * widget,gdouble x,gdouble y);

void ling_fixed_remove(LingFixed * self,GtkWidget * widget);

G_END_DECLS
