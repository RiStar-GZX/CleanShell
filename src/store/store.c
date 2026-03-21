#include "store.h"

static void main_clicked(GtkButton * button,LingOperate *op){
    ling_operate_emit(op,LING_ACTION_ANIMATE,NULL,TRUE,EMIT_FINISH_DIR_REVERSE);
}

static void add_side(LingWindow * win,LING_WINDOW_GUIDE_SIDE s,GtkWidget* button){
    GtkWidget * side = gtk_button_new_with_label("side");
    ling_window_add_guide(LING_WINDOW(win),side,s,30);
    LingOperate * op = ling_window_guide_enable_ani(LING_WINDOW(win),side);
    g_signal_connect(button,"clicked",G_CALLBACK(main_clicked),op);
}

GtkWidget * ling_store_new(){
    GtkWidget * win = ling_window_new();
    GtkWidget * button = gtk_button_new();
    GtkWidget * fixed = ling_fixed_new();
    ling_fixed_put_none(LING_FIXED(fixed),button,200,400,0,0);
    ling_fixed_set_child_size(LING_FIXED(fixed),button,30,30);
    ling_window_set_main(LING_WINDOW(win),fixed);

    add_side(LING_WINDOW(win),LING_WINDOW_GUIDE_TOP,button);
    // add_side(LING_WINDOW(win),LING_WINDOW_GUIDE_BOTTOM,button);
    // add_side(LING_WINDOW(win),LING_WINDOW_GUIDE_LEFT,button);
    // add_side(LING_WINDOW(win),LING_WINDOW_GUIDE_RIGHT,button);

    // GtkWidget * side = gtk_button_new_with_label("side");
    // ling_window_add_guide(LING_WINDOW(win),side,
    //                       LING_WINDOW_GUIDE_RIGHT,30);
    // LingOperate * op = ling_window_guide_enable_ani(LING_WINDOW(win),side);
    // g_signal_connect(button,"clicked",G_CALLBACK(main_clicked),op);

    return win;
}
