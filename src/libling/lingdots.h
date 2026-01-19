#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

enum dots_mode{
    DOTS_MODE_SINGAL,
    DOTS_MODE_MULTI,
    N_DOTS_MODE
};

#define LING_TYPE_DOTS (ling_dots_get_type())
G_DECLARE_FINAL_TYPE(LingDots,ling_dots,LING,DOTS,GtkBox)

#define LING_TYPE_DOT (ling_dot_get_type())
G_DECLARE_FINAL_TYPE(LingDot,ling_dot,LING,DOT,GtkButton)

GtkWidget * ling_dots_new(uint num,uint mode);

void ling_dots_set_num(LingDots * dots,uint num);

void ling_dots_set_mode(LingDots * dots,uint mode);

void ling_dots_ativate(LingDots * dots,uint pos);

int ling_dots_num(LingDots * dots);

G_END_DECLS
