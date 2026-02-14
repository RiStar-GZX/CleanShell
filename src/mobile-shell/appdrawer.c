#include "appdrawer.h"
#include <desktop.h>

struct _ClmAppDrawer{
    GtkBox parent;
    GtkWidget * sw; //scrolledwindow
    GtkWidget * drawer; //l
    LingOperate * op;
    int timer_id;
};

G_DEFINE_FINAL_TYPE(ClmAppDrawer,clm_app_drawer,GTK_TYPE_BOX)


void clm_app_drawer_class_init(ClmAppDrawerClass * klass){

}

void clm_app_drawer_init(ClmAppDrawer * self){
    gtk_orientable_set_orientation(GTK_ORIENTABLE(self),GTK_ORIENTATION_VERTICAL);
    self->sw = gtk_scrolled_window_new();
    gtk_box_append(GTK_BOX(self),self->sw);
    gtk_widget_add_css_class(GTK_WIDGET(self),"app-drawer");

    self->op = ling_operate_add(shell->controler,CLM_DESKTOP_DRAWER_OP_NAME,self);
    //ling_operate_set_force_run(self->op,TRUE);
}


gboolean clm_app_drawer_add_item(ClmAppDrawer *self,app_info * info,
                                  int column,int row,int width,int height){
    GtkWidget * item =clm_desktop_item_app_new(info,64,TRUE);
    clm_desktop_item_app_set_runable(CLM_DESKTOP_ITEM(item),TRUE);
    LingOperate * op = clm_desktop_item_get_operate(CLM_DESKTOP_ITEM(item));
    ling_operate_emit_connect(op,LING_ACTION_CLICK,LING_OPERATE_EMIT_AT_RELEASE,self->op,LING_ACTION_FINISH_E,NULL);
    ling_grid_attach(LING_GRID(self->drawer),item,column,row,width,height);
    return 1;
}


GtkWidget * clm_app_drawer_new(){
    ClmAppDrawer * self = LING_APP_DRAWER(g_object_new(CLM_TYPE_APP_DRAWER,NULL));

    int column_space=30;
    int row_space=30;
    self->drawer = ling_grid_new(5,10,column_space,row_space);
    ling_grid_set_aspect(LING_GRID(self->drawer),1);
    gtk_widget_set_margin_top(self->drawer,row_space);
    gtk_widget_set_margin_bottom(self->drawer,row_space);
    gtk_widget_set_margin_start(self->drawer,column_space);
    gtk_widget_set_margin_end(self->drawer,column_space);

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(self->sw),self->drawer);
    return GTK_WIDGET(self);
}

