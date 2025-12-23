#include "lingappdrawer.h"
#include <lingdesktop.h>

struct _LingAppDrawer{
    GtkBox parent;
    GtkWidget * sw; //scrolledwindow
    GtkWidget * drawer; //l
    //GtkWidget * overlay;
    //share_info * share_info;
    //style_info style_info;
    //GList * app_items;

    int timer_id;
};

G_DEFINE_FINAL_TYPE(LingAppDrawer,ling_app_drawer,GTK_TYPE_BOX)


//static gboolean press_timeout(gpointer data){
//    LingAppDrawer * self = LING_APP_DRAWER(data);
//    self->timer_id=0;
//    g_print("timeouts\n");
//    LingAppItem * item = ling_app_item_new();
//    ling_app_item_set(item,source->app_info,64,TRUE);
//    ling_grid_drag_face_item_start_drag(self->share_info->drag_face,NULL,item,100,100);
//    return G_SOURCE_REMOVE;
//}

// static int x,y,ison;
// void ling_app_drawer_drag_begin (GtkGestureDrag* gedture,gdouble start_x,
//                                  gdouble start_y,LingAppDrawer *self){
//     ison=0;
//     //self->timer_id = g_timeout_add(500,press_timeout,self);
// }

// static void ling_app_drawer_drag_update(GtkGestureDrag *gesture,
//                                         gdouble offset_x,gdouble offset_y,LingAppDrawer *self) {
//     int top =gtk_widget_get_margin_top(shell->drawer);
//     int h = gtk_widget_get_height(shell->bodybox);
//     gdouble up = top+offset_y;
//     if(ison!=0);
//     else if(up>0){
//         ison=1;
//         g_source_remove(self->timer_id);
//         self->timer_id=0;
//     }
//     if(ison==1){
//         LingOperate * op = ling_operate_get(shell->controler,"drawer");
//         if(ling_operate_start_operating(op)){
//             if(up<0)up=0;
//             if(up>h/9){
//                 ling_operate_set_animation_cb(op,drawerdown_animation,op->animation_data);
//                 ling_operate_set_finish_cb(op,drawerdown_finish,op->finish_data);
//             }
//             else{
//                 ling_operate_set_animation_cb(op,drawerup_animation,op->animation_data);
//                 ling_operate_set_finish_cb(op,drawerup_finish,op->finish_data);
//             }
//             gtk_widget_set_visible(shell->bodybox,1);
//             ling_desktop_set_opacity(shell->bodybox,shell->drawer,h);
//             ling_desktop_set_wallpaper_blur(LING_DESKTOP(shell->desktop),0);
//             gtk_widget_set_margin_top(shell->drawer,up);
//         }
//     }
//     if(ison==2){

//     }
// }

// static void ling_app_drawer_drag_end(GtkGestureDrag *gesture,
//                                      gdouble offset_x,
//                                      gdouble offset_y,
//                                      LingAppDrawer *self) {
//     ling_operate_run_animation(ling_operate_get(shell->controler,"drawer"));
//     ling_operate_run_animation(ling_operate_get(shell->controler,"drawer_app"));
// }

void ling_app_drawer_class_init(LingAppDrawerClass * klass){

}



void ling_app_drawer_init(LingAppDrawer * self){
    gtk_orientable_set_orientation(GTK_ORIENTABLE(self),GTK_ORIENTATION_VERTICAL);

}

static void drag_start(LingFixedViewItem * item,gpointer user_data){
    LingAppDrawer * drawer = user_data;
    ling_fixed_view_item_set_drag_source_content(item);
    gtk_widget_set_margin_top(GTK_WIDGET(drawer),gtk_widget_get_height(shell->drawer));
    //gtk_widget_set_margin_top(GTK_WIDGET(drawer->share_info->drawer),gtk_widget_get_height(drawer->share_info->drawer));
    gtk_widget_set_opacity(shell->bodybox,1);
    //gtk_widget_set_opacity(GTK_WIDGET(drawer),0);
    //gtk_widget_set_visible(GTK_WIDGET(drawer),0);
    gtk_widget_set_visible(GTK_WIDGET(shell->bodybox),1);
    //gtk_widget_set_visible(GTK_WIDGET(fv),FALSE);
}

