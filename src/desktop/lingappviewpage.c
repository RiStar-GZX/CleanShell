#include "lingappviewpage.h"


struct _LingAppViewPage{
    GtkBox parent;

    GtkWidget * fixed_view;

    GtkWidget * page_left;
    GtkWidget * page_right;

    style_info style_info;
    //shell * shell;
};

G_DEFINE_FINAL_TYPE(LingAppViewPage,ling_app_view_page,GTK_TYPE_BOX)

int get_str_now_pos_to(const char * src,GString ** str,int * p,char to);
int ling_app_view_set_apps(LingDesktop * self,const char * apps_dir);

void ling_app_view_page_class_init(LingAppViewPageClass * klass){
    GObjectClass * object_class = G_OBJECT_CLASS(klass);
}

void ling_app_view_page_init(LingAppViewPage * self){
    gtk_orientable_set_orientation(GTK_ORIENTABLE(self),GTK_ORIENTATION_HORIZONTAL);
}

void ling_app_view_page_set_style(LingAppViewPage * page,style_info style){
    page->style_info = style;
    ling_fixed_view_set_margin(LING_FIXED_VIEW(page->fixed_view),
                               page->style_info.row_space,
                               page->style_info.row_space,20,20);
                               //page->style_info.column_space,
                               //page->style_info.column_space-);
}

// static int page_turn_timer_id=0;

// static gboolean pager_prev_timeout(gpointer user_data){
//     LingViewPager * self = user_data;

//     ling_view_pager_prev(self,TRUE);
//     return G_SOURCE_REMOVE;
// }

// GdkDragAction left_enter (GtkDropTarget* drop,
//                           gdouble x,gdouble y,gpointer user_data){
//     g_print("left_enter\n");
//     LingAppViewPage * self = user_data;
//     GtkWidget * view_pager = gtk_widget_get_parent(GTK_WIDGET(self));
//     view_pager=gtk_widget_get_parent(GTK_WIDGET(view_pager));
//     view_pager=gtk_widget_get_parent(GTK_WIDGET(view_pager));

//     page_turn_timer_id = g_timeout_add(200,pager_prev_timeout,view_pager);
//     return GDK_ACTION_MOVE;
// }

// static gboolean pager_next_timeout(gpointer user_data){
//     LingViewPager * self = user_data;
//     ling_view_pager_next(self,TRUE);
//     return G_SOURCE_REMOVE;
// }

// GdkDragAction right_enter (GtkDropTarget* drop,
//                           gdouble x,gdouble y,gpointer user_data){
//     g_print("right_enter\n");
//     LingAppViewPage * self = user_data;
//     GtkWidget * view_pager = gtk_widget_get_parent(GTK_WIDGET(self));
//     view_pager=gtk_widget_get_parent(GTK_WIDGET(view_pager));
//     view_pager=gtk_widget_get_parent(GTK_WIDGET(view_pager));

//     page_turn_timer_id = g_timeout_add(200,pager_next_timeout,view_pager);
//     return GDK_ACTION_MOVE;
// }

// static void drop_target_leave (GtkDropTarget* drop,gpointer user_data){
//     g_print("leave\n");
//     LingAppViewPage * self = user_data;
//     if(page_turn_timer_id!=0){
//         g_source_remove(page_turn_timer_id);
//         page_turn_timer_id=0;
//     }
// }


GtkWidget * ling_app_view_page_new(style_info style_info){
    LingAppViewPage * self = LING_APP_VIEW_PAGE(g_object_new(LING_TYPE_APP_VIEW_PAGE,NULL));
    self->style_info = style_info;

//    self->grid = ling_grid_new(style_info.column_num,style_info.row_num,TRUE,
//                               self->shell->controler,LING_GRID_DRAG_FACE(self->shell->drag_face));
    self->fixed_view = ling_fixed_view_new(LING_FIXED_VIEW_ARRANGE_GRID,
                                     style_info.column_num,style_info.row_num,
                                     style_info.column_space,style_info.row_space);
    int * drop_data=malloc(sizeof(int));
    *drop_data=1;   //显示文字
    ling_fixed_view_set_item_drop_data(LING_FIXED_VIEW(self->fixed_view),drop_data);


    //左右翻页
    self->page_left=gtk_label_new("");
    self->page_right=gtk_label_new("");
    gtk_widget_set_hexpand(self->page_left,TRUE);
    gtk_widget_set_hexpand(self->page_right,TRUE);

    // GtkDropTarget * drop_left = gtk_drop_target_new(LING_TYPE_FIXED_VIEW_ITEM,GDK_ACTION_MOVE);
    // g_signal_connect(drop_left,"enter",G_CALLBACK(left_enter),self);
    // g_signal_connect(drop_left,"leave",G_CALLBACK(drop_target_leave),self);
    // gtk_widget_add_controller(self->page_left,GTK_EVENT_CONTROLLER(drop_left));

    // GtkDropTarget * drop_right = gtk_drop_target_new(LING_TYPE_FIXED_VIEW_ITEM,GDK_ACTION_MOVE);
    // g_signal_connect(drop_right,"enter",G_CALLBACK(right_enter),self);
    // g_signal_connect(drop_right,"leave",G_CALLBACK(drop_target_leave),self);
    // gtk_widget_add_controller(self->page_right,GTK_EVENT_CONTROLLER(drop_right));

    gtk_box_append(GTK_BOX(self),self->page_left);
    gtk_box_append(GTK_BOX(self),self->fixed_view);
    gtk_box_append(GTK_BOX(self),self->page_right);

    gtk_widget_set_overflow(GTK_WIDGET(self),GTK_OVERFLOW_HIDDEN);
    gtk_widget_set_overflow(self->fixed_view,GTK_OVERFLOW_HIDDEN);
    gtk_widget_set_hexpand(self->fixed_view,TRUE);
    gtk_widget_set_vexpand(self->fixed_view,TRUE);
    //gtk_widget_set_visible(self->grid,FALSE);

    ling_app_view_page_set_style(self,self->style_info);
    return GTK_WIDGET(self);
}

GtkWidget * ling_app_view_page_get_fixed_view(LingAppViewPage * self){
    return self->fixed_view;
}

gboolean ling_app_view_page_add_item(LingAppViewPage * self,GtkWidget * content,
                                     int column,int row,int width,int height){
//    LingAppItem * item = LING_APP_ITEM(ling_app_item_new(self->shell));
//    ling_app_item_set(item,info,self->style_info.icon_size,TRUE);

    GtkWidget * fixed_item = ling_fixed_view_add_content_grid(LING_FIXED_VIEW(self->fixed_view),GTK_WIDGET(content),width,height,column,row);
    ling_fixed_view_item_set_remove_on_drag(LING_FIXED_VIEW_ITEM(fixed_item),TRUE);
    ling_fixed_view_item_set_middle_press_cb(LING_FIXED_VIEW_ITEM(fixed_item),
                                             ling_app_item_drag_start,ling_app_item_drag_end,self->fixed_view);
    ling_fixed_view_item_set_content_copy_func(LING_FIXED_VIEW_ITEM(fixed_item),ling_app_item_copy);
    //GtkWidget * fixed_item = ling_fixed_
    return 1;
}

//ling_app_view_page_edge()

