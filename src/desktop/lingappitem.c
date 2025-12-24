#include "lingappitem.h"

struct _LingAppItem{
    GtkBox parent;

    GtkWidget * image;
    GIcon * gicon;
    GdkPaintable * paintable;
    GtkWidget * label;
    gboolean label_visible;
    //shell * shell;
    app_info * app_info;
    int icon_size;
    GtkEventController * drag;
    //GtkEventController * click;

    uint timer_id;
};

G_DEFINE_FINAL_TYPE(LingAppItem,ling_app_item,GTK_TYPE_BOX)


void ling_app_item_class_init(LingAppItemClass * klass){

}

void ling_app_item_init(LingAppItem * self){
    gtk_orientable_set_orientation(GTK_ORIENTABLE(self),GTK_ORIENTATION_VERTICAL);

}

GtkWidget * ling_app_item_new(){
    return GTK_WIDGET(LING_APP_ITEM(g_object_new(LING_TYPE_APP_ITEM,NULL)));
}

GtkWidget * ling_app_item_copy(GtkWidget * source){
    LingAppItem * item = LING_APP_ITEM(source);
    GtkWidget * item_new = ling_app_item_new();
    ling_app_item_set(LING_APP_ITEM(item_new),item->app_info,item->icon_size,item->label_visible);
//    GtkWidget * item_new = gtk_image_new_from_icon_name("firefox");
//    gtk_image_set_pixel_size(GTK_IMAGE(item_new),64);
    return item_new;
}

void ling_app_item_run_app(LingAppItem * item){
    GString * cmd = g_string_new(item->app_info->exec);
    uint lenth=cmd->len;
    for(int i=0;i<cmd->len;i++){
        if(cmd->str[i]==' '){
            lenth=i;
            break;
        }
    }
    g_string_truncate(cmd,lenth);
    //g_string_printf(cmd,"%s &",info->exec);
    g_string_append_printf(cmd," &");
    g_print("exec:%s\n",cmd->str);
    system(cmd->str);
}


void ling_app_item_drag_finish(gpointer data){
    LingAppItem * self = LING_APP_ITEM(data);
    g_print("finish\n");
    //gtk_overlay_remove_overlay(GTK_OVERLAY(shell->lingoverlay),shell->drag_icon);
    g_source_remove(self->timer_id);
    //shell->drag_icon = NULL;
    self->timer_id =0;
}


static int ison;
static gdouble x,y;
static int timeout;
// static gboolean long_press_timeout(gpointer user_data){
//     timeout =1;
//     LingAppItem * self = LING_APP_ITEM(user_data);
//     LingOperate * op=ling_operate_get(shell->controler,"app_drag");
//     if(ling_operate_start_operating(op)){
//         //ling_app_item_set_drag(self,x,y);
//     }
//     return 0;
// }
// static void drag_begin (GtkGestureDrag* gesture,gdouble start_x,
//                         gdouble start_y,LingAppItem *self){
//     ison=0;
//     timeout=0;
//     x=start_x;
//     y=start_y;
//     self->timer_id = g_timeout_add(500, long_press_timeout, self);
// }

// static void drag_update(GtkGestureDrag *gesture,
//                         gdouble offset_x,
//                         gdouble offset_y,
//                         LingAppItem *self) {
//     if(ison!=0);
//     else if(timeout==1){
//         ison=1;
//     }
//     if(ison==1){
//         LingOperate * op=ling_operate_get(shell->controler,"app_drag");
//         if(ling_operate_start_operating(op)){
//             //g_print("update\n");
//             //gtk_widget_set_margin_start(shell->drag_icon,offset_x);
//             //gtk_widget_set_margin_top(shell->drag_icon,offset_y);
//         }
//     }
// }

// static void drag_end(GtkGestureDrag *gesture,
//                      gdouble offset_x,
//                      gdouble offset_y,
//                      LingAppItem *self) {

//     //gtk_overlay_remove_overlay(GTK_OVERLAY(shell->overlay),self->drag_image);
//     ling_operate_run_animation(ling_operate_get(shell->controler,"app_drag"));
// }



gboolean ling_app_item_set(LingAppItem * self,app_info * app,guint icon_size,gboolean label_visible){
    //app_item * item = malloc(sizeof(app_item));
    self->app_info = app;
//    g_print("APP:%s\n",app->desktop_path->str);
    self->icon_size = icon_size;
    //gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(self), column, row, width, height);

    self->image = gtk_image_new();
    self->gicon = g_icon_new_for_string(app->icon,NULL);
    GtkIconTheme *icon_theme = gtk_icon_theme_get_for_display(gdk_display_get_default());
    self->paintable =  GDK_PAINTABLE(gtk_icon_theme_lookup_by_gicon(icon_theme,self->gicon,128,1,
                                                                    GTK_TEXT_DIR_NONE,0));
    gtk_image_set_pixel_size(GTK_IMAGE(self->image),icon_size);
    gtk_image_set_from_paintable(GTK_IMAGE(self->image), self->paintable);

    //填入body_box
    gtk_box_append(GTK_BOX(self),self->image);
    self->label_visible = label_visible;
    //if(label_able){
        self->label = gtk_label_new(app->name);
        gtk_widget_add_css_class(self->label,"light-style");


        gtk_label_set_max_width_chars(GTK_LABEL(self->label),0);
        gtk_label_set_ellipsize(GTK_LABEL(self->label),PANGO_ELLIPSIZE_END);
        gtk_box_append(GTK_BOX(self),self->label);
    //}
    gtk_widget_set_visible(self->label,label_visible);

    // LingOperate * op_page=ling_operate_add(shell->controler,"app_drag",
    //         NULL,self,
    //         NULL,NULL,
    //         ling_app_item_drag_finish,self);

    // self->drag = GTK_EVENT_CONTROLLER(gtk_gesture_drag_new());
    // gtk_widget_add_controller(GTK_WIDGET(self), GTK_EVENT_CONTROLLER(self->drag));
    // g_signal_connect(self->drag, "drag-begin", G_CALLBACK(drag_begin), self);
    // g_signal_connect(self->drag, "drag-update", G_CALLBACK(drag_update), self);
    // g_signal_connect(self->drag, "drag-end", G_CALLBACK(drag_end), self);
    return 1;
}


app_info * ling_app_item_get_app_info(LingAppItem * self){
    return self->app_info;
}

void ling_app_item_set_label_visible(LingAppItem * self,gboolean visible){
    gtk_widget_set_visible(self->label,visible);
}


// void ling_app_item_drag_start(LingFixedViewItem * item,gpointer user_data){
//     LingFixedView * fv = user_data;
//     //GtkWidget * w = gtk_image_new_from_icon_name("firefox");
//     ling_fixed_view_item_set_drag_source_content(item);

//     GtkWidget * app_item = ling_fixed_view_item_get_content(item);
//     ling_app_item_set_label_visible(LING_APP_ITEM(app_item),FALSE);
// }

// void ling_app_item_drag_end(LingFixedViewItem * item,gpointer item_data,gpointer fv_data){
//     if(item_data==NULL||fv_data==NULL)return;
//     GtkWidget * app_item = ling_fixed_view_item_get_content(item);
//     int * visible = fv_data;
//     ling_app_item_set_label_visible(LING_APP_ITEM(app_item),*visible);
// }
