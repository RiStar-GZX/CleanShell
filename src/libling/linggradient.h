#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define LING_TYPE_GRADIENT (ling_gradient_get_type())
G_DECLARE_FINAL_TYPE(LingGradient,ling_gradient,LING,GRADIENT,GtkBox)

GtkWidget * ling_gradient_new(GtkWidget * top,GtkWidget * bottom);

void ling_gradient_progress(LingGradient * gradient,gdouble progress);

GtkWidget * ling_gradient_get_top(LingGradient * gradient);

GtkWidget * ling_gradient_get_bottom(LingGradient * gradient);

G_END_DECLS
