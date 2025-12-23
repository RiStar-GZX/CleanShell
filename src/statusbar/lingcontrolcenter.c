#include "lingcontrolcenter.h"

enum{
    LAYER_TOP = 0,

    LAYER_FOLDER,
    LAYER_BODY,

    LAYER_END = 255
};

struct _LingControlCenter{
    GtkBox parent;

    //GtkWidget * folder;
    GtkWidget * overlay;

    GtkWidget * center_head;

    GtkWidget * grid;
    GtkWidget * wlan_button;
    GtkWidget * ble_button;

    GtkWidget * media;
    GtkWidget * brightness;
    GtkWidget * volume;
};

G_DEFINE_FINAL_TYPE(LingControlCenter,ling_control_center,GTK_TYPE_BOX);

void ling_control_center_load(LingControlCenter * self);

gboolean change_value (GtkRange* range,GtkScrollType* scroll,
                       gdouble value,gpointer user_data){
    ling_sys_set_volume((int)value);
    return FALSE;
}

gboolean brightess_change_value (GtkRange* range,GtkScrollType* scroll,
                       gdouble value,gpointer user_data){
    ling_sys_set_brightness((int)value);
    return FALSE;
}

static void ling_control_center_class_init(LingControlCenterClass * klass){

}

static void ling_control_center_init(LingControlCenter * self){
    gtk_orientable_set_orientation(GTK_ORIENTABLE(self),GTK_ORIENTATION_VERTICAL);

    self->center_head = ling_center_head_new();
    gtk_box_append(GTK_BOX(self),self->center_head);
}


GtkWidget * ling_control_center_new(GtkWidget * overlay){
    LingControlCenter * self = LING_CONTROL_CENTER(g_object_new(LING_TYPE_CONTROL_CENTER,NULL));


    self->overlay = overlay;

    self->grid = ling_fixed_view_new(LING_FIXED_VIEW_ARRANGE_GRID,4,7,10,10);
    //gtk_widget_set_margin_top(self->grid,20);
    gtk_widget_set_margin_bottom(self->grid,64);
    gtk_widget_set_margin_start(self->grid,20);
    gtk_widget_set_margin_end(self->grid,20);
//    gtk_grid_set_row_spacing(GTK_GRID(self->grid),20);
//    gtk_grid_set_column_spacing(GTK_GRID(self->grid),20);

    gtk_widget_set_vexpand(self->grid,TRUE);
    gtk_widget_set_hexpand(self->grid,TRUE);
    //gtk_box_append(GTK_BOX(self->center_head),self->grid);
    ling_center_head_append(LING_CENTER_HEAD(self->center_head),self->grid);
    ling_control_center_load(self);
    return GTK_WIDGET(self);
}

static void wlan_folder_start(LingFixedViewItem * item,gpointer user_data){
    g_print("start\n");
    GtkWidget * folder = user_data;
    gtk_widget_set_visible(folder,TRUE);
}

void ling_control_center_load(LingControlCenter * self){
    self->wlan_button = ling_control_button_new("wlan","network-wireless-100");
    GtkWidget * a=ling_fixed_view_add_content_grid(LING_FIXED_VIEW(self->grid),self->wlan_button,2,1,1,1);

    GtkWidget * folder= ling_wlan_list_new();
    gtk_widget_set_hexpand(folder,TRUE);
    gtk_widget_set_vexpand(folder,TRUE);
    gtk_widget_set_size_request(folder,500,1000);
    //ling_overlay_add_layer(GTK_OVERLAY(self->overlay),folder,);
    gtk_overlay_add_overlay(GTK_OVERLAY(self->overlay),folder);
    gtk_widget_set_visible(folder,FALSE);
    ling_fixed_view_item_set_middle_press_cb(LING_FIXED_VIEW_ITEM(a),wlan_folder_start,NULL,folder);



    self->ble_button = ling_control_button_new("蓝牙","bluetooth-symbolic");
    GtkWidget * b=ling_fixed_view_add_content_grid(LING_FIXED_VIEW(self->grid),self->ble_button,2,1,3,1);

    GtkWidget * ble_folder= ling_ble_list_new();
    gtk_widget_set_hexpand(ble_folder,TRUE);
    gtk_widget_set_vexpand(ble_folder,TRUE);
    gtk_widget_set_size_request(ble_folder,500,1000);
    gtk_overlay_add_overlay(GTK_OVERLAY(self->overlay),ble_folder);
    gtk_widget_set_visible(ble_folder,FALSE);
    ling_fixed_view_item_set_middle_press_cb(LING_FIXED_VIEW_ITEM(b),wlan_folder_start,NULL,ble_folder);


    self->media = ling_control_media_new();
    ling_fixed_view_add_content_grid(LING_FIXED_VIEW(self->grid),self->media,2,2,1,2);
    //gtk_widget_add_css_class(self->media,"control-item");

    //添加亮度条
    self->brightness = gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL, 0.0, 100.0, 1.0);
    gtk_range_set_inverted(GTK_RANGE(self->brightness), TRUE);
    gtk_range_set_value(GTK_RANGE(self->brightness), ling_sys_get_brightness());
    gtk_widget_add_css_class(self->brightness,"ling-scale");
    ling_fixed_view_add_content_grid(LING_FIXED_VIEW(self->grid),self->brightness,1,2,3,2);
    gtk_range_set_slider_size_fixed(GTK_RANGE(self->brightness),0);
    g_signal_connect(self->brightness,"change_value",G_CALLBACK(brightess_change_value),NULL);

    //添加音量条
    self->volume = gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL, 0.0, 100.0, 1.0);
    gtk_range_set_inverted(GTK_RANGE(self->volume), TRUE);
    gtk_range_set_value(GTK_RANGE(self->volume),ling_sys_get_volume());
    gtk_widget_add_css_class(self->volume,"ling-scale");
    ling_fixed_view_add_content_grid(LING_FIXED_VIEW(self->grid),self->volume,1,2,4,2);
    gtk_range_set_slider_size_fixed(GTK_RANGE(self->volume),0);
    g_signal_connect(self->volume,"change_value",G_CALLBACK(change_value),NULL);

    //添加其余控件
    for(int i=0;i<2;i++){
        for(int j=1;j<=4;j++){
            GtkWidget * button = gtk_toggle_button_new();
            gtk_widget_add_css_class(button,"toggle-button");
            gtk_widget_set_vexpand(button,TRUE);
            gtk_widget_set_hexpand(button,TRUE);
            ling_fixed_view_add_content_grid(LING_FIXED_VIEW(self->grid),button,1,1,j,4+i);
        }
    }
}
