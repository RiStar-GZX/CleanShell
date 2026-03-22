#include "store.h"

static void main_clicked(GtkButton * button,LingOperate *op){
    ling_operate_emit(op,LING_ACTION_ANIMATE,NULL,TRUE,EMIT_FINISH_DIR_REVERSE);
}

typedef struct{
    GString * name;
    GString * icon_name;
}item_data;

typedef struct{
    LingWindow * window;
    GtkWidget * side_widget;
    uint pos;
    LING_WINDOW_SIDE side;
}click_data;

static void item_clicked(GtkButton * button, click_data * d){
    ling_window_page_set_pos(d->window,d->pos);
    //ling_window_side_set_side(d->window,d->side_widget,d->side);
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
        GtkWidget * item = gtk_button_new();//_with_label(data->name->str);
        GtkWidget * image = gtk_image_new_from_icon_name(data->icon_name->str);
        gtk_image_set_pixel_size(GTK_IMAGE(image),64);
        gtk_widget_set_parent(image,item);
        //gtk_button_set_icon_name(GTK_BUTTON(item),data->icon_name->str);
        gtk_widget_set_hexpand(item,TRUE);
        //gtk_widget_set_vexpand(item,TRUE);

        gtk_box_append(GTK_BOX(widget),item);

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
        g_signal_connect(item,"clicked",G_CALLBACK(item_clicked),d);
    }
}

static void add_page(LingWindow * window,GtkWidget * widget,LingOperate * op,const char * name,const char * icon_name){
    item_data * data = g_malloc0(sizeof(item_data));
    data->name = g_string_new(name);
    data->icon_name = g_string_new(icon_name);
    ling_window_add_page(window,widget,data);
    g_signal_connect(widget,"clicked",G_CALLBACK(main_clicked),op);
}

GtkWidget * ling_store_new(){
    GtkWidget * button;
    GtkWidget * fixed = ling_fixed_new();

    GtkWidget * win = ling_window_new(LING_WINDOW_TYPE_MULTI);

    GtkWidget * side = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    ling_window_add_side(LING_WINDOW(win),side,LING_WINDOW_SIDE_LEFT,15);
    LingOperate * op = ling_window_side_enable_ani(LING_WINDOW(win),side);
    ling_window_side_set_guide(LING_WINDOW(win),side,item_load,NULL);

    add_page(LING_WINDOW(win),gtk_button_new_with_label("1"),op,"1","firefox");
    add_page(LING_WINDOW(win),gtk_button_new_with_label("2"),op,"2","spotify");
    add_page(LING_WINDOW(win),gtk_button_new_with_label("3"),op,"3","vscode");
    add_page(LING_WINDOW(win),gtk_button_new_with_label("4"),op,"4","konsole");

    ling_window_side_guide_update(LING_WINDOW(win));

    // GtkWidget * win = ling_window_new(LING_WINDOW_TYPE_SINGAL);
    // ling_fixed_put_none(LING_FIXED(fixed),button,200,400,0,0);
    // ling_fixed_set_child_size(LING_FIXED(fixed),button,30,30);
    // ling_window_set_main(LING_WINDOW(win),fixed);

    return win;
}
