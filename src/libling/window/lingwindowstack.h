#pragma once

#include <gtk/gtk.h>
#include <ling.h>

G_BEGIN_DECLS

typedef enum{
    LING_WINDOW_STACK_ANI_NONE = 0,
    LING_WINDOW_STACK_ANI_LEFT,
    LING_WINDOW_STACK_ANI_RIGHT,
    LING_WINDOW_STACK_ANI_TOP,
    LING_WINDOW_STACK_ANI_BOTTOM,
}LING_WINDOW_STACK_ANI;

#define LING_TYPE_WINDOW_STACK_LAYOUT (ling_window_stack_layout_get_type())
G_DECLARE_FINAL_TYPE(LingWindowStackLayout,ling_window_stack_layout,LING,WINDOW_STACK_LAYOUT,GtkLayoutManager)

#define LING_TYPE_WINDOW_STACK (ling_window_stack_get_type())
G_DECLARE_FINAL_TYPE(LingWindowStack,ling_window_stack,LING,WINDOW_STACK,GtkWidget)

GtkWidget * ling_window_stack_new();

void ling_window_stack_add_window(LingWindowStack * stack,GtkWidget * window);

void ling_window_stack_remove_window(LingWindowStack * stack,GtkWidget * window);

int ling_window_stack_push(LingWindowStack * stack,GtkWidget * window);

int ling_window_stack_pop(LingWindowStack * stack);

void ling_window_stack_set_ani(LingWindowStack * stack,
                               GtkWidget * window,LING_WINDOW_STACK_ANI ani_type);

void ling_window_stack_push_ani(LingWindowStack * stack,GtkWidget * window);

void ling_window_stack_pop_ani(LingWindowStack * stack);

G_END_DECLS
