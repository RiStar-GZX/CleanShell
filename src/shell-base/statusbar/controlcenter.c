#include "controlcenter.h"
#include "wlanlist.h"

enum{
    LAYER_TOP = 0,

    LAYER_FOLDER,
    LAYER_BODY,

    LAYER_END = 255
};



struct _ClControlCenter{
    GtkBox parent;

    ClStatusBar * statusbar;

    GtkWidget * center_head;

    GtkWidget * grid;
    GtkWidget * wlan_button;
    GtkWidget * ble_button;

    GtkWidget * media;

    struct {
        GtkWidget * box;
        GtkWidget * scale;
    }brightness,volume;
};

G_DEFINE_FINAL_TYPE(ClControlCenter,cl_control_center,GTK_TYPE_BOX);

void cl_control_center_load(ClControlCenter * self);

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

static void cl_control_center_class_init(ClControlCenterClass * klass){

}

static void cl_control_center_init(ClControlCenter * self){
    gtk_orientable_set_orientation(GTK_ORIENTABLE(self),GTK_ORIENTATION_VERTICAL);

    self->center_head = cl_center_head_new();
    gtk_box_append(GTK_BOX(self),self->center_head);
}


GtkWidget * cl_control_center_new(ClStatusBar * statusbar){
    ClControlCenter * self = CL_CONTROL_CENTER(g_object_new(CL_TYPE_CONTROL_CENTER,NULL));

    self->statusbar = statusbar;

    self->grid = ling_grid_new(4,7,10,10);
    ling_grid_set_aspect(LING_GRID(self->grid),1);
    //gtk_widget_set_margin_top(self->grid,20);
    gtk_widget_set_margin_bottom(self->grid,64);
    gtk_widget_set_margin_start(self->grid,20);
    gtk_widget_set_margin_end(self->grid,20);
//    gtk_grid_set_row_spacing(GTK_GRID(self->grid),20);
//    gtk_grid_set_column_spacing(GTK_GRID(self->grid),20);

    gtk_widget_set_vexpand(self->grid,TRUE);
    gtk_widget_set_hexpand(self->grid,TRUE);
    //gtk_box_append(GTK_BOX(self->center_head),self->grid);
    cl_center_head_append(CL_CENTER_HEAD(self->center_head),self->grid);
    cl_control_center_load(self);
    return GTK_WIDGET(self);
}

/*------------------------------------------------------------------------------------------------------*/


static GtkWidget * folder_open(LingFolder * folder,GtkWidget * widget,gdouble * x,gdouble * y,gpointer user_data){
    ClControlItem * self = CL_CONTROL_ITEM(widget);
    LingGridOriPos * info = (LingGridOriPos *)user_data;

    info->size_w = gtk_widget_get_width(widget);
    info->size_h = gtk_widget_get_height(widget);
    int c,r,w,h;
    ling_grid_query_child(LING_GRID(info->grid),widget,&c,&r,&w,&h);

    gtk_widget_translate_coordinates(widget,info->coordinates,0,0,x,y);
    info->start_x = *x;
    info->start_y = *y;
    info->column = c;
    info->row = r;
    info->grid_w = w;
    info->grid_h = h;

    //ling_grid_remove(LING_GRID(info->grid),c,r);
    ling_grid_remove(LING_GRID(info->grid),widget);
    return NULL;
}

static int folder_expand_w=400,folder_expand_h=800;
static void folder_ani(LingFolder * folder,GtkWidget * widget,GtkWidget * content,gdouble progress,gpointer user_data){
    //
    ClControlItem * self =  CL_CONTROL_ITEM(widget);
    //GtkWidget * folder = cl_control_item_get_folder(self);
    gtk_widget_set_visible(GTK_WIDGET(folder),TRUE);
    gtk_widget_set_opacity(GTK_WIDGET(folder),(progress/100.00f));
    GtkWidget * button = cl_control_item_get_button(self);
    gtk_widget_set_visible(button,TRUE);
    gtk_widget_set_opacity(button,1-(progress/100.00f));

    //
    LingGridOriPos * info = (LingGridOriPos *)user_data;
    int  w = info->size_w+(progress/100)*(folder_expand_w-info->size_w);
    int  h = info->size_h+(progress/100)*(folder_expand_h-info->size_h);
    //gtk_widget_set_size_request(content,w,h);
    // g_print("progress:%f\n",progress);

    int bodybox_w = gtk_widget_get_width(info->coordinates);
    int bodybox_h = gtk_widget_get_height(info->coordinates);
    int end_x=(bodybox_w-folder_expand_w)/2;
    int end_y=(bodybox_h-folder_expand_h)/2;
    int new_x= info->start_x+(end_x-info->start_x)*(progress/100.00f);
    int new_y= info->start_y+(end_y-info->start_y)*(progress/100.00f);

    ling_folder_set_pos(LING_FOLDER(info->face),new_x,new_y);
    ling_folder_set_size(LING_FOLDER(info->face),w,h);
}

static void folder_open_finish(LingFolder * folder,GtkWidget * widget,GtkWidget * content,gpointer user_data){
    ClControlItem * self =  CL_CONTROL_ITEM(widget);
    //GtkWidget * folder = cl_control_item_get_folder(self);
    gtk_widget_set_visible(GTK_WIDGET(folder),TRUE);
    gtk_widget_set_opacity(GTK_WIDGET(folder),1);
    GtkWidget * button = cl_control_item_get_button(self);
    gtk_widget_set_visible(button,FALSE);
    gtk_widget_set_opacity(button,0);
}

