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

G_END_DECLS
