#include "store.h"

static void main_clicked(GtkButton * button,LingOperate *op){
    ling_operate_emit(op,LING_ACTION_ANIMATE,NULL,TRUE,EMIT_FINISH_DIR_REVERSE);
}

typedef struct{
    GString * name;
}item_data;

typedef struct{
    LingWindow * window;
    GtkWidget * side_widget;
    uint pos;
    LING_WINDOW_SIDE side;
}click_data;

static void item_clicked(GtkButton * button, click_data * d){
    g_print("d.pos:%d\n",d->pos);
    ling_window_page_set_pos(d->window,d->pos);
    ling_window_side_set_side(d->window,d->side_widget,d->side);
}

void item_load(LingWindow * window,GtkWidget * widget,GList * pages,gpointer user_data){
    int i=1;
    GtkWidget * w = gtk_widget_get_first_child(widget);
    while(w!=NULL){
        gtk_box_remove(GTK_BOX(widget),w);
        w = gtk_widget_get_first_child(widget);
    }
    for(GList * l = pages;l!=NULL;l=l->next,i++){
        WindowPage * page = l->data;
        item_data * data = page->user_data;
        GtkWidget * w = gtk_button_new_with_label(data->name->str);
        gtk_widget_set_hexpand(w,TRUE);
        gtk_box_append(GTK_BOX(widget),w);
        click_data * d = g_malloc(sizeof(click_data));
        d->window = window;
        d->side_widget = widget;
        d->pos = i;
        switch (i) {
        case 1:
            d->side = LING_WINDOW_SIDE_LEFT;
            break;
        case 2:
            d->side = LING_WINDOW_SIDE_RIGHT;
            break;
        case 3:
            d->side = LING_WINDOW_SIDE_TOP;
            break;
        case 4:
            d->side = LING_WINDOW_SIDE_BOTTOM;
            break;

        default:
            break;
        }
        g_signal_connect(w,"clicked",G_CALLBACK(item_clicked),d);
    }
}

// static void add_side(LingWindow * win,LING_WINDOW_SIDE s,GtkWidget* button){
//     //GtkWidget * side = gtk_button_new_with_label("side");
//     GtkWidget * side = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
//     ling_window_add_side(LING_WINDOW(win),side,s,20);

//     ling_window_side_set_guide(win,side,item_load,NULL);
//     ling_window_side_guide_update(win);
//     LingOperate * op = ling_window_side_enable_ani(LING_WINDOW(win),side);
//     g_signal_connect(button,"clicked",G_CALLBACK(main_clicked),op);
// }

GtkWidget * ling_store_new(){
    GtkWidget * button;
    GtkWidget * fixed = ling_fixed_new();

    GtkWidget * win = ling_window_new(LING_WINDOW_TYPE_MULTI);
    item_data * data = g_malloc0(sizeof(item_data));
    data->name = g_string_new("sdas");

    GtkWidget * side = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
    ling_window_add_side(LING_WINDOW(win),side,LING_WINDOW_SIDE_BOTTOM,40);
    LingOperate * op = ling_window_side_enable_ani(LING_WINDOW(win),side);
    ling_window_side_set_guide(LING_WINDOW(win),side,item_load,NULL);

    button = gtk_button_new_with_label("1");
    ling_window_add_page(LING_WINDOW(win),button,data);
    g_signal_connect(button,"clicked",G_CALLBACK(main_clicked),op);
    button = gtk_button_new_with_label("2");
    ling_window_add_page(LING_WINDOW(win),button,data);
    g_signal_connect(button,"clicked",G_CALLBACK(main_clicked),op);
    ling_window_side_guide_update(LING_WINDOW(win));
    button = gtk_button_new_with_label("3");
    ling_window_add_page(LING_WINDOW(win),button,data);
    g_signal_connect(button,"clicked",G_CALLBACK(main_clicked),op);
    button = gtk_button_new_with_label("4");
    ling_window_add_page(LING_WINDOW(win),button,data);
    g_signal_connect(button,"clicked",G_CALLBACK(main_clicked),op);
    ling_window_side_guide_update(LING_WINDOW(win));



    // GtkWidget * win = ling_window_new(LING_WINDOW_TYPE_SINGAL);
    // ling_fixed_put_none(LING_FIXED(fixed),button,200,400,0,0);
    // ling_fixed_set_child_size(LING_FIXED(fixed),button,30,30);
    // ling_window_set_main(LING_WINDOW(win),fixed);



    return win;
}