gboolean ling_app_drawer_add_item(LingAppDrawer *self,app_info * info,
                                  int column,int row,int width,int height){
    GtkWidget * item =ling_app_item_new();
    ling_app_item_set(LING_APP_ITEM(item),info,64,TRUE);
    GtkWidget * fixed_item = ling_fixed_view_item_new(item);
    ling_fixed_view_add_grid(LING_FIXED_VIEW(self->drawer),LING_FIXED_VIEW_ITEM(fixed_item),width,height,column,row);
    ling_fixed_view_item_set_remove_on_drag(LING_FIXED_VIEW_ITEM(fixed_item),TRUE);
    //ling_fixed_view_item_set_content_copy_func(LING_FIXED_VIEW_ITEM(fixed_item),ling_app_item_copy);
    ling_fixed_view_item_set_middle_press_cb(LING_FIXED_VIEW_ITEM(fixed_item),drag_start,NULL,self);

    return 1;
}

//void ling_drawer_app_move_finish(gpointer data){
//    LingAppDrawer * drawer = LING_APP_DRAWER(data);
//    //gtk_widget_set_visible(GTK_WIDGET(drawer),FALSE);
//    gtk_widget_set_margin_top(GTK_WIDGET(drawer),gtk_widget_get_height(drawer->share_info->appview));

//    ling_grid_drag_face_item_end_drag(drawer->share_info->drag_face);
//}

//static void hide_drawer(LingGrid * grid,gpointer data){
//    LingAppDrawer * drawer = LING_APP_DRAWER(data);

////    gtk_widget_set_opacity(GTK_WIDGET(drawer),1.0);
////    gtk_widget_set_opacity(drawer->share_info->bodybox,1.0);
//    //gtk_widget_set_margin_top(GTK_WIDGET(drawer),gtk_widget_get_height(drawer->share_info->appview));
////    gtk_widget_set_visible(GTK_WIDGET(drawer),FALSE);
//    gtk_widget_set_visible(drawer->share_info->bodybox,TRUE);
//    //gtk_widget_set_visible(drawer->share_info->drag_face,TRUE);

//    gtk_widget_set_opacity(GTK_WIDGET(drawer),0);

//    LingOperate * op = ling_operate_get(drawer->share_info->controler,"drawer_app");
//    ling_operate_start_operating(op);
//    ling_operate_set_finish_cb(op,ling_drawer_app_move_finish,drawer);
//    ison=2;

//}

GtkWidget * ling_app_drawer_new(){
    LingAppDrawer * self = LING_APP_DRAWER(g_object_new(LING_TYPE_APP_DRAWER,NULL));
    //share_info->drawer = GTK_WIDGET(drawer);

    self->sw = gtk_scrolled_window_new();
    //self->drawer = ling_grid_new(6,20,TRUE,self->share_info->controler,LING_GRID_DRAG_FACE(self->share_info->drag_face));

    //ling_grid_set_press_cb(LING_GRID(self->drawer),LING_GRID_PRESS_MIDDLE,hide_drawer,self);
    int column_space=30;
    int row_space=30;
    self->drawer = ling_fixed_view_new(LING_FIXED_VIEW_ARRANGE_GRID,5,10,10,10);
    gtk_widget_set_margin_top(self->drawer,row_space);
    gtk_widget_set_margin_bottom(self->drawer,row_space);
    gtk_widget_set_margin_start(self->drawer,column_space);
    gtk_widget_set_margin_end(self->drawer,column_space);
    //gtk_grid_set_column_spacing(GTK_GRID(self->drawer),column_space);
    //gtk_grid_set_row_spacing(GTK_GRID(self->drawer),row_space);


    //LingOperate * op = ling_operate_add(shell->controler,"drawer_app",NULL,self,NULL,self,NULL,self);
    //gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(self->sw),self->drawer);
    //gtk_box_append(GTK_BOX(self),self->sw);
    gtk_box_append(GTK_BOX(self),self->drawer);


    // GtkGesture * drag = gtk_gesture_drag_new();
    // gtk_widget_add_controller(GTK_WIDGET(self), GTK_EVENT_CONTROLLER(drag));
    // g_signal_connect(drag, "drag-begin", G_CALLBACK(ling_app_drawer_drag_begin), self);
    // g_signal_connect(drag, "drag-update", G_CALLBACK(ling_app_drawer_drag_update), self);
    // g_signal_connect(drag, "drag-end", G_CALLBACK(ling_app_drawer_drag_end), self);

    // GtkGesture * swipe = gtk_gesture_swipe_new();
    // gtk_widget_add_controller(GTK_WIDGET(self), GTK_EVENT_CONTROLLER(swipe));
    // g_signal_connect(swipe,"swipe",G_CALLBACK(ling_operate_swipe_cb),op);

    gtk_widget_add_css_class(GTK_WIDGET(self),"app-drawer");


    return GTK_WIDGET(self);
}

