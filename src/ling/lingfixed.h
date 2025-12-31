#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define LING_FIXED_TOP  0
#define LING_FIXED_BOTTOM  -1

typedef struct LingFixedItem{
    GtkWidget * widget;
    uint w;
    uint h;
    gdouble x;
    gdouble y;
    uint level;
}LingFixedChild;


#define LING_TYPE_FIXED (ling_fixed_get_type())
G_DECLARE_FINAL_TYPE(LingFixed,ling_fixed,LING,FIXED,GtkFixed)

GtkWidget * ling_fixed_new();

void ling_fixed_put(LingFixed * self,GtkWidget * widget,uint x,uint y,int level);

void ling_fixed_set_child_size(LingFixed * self,GtkWidget * widget,int w,int h);

LingFixedChild * ling_fixed_get_child_info(LingFixed * self,GtkWidget * widget);

void ling_fixed_move(LingFixed * self,GtkWidget * widget,gdouble x,gdouble y);

void ling_fixed_remove(LingFixed * self,GtkWidget * widget);

G_END_DECLS
