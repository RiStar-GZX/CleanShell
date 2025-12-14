#pragma once

#include <gtk/gtk.h>
#include <lingoperate.h>

G_BEGIN_DECLS

enum{
    LING_OVERLAY_OP_SWIPE_LEFT,
    LING_OVERLAY_OP_SWIPE_RIGHT,
    LING_OVERLAY_OP_SWIPE_UP,
    LING_OVERLAY_OP_SWIPE_DOWN,
};

enum{
    LING_OVERLAY_ANIMATE_FADE,
};

#define LING_TYPE_OVERLAY (ling_overlay_get_type())
G_DECLARE_FINAL_TYPE(LingOverlay,ling_overlay,LING,OVERLAY,GtkBox)

GtkWidget * ling_overlay_new();

int ling_overlay_add_layer(LingOverlay * self,GtkWidget * widget,uint8_t level);

void ling_overlay_add_switch(LingOverlay * overlay_m,uint lay_main,uint op_type,uint ani_type,LingOverlay * overlay_s,uint lay_sub);

G_END_DECLS
