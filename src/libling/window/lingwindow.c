#include "lingwindow.h"

struct _LingWindow{
    GtkBox parent;
};

G_DEFINE_FINAL_TYPE(LingWindow,ling_window,GTK_TYPE_BOX);

static void ling_window_class_init(LingWindowClass * klass){

}

static void ling_window_init(LingWindow * self){

}

GtkWindow * ling_window_new(){

}