static void folder_close(LingFolder * folder,GtkWidget * widget,GtkWidget * content,gpointer user_data){
    ClControlItem * self =  CL_CONTROL_ITEM(widget);
    //GtkWidget * folder = cl_control_item_get_folder(self);
    //gtk_widget_set_visible(folder,FALSE);
    GtkWidget * button = cl_control_item_get_button(self);
    gtk_widget_set_visible(button,TRUE);
    gtk_widget_set_opacity(button,1);

    LingGridOriPos * info = (LingGridOriPos *)user_data;
    gtk_widget_set_size_request(widget,-1,-1);
    ling_grid_attach(LING_GRID(info->grid),widget,info->column,info->row,info->grid_w,info->grid_h);
}

/*------------------------------------------------------------------------------------------------------*/
GtkWidget * cl_control_scale_new(GtkWidget ** scale){
    GtkWidget * box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    *scale = gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL, 0.0, 100.0, 1.0);
    gtk_widget_set_vexpand(*scale,TRUE);
    gtk_widget_set_hexpand(*scale,TRUE);
    gtk_box_append(GTK_BOX(box),*scale);
    gtk_range_set_inverted(GTK_RANGE(*scale), TRUE);
    gtk_range_set_value(GTK_RANGE(*scale), ling_sys_get_brightness());
    gtk_widget_add_css_class(*scale,"ling-scale");
    gtk_range_set_slider_size_fixed(GTK_RANGE(*scale),0);

    return box;
}

void cl_control_center_load(ClControlCenter * self){
    GtkWidget * folder = cl_status_bar_get_folder(CL_STATUS_BAR(self->statusbar));
    LingGridOriPos * w_info = (LingGridOriPos*)g_malloc0(sizeof(LingGridOriPos));
    w_info->face = folder;
    w_info->grid = self->grid;
    w_info->coordinates = GTK_WIDGET(self);

    LingOperate * ctb_op = ling_operate_get(shell->controler,CL_STATUSBAR_CENTERBOX_OP_NAME);
    LingOperate * vp_op = ling_operate_get(shell->controler,CL_STATUSBAR_VIEWPAGER_OP_NAME);

    //wifi按钮
    GtkWidget * button = cl_control_button_new("网络","network-wireless-100");
    GtkWidget * item_folder = cl_wlan_list_new();
    self->wlan_button = cl_control_item_new(button,item_folder);
    ling_grid_attach(LING_GRID(self->grid),self->wlan_button,1,1,2,1);
    LingOperate * op = ling_operate_add(shell->controler,"wifi-button",self->wlan_button);
    ling_folder_operate(op,LING_FOLDER(folder),LING_ACTION_CLICK,
                        folder_open,w_info,folder_ani,w_info,
                        folder_open_finish,w_info,folder_close,w_info);
    GtkWidget * be = cl_control_button_get_sub_button(CL_CONTROL_BUTTON(button));
    ling_operate_drag_ignore(op,be);

    //蓝牙按钮
    button = cl_control_button_new("蓝牙","bluetooth-symbolic");
    self->ble_button = cl_control_item_new(button,gtk_button_new());
    ling_grid_attach(LING_GRID(self->grid),self->ble_button,3,1,2,1);
    op = ling_operate_add(shell->controler,"ble-button",self->ble_button);
    ling_folder_operate(op,LING_FOLDER(folder),LING_ACTION_CLICK,
                        folder_open,w_info,folder_ani,w_info,
                        folder_open_finish,w_info,folder_close,w_info);
    be = cl_control_button_get_sub_button(CL_CONTROL_BUTTON(button));
    ling_operate_drag_ignore(op,be);

    self->media = cl_control_media_new();
    ling_grid_attach(LING_GRID(self->grid),self->media,1,2,2,2);


    //添加亮度条
    self->brightness.box = cl_control_scale_new(&self->brightness.scale);
    ling_grid_attach(LING_GRID(self->grid),self->brightness.box,3,2,1,2);
    g_signal_connect(self->brightness.scale,"change_value",G_CALLBACK(brightess_change_value),ctb_op);
    ling_operate_drag_ignore(ctb_op,self->brightness.box);
    ling_operate_drag_ignore(vp_op,self->brightness.box);

    //添加音量条
    self->volume.box = cl_control_scale_new(&self->volume.scale);
    ling_grid_attach(LING_GRID(self->grid),self->volume.box,4,2,1,2);
    g_signal_connect(self->volume.scale,"change_value",G_CALLBACK(change_value),NULL);
    ling_operate_drag_ignore(ctb_op,self->volume.box);
    ling_operate_drag_ignore(vp_op,self->volume.box);

    //添加其余控件
    for(int i=0;i<2;i++){
        for(int j=1;j<=4;j++){
            GtkWidget * button = gtk_toggle_button_new();
            gtk_widget_add_css_class(button,"toggle-button");
            gtk_widget_set_vexpand(button,TRUE);
            gtk_widget_set_hexpand(button,TRUE);
            ling_grid_attach(LING_GRID(self->grid),button,j,4+i,1,1);
        }
    }
}
