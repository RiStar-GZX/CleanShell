#include "appviewpage.h"


struct _ClmAppViewPage{
    GtkBox parent;

    GtkWidget * grid;

    style_info style_info;
};

G_DEFINE_FINAL_TYPE(ClmAppViewPage,clm_app_view_page,GTK_TYPE_BOX)

int get_str_now_pos_to(const char * src,GString ** str,int * p,char to);
int ling_app_view_set_apps(ClmDesktop * self,const char * apps_dir);

void clm_app_view_page_class_init(ClmAppViewPageClass * klass){
    GObjectClass * object_class = G_OBJECT_CLASS(klass);
}

void clm_app_view_page_init(ClmAppViewPage * self){
    gtk_orientable_set_orientation(GTK_ORIENTABLE(self),GTK_ORIENTATION_HORIZONTAL);
}



GtkWidget * clm_app_view_page_new(style_info style_info){
    ClmAppViewPage * self = CLM_APP_VIEW_PAGE(g_object_new(CLM_TYPE_APP_VIEW_PAGE,NULL));
    self->style_info = style_info;

    self->grid = ling_grid_new(style_info.column_num,style_info.row_num,style_info.column_space,style_info.row_space);

    gtk_box_append(GTK_BOX(self),self->grid);

    // gtk_widget_set_margin_start(self->grid,10);
    // gtk_widget_set_margin_end(self->grid,10);

    gtk_widget_set_overflow(GTK_WIDGET(self),GTK_OVERFLOW_HIDDEN);
    gtk_widget_set_overflow(self->grid,GTK_OVERFLOW_HIDDEN);
    gtk_widget_set_hexpand(self->grid,TRUE);
    gtk_widget_set_vexpand(self->grid,TRUE);

    gtk_widget_set_margin_top(GTK_WIDGET(self),self->style_info.top_space);
    return GTK_WIDGET(self);
}

GtkWidget * clm_app_view_page_get_fixed_view(ClmAppViewPage * self){
    return self->grid;
}

int i=0;
static void finish_e(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    g_print("end:%d\n",i);
    i++;
}

gboolean clm_app_view_page_add_item(ClmAppViewPage * self,GtkWidget * content,
                                     int column,int row,int width,int height){

    ling_grid_attach(LING_GRID(self->grid),content,column,row,width,height);
    // LingOperate * op = ling_operate_add(shell->controler,"app_item",content);
    // ling_operate_add_action(op,LING_ACTION_CLICK,
    //                         NULL,NULL,
    //                         NULL,NULL,
    //                         NULL,NULL,
    //                         finish_e,finish_e,NULL);
    return 1;
}
