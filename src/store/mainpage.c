#include "mainpage.h"

struct _OmagMainPage{
    GtkBox parent;
    LingWindow * page;
    GtkBox * main_box;
    GtkButton * tdl_show_button;

    GtkBox * side;
    GtkButton * side_hide_button;
    LingOperate * side_ani;
};

G_DEFINE_FINAL_TYPE(OmagMainPage,omag_main_page,GTK_TYPE_BOX);

static void omag_main_page_class_init(OmagMainPageClass * klass){

}

static void show_side_clicked(GtkButton * button,LingOperate * op){
    ling_operate_emit_end(op,LING_ACTION_ANIMATE,NULL,TRUE);
}

static void hide_side_clicked(GtkButton * button,LingOperate * op){
    ling_operate_emit_start(op,LING_ACTION_ANIMATE,NULL,TRUE);
}


static void omag_main_page_init(OmagMainPage * self){
    self->page = LING_WINDOW(ling_window_new(LING_WINDOW_TYPE_SINGAL));
    gtk_widget_set_parent(GTK_WIDGET(self->page),GTK_WIDGET(self));

    GtkBuilder * builder = gtk_builder_new_from_file("../../ui/store_page_main.ui");
    self->main_box = GTK_BOX(gtk_builder_get_object(builder,"main_box"));
    self->tdl_show_button = GTK_BUTTON(gtk_builder_get_object(builder,"tdl_show_button"));
    ling_window_set_main(self->page,GTK_WIDGET(self->main_box));


    self->side = GTK_BOX(gtk_builder_get_object(builder,"side_box"));
    self->side_hide_button = GTK_BUTTON(gtk_builder_get_object(builder,"side_hide_button"));
    ling_window_add_side(self->page,GTK_WIDGET(self->side),LING_WINDOW_SIDE_RIGHT,30);
    ling_window_side_set_show_progress(self->page,GTK_WIDGET(self->side),0);
    self->side_ani =ling_window_side_enable_ani(self->page,GTK_WIDGET(self->side));
    g_signal_connect(self->tdl_show_button,"clicked",G_CALLBACK(show_side_clicked),self->side_ani);
    g_signal_connect(self->side_hide_button,"clicked",G_CALLBACK(hide_side_clicked),self->side_ani);

    g_object_unref(builder);
}

GtkWidget * omag_main_page_new(){
    return g_object_new(OMAG_TYPE_MAIN_PAGE,NULL);
}
