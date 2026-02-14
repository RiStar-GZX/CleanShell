#include "desktopitem.h"
#include <desktop.h>
#include <wm.h>

#define ICON_SIZE 20

typedef struct{
    GtkWidget * image;
    GIcon * gicon;
    GdkPaintable * paintable;
    app_info * app_info;
    int icon_size;
    gboolean runable;
    //LingOperate * op;
}ClmDesktopItemApp;

typedef struct{
    //GtkWidget * label;
    GtkWidget * box;
    GtkWidget * grid;
    //LingOperate * op;
}ClmDesktopItemFolder;


struct _ClmDesktopItem{
    GtkBox parent;
    uint type;

    LingOperate * op;
    gboolean label_visible;
    GtkWidget * label;
    GtkWidget * content;
    ClmDesktopItemApp app;
    ClmDesktopItemFolder folder;
    LingGridOriPos info;
};

G_DEFINE_FINAL_TYPE(ClmDesktopItem,clm_desktop_item,GTK_TYPE_BOX)

static GdkContentProvider * ds_prepare (GtkDragSource* source,gdouble x,gdouble y,gpointer user_data){
    ClmDesktopItem * item = CLM_DESKTOP_ITEM(user_data);
    if(item->type==CLM_DESKTOP_ITEM_APP&&!item->app.runable)return NULL;

    GtkWidget * grid = gtk_widget_get_parent(GTK_WIDGET(item));
    if(grid==NULL)return NULL;

    GdkPaintable *paintable = gtk_widget_paintable_new (GTK_WIDGET(item));
    gtk_drag_source_set_icon(source, paintable, x, y);
    return gdk_content_provider_new_typed(G_TYPE_STRING,"item");
}

static void ds_begin(GtkDragSource* source,GdkDrag* drag,gpointer user_data){
    ClmDesktopItem * item = CLM_DESKTOP_ITEM(user_data);
    GtkWidget * grid = gtk_widget_get_parent(GTK_WIDGET(item));
    //ling_grid_remove(LING_GRID(grid),GTK_WIDGET(item));
}

static gboolean ds_cancel(GtkDragSource* source,GdkDrag* drag,GdkDragCancelReason* reason,gpointer user_data){

    return 0;
}

static void ds_end(GtkDragSource* source,GdkDrag* drag,gboolean delete_data,gpointer user_data){

}


void clm_desktop_item_class_init(ClmDesktopItemClass * klass){

}

void clm_desktop_item_init(ClmDesktopItem * self){
    gtk_orientable_set_orientation(GTK_ORIENTABLE(self),GTK_ORIENTATION_VERTICAL);
    self->content = gtk_aspect_frame_new(0,0,1,FALSE);//gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    gtk_box_append(GTK_BOX(self),self->content);

    self->op = ling_operate_add(shell->controler,"desktop_item",self);
    ling_operate_add_dragsource(self->op,LING_DRAG_SOURCE_LONG_PRESS,
                                ds_prepare,self,
                                ds_begin,self,
                                ds_cancel,self,
                                ds_end,self);
}

void clm_desktop_item_set_label_visible(ClmDesktopItem * self,gboolean visible){
    self->label_visible = visible;
    gtk_widget_set_visible(self->label,visible);
}

/*---------------------------------APP-----------------------------------------------*/

/*保留
void ling_desktop_item_run_app(ClmDesktopItem * self){

    if(self->type!=CLM_DESKTOP_ITEM_APP||!self->app.runable)return;
    GString * cmd = g_string_new(self->app.app_info->exec);
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
*/

void ling_desktop_item_run_app(ClmDesktopItem * self){

}

/*---------------------------------appani-----------------------------------------------------*/

// static gboolean app_click_release(GtkWidget * widget,LingActionArgs args,gpointer user_data){
//     ClmDesktopItem * self =  CLM_DESKTOP_ITEM(widget);

//     if(self->type!=CLM_DESKTOP_ITEM_APP||self->app.runable==FALSE)return LING_OPERATE_ANIMATION_DIR_BACK;

//     //LingLayer * layer;
//     //clm_desktop_get_layer_wm(CLM_DESKTOP(shell->desktop),&layer);
//     gtk_widget_set_visible(shell->wm,TRUE);

//     ClWmWindow * win = cl_wm_get_window_by_name(CL_WM(shell->wm),self->app.app_info->name);
//     if(win==NULL)win = cl_wm_add_window(CL_WM(shell->wm),self->app.app_info->icon,self->app.app_info->name);
//     cl_wm_set_window_showable(win,TRUE);
//     cl_wm_set_window_size(win,300,300);
//     cl_wm_move_window(win,100,100);
//     //cl_wm_move_window_by_progress(CL_WM(shell->wm),win,100,100,1,50);
//     // gtk_widget_set_visible(shell->wm,TRUE);
//     return LING_OPERATE_ANIMATION_DIR_FORWARD;
// }

// static void app_click_finish(GtkWidget * widget,LingActionArgs args,gpointer user_data){
//     ClmDesktopItem * self =  CLM_DESKTOP_ITEM(widget);
//     if(self->type!=CLM_DESKTOP_ITEM_APP||self->app.runable==FALSE)return;
//     ling_desktop_item_run_app(self);
// }

/*--------------------------------------------------------------------------------------------*/

static void app_open_ani(GtkWidget * widget,ClWmWindow * window,LingActionArgs args,CL_WM_WINDOW_ACT act,gpointer user_data){
    ClmDesktopItem * self =  CLM_DESKTOP_ITEM(widget);

    if(self->type!=CLM_DESKTOP_ITEM_APP)return;
    if(cl_wm_get_current_window(CL_WM(shell->wm))==NULL&&self->app.runable==FALSE)return;

    //计算并且设置窗口大小
    int  nw = self->info.size_w+(args.progress/100)*(self->info.target_w-self->info.size_w);
    int  nh = self->info.size_h+(args.progress/100)*(self->info.target_h-self->info.size_h);
    cl_wm_set_window_size(window,nw,nh);

    //计算并且设置窗口位置
    int new_x= self->info.start_x-(self->info.start_x-self->info.target_x)*(args.progress/100.00f);
    int new_y= self->info.start_y-(self->info.start_y-self->info.target_y)*(args.progress/100.00f);

    if(args.emit_data!=NULL){
        ClWmBack * back = (ClWmBack *)args.emit_data;
        gdouble x=args.progress,k=fabs(back->velocity_y)/2000;
        if(k>1)k=1;
        if(k>0){
            //斜抛
            gdouble t = (new_y-0)/k/10000;
            new_y+=-k*t*x*x-1*x;
        }
    }

    cl_wm_move_window(window,new_x,new_y);

    //图标到窗口的渐变
    float a=1;
    cl_wm_window_set_gradient_progress(window,(args.progress*a/100.00f)-a+1);

    //设置桌面效果
    LingLayer * layer;
    clm_desktop_get_layer_bodybox(CLM_DESKTOP(shell->desktop),&layer);
    ling_widget_scale(layer->widget,0.95+0.05*fabs(100-args.progress)/100);

    gdouble blur = (args.progress/100)*CLM_DESKTOP_BLUR;
    if(act==CL_WM_WINDOW_OPEN&&blur<clm_desktop_get_wallpaper_blur(CLM_DESKTOP(shell->desktop))){
        return;
    }
    clm_desktop_set_wallpaper_blur(CLM_DESKTOP(shell->desktop),blur);
}

static ClWmWindow * app_open_start(GtkWidget * widget,gdouble * x,gdouble * y,gpointer user_data){
    ClmDesktopItem * self =  CLM_DESKTOP_ITEM(widget);
    if(self->type!=CLM_DESKTOP_ITEM_APP)return NULL;
    if(cl_wm_get_current_window(CL_WM(shell->wm))==NULL&&self->app.runable==FALSE)return NULL;

    gtk_widget_set_visible(shell->wm,TRUE);

    ClWmWindow * win = cl_wm_get_window_by_name(CL_WM(shell->wm),self->app.app_info->name);
    if(win==NULL){
        win = cl_wm_add_window(CL_WM(shell->wm),self->app.app_info->icon,self->app.app_info->name);
    }

    //self->info.content= GTK_WIDGET(win);
    cl_wm_set_window_showable(win,TRUE);

    GtkWidget * grid = gtk_widget_get_parent(widget);
    //LingGrid * grid = LING_GRID(parent);
    int c,r,w,h;
    ling_grid_query_child(LING_GRID(grid),GTK_WIDGET(self),&c,&r,&w,&h);

    gtk_widget_translate_coordinates(GTK_WIDGET(self),GTK_WIDGET(shell),0,0,x,y);

    self->info.start_x = *x;
    self->info.start_y = *y;
    self->info.column = c;
    self->info.row = r;
    self->info.grid_w = w;
    self->info.grid_h = h;
    self->info.grid = grid;
    self->info.target_w = gtk_widget_get_width(GTK_WIDGET(shell));
    self->info.target_h = gtk_widget_get_height(GTK_WIDGET(shell));
    self->info.target_x = 0;
    self->info.target_y = 0;

    self->info.size_w = gtk_widget_get_width(self->content);
    self->info.size_h = gtk_widget_get_height(self->content);

    cl_wm_move_window(win,*x,*y);

    gtk_widget_set_opacity(GTK_WIDGET(self),0);
    //cl_wm_move_window_by_progress(CL_WM(shell->wm),win,100,100,1,50);
    // gtk_widget_set_visible(shell->wm,TRUE);
    //LingLayer * layer;
    //clm_desktop_get_layer_folder(CLM_DESKTOP(shell->desktop),&layer);
    //ling_folder_close(LING_FOLDER(layer->widget));
    return win;
}


static void app_open_finish(GtkWidget * widget,ClWmWindow * win,LingActionArgs args,gpointer user_data){
    ClmDesktopItem * self =  CLM_DESKTOP_ITEM(widget);

    if(self->type!=CLM_DESKTOP_ITEM_APP)return;
    if(cl_wm_get_current_window(CL_WM(shell->wm))==NULL&&self->app.runable==FALSE)return;

    LingLayer * layer;
    clm_desktop_get_layer_folder(CLM_DESKTOP(shell->desktop),&layer);

    gtk_widget_set_opacity(GTK_WIDGET(self),1);
    ling_folder_close(LING_FOLDER(layer->widget),FALSE);
}

static void app_close_start(GtkWidget * widget,ClWmWindow * win,LingActionArgs args,gpointer user_data){
    ClmDesktopItem * self =  CLM_DESKTOP_ITEM(widget);
    if(self->type!=CLM_DESKTOP_ITEM_APP)return;
    if(cl_wm_get_current_window(CL_WM(shell->wm))==NULL&&self->app.runable==FALSE)return;
    gtk_widget_set_opacity(GTK_WIDGET(self),0);

    GtkWidget * grid = gtk_widget_get_parent(widget);
    //LingGrid * grid = LING_GRID(parent);

    gdouble x,y;
    int c,r,w,h;
    ling_grid_query_child(LING_GRID(grid),GTK_WIDGET(self),&c,&r,&w,&h);

    gtk_widget_translate_coordinates(GTK_WIDGET(self),GTK_WIDGET(shell),0,0,&x,&y);

    self->info.start_x = x;
    self->info.start_y = y;
    self->info.column = c;
    self->info.row = r;
    self->info.grid_w = w;
    self->info.grid_h = h;
    self->info.target_w = gtk_widget_get_width(GTK_WIDGET(win));
    self->info.target_h = gtk_widget_get_height(GTK_WIDGET(win));
    cl_wm_window_get_info(win,&self->info.target_x,&self->info.target_y,NULL,NULL);

    self->info.size_w = gtk_widget_get_width(self->content);
    self->info.size_h = gtk_widget_get_height(self->content);
}

static void app_close_finish(GtkWidget * widget,ClWmWindow * win,LingActionArgs args,gpointer user_data){
    ClmDesktopItem * self =  CLM_DESKTOP_ITEM(widget);
    if(self->type!=CLM_DESKTOP_ITEM_APP)return;
    if(cl_wm_get_current_window(CL_WM(shell->wm))==NULL&&self->app.runable==FALSE)return;
    gtk_widget_set_opacity(GTK_WIDGET(self),1);
    cl_wm_set_window_showable(win,FALSE);
    gtk_widget_set_visible(shell->wm,FALSE);
}

GtkWidget * clm_desktop_item_app_new(app_info * app,guint icon_size,gboolean label_visible){
    ClmDesktopItem * self =  CLM_DESKTOP_ITEM(g_object_new(CLM_TYPE_DESKTOP_ITEM,NULL));
    self->type = CLM_DESKTOP_ITEM_APP;

    ClmDesktopItemApp * a = &self->app;
    a->app_info = app;
    a->icon_size = icon_size;

    a->image = gtk_image_new();
    // gtk_widget_set_valign(a->image,GTK_ALIGN_CENTER);
    // gtk_widget_set_halign(a->image,GTK_ALIGN_CENTER);
    a->gicon = g_icon_new_for_string(app->icon,NULL);
    GtkIconTheme *icon_theme = gtk_icon_theme_get_for_display(gdk_display_get_default());
    a->paintable =  GDK_PAINTABLE(gtk_icon_theme_lookup_by_gicon(icon_theme,a->gicon,512,1,
                                                                GTK_TEXT_DIR_NONE,0));
    gtk_image_set_pixel_size(GTK_IMAGE(a->image),icon_size);
    gtk_image_set_from_paintable(GTK_IMAGE(a->image), a->paintable);
    //gtk_box_append(GTK_BOX(self->content),a->image);
    gtk_aspect_frame_set_child(GTK_ASPECT_FRAME(self->content),a->image);

    //label
    self->label_visible = label_visible;
    self->label = gtk_label_new(app->name);
    gtk_widget_add_css_class(self->label,"dark-style");

    gtk_label_set_ellipsize(GTK_LABEL(self->label),PANGO_ELLIPSIZE_END);
    gtk_box_append(GTK_BOX(self),self->label);

    gtk_widget_set_visible(self->label,label_visible);

    cl_wm_add_operate(self->op,CL_WM(shell->wm),
                    self->app.app_info->icon,self->app.app_info->name,
                    app_open_ani,NULL,
                    app_open_start,NULL,
                    app_open_finish,NULL,
                    app_close_start,NULL,
                    app_close_finish,NULL);
    ling_operate_set_force_run(self->op,TRUE);

    //op = ling_drag_face
    return GTK_WIDGET(self);
}

app_info * clm_desktop_item_get_app_info(ClmDesktopItem * self){
    if(self->type==CLM_DESKTOP_ITEM_APP)return self->app.app_info;
    return NULL;
}

void clm_desktop_item_app_set_runable(ClmDesktopItem * self,gboolean runable){
    if(self->type!=CLM_DESKTOP_ITEM_APP)return;
    self->app.runable=runable;
}

void clm_desktop_item_app_set_icon_size(ClmDesktopItem * self,int size){
    if(self->type!=CLM_DESKTOP_ITEM_APP)return;
    gtk_image_set_pixel_size(GTK_IMAGE(self->app.image),size);
}

LingOperate  * clm_desktop_item_get_operate(ClmDesktopItem * self){
    return self->op;
}


/*---------------------------------FOLDER-----------------------------------------------*/

void clm_desktop_item_folder_set_app_runable(ClmDesktopItem * self,gboolean runable){
    GtkWidget * a = gtk_widget_get_first_child(self->folder.grid);
    for(;a!=NULL;a=gtk_widget_get_next_sibling(a)){
        if(!CLM_IS_DESKTOP_ITEM(a))continue;
        ClmDesktopItem * app = CLM_DESKTOP_ITEM(a);
        app->app.runable = runable;
    }
}

void clm_desktop_item_folder_set_app_icon_size(ClmDesktopItem * self,int size){
    GtkWidget * a = gtk_widget_get_first_child(self->folder.grid);
    for(;a!=NULL;a=gtk_widget_get_next_sibling(a)){
        if(!CLM_IS_DESKTOP_ITEM(a))continue;
        clm_desktop_item_app_set_icon_size(CLM_DESKTOP_ITEM(a),size);
    }
}

static GtkWidget * folder_open(LingFolder * folder,GtkWidget * widget,gdouble * x,gdouble * y,gpointer user_data){
    ClmDesktopItem * self =  CLM_DESKTOP_ITEM(widget);
    GtkWidget * grid = gtk_widget_get_parent(widget);
    if(!LING_IS_GRID(grid))return NULL;

    //self->content
    GtkWidget * content = gtk_widget_get_first_child(self->content);
    self->info.size_w = gtk_widget_get_width(content);
    self->info.size_h = gtk_widget_get_height(content);
    //LingGrid * grid = LING_GRID(parent);
    int c,r,w,h;
    ling_grid_query_child(LING_GRID(grid),GTK_WIDGET(widget),&c,&r,&w,&h);

    gtk_widget_translate_coordinates(content,GTK_WIDGET(shell),0,0,x,y);
    self->info.start_x = *x;
    self->info.start_y = *y;
    self->info.column = c;
    self->info.row = r;
    self->info.grid_w = w;
    self->info.grid_h = h;
    self->info.grid = grid;

    GtkWidget * space = gtk_label_new("");
    gtk_widget_set_opacity(space,0);
    gtk_widget_set_hexpand(space,TRUE);
    gtk_widget_set_vexpand(space,TRUE);
    gtk_aspect_frame_set_child(GTK_ASPECT_FRAME(self->content),space);
    return content;
}

static int folder_expand_w=300,folder_expand_h=300;
static void folder_ani(LingFolder * folder,GtkWidget * item,GtkWidget * content,gdouble progress,gpointer user_data){
    ClmDesktopItem * self = CLM_DESKTOP_ITEM(item);

    int body_w = gtk_widget_get_width(shell->desktop);
    int body_h = gtk_widget_get_height(shell->desktop);
    int w = self->info.size_w+(progress/100)*(folder_expand_w-self->info.size_w);
    int h = self->info.size_h+(progress/100)*(folder_expand_h-self->info.size_h);
    gtk_widget_set_opacity(self->label,1-progress/100.0000f);
    //gtk_widget_set_opacity(self->folder.label,progress/100.0000f);
    clm_desktop_item_folder_set_app_icon_size(self,ICON_SIZE+(progress/100)*100);

    //计算新坐标新尺寸
    LingLayer * layer;
    clm_desktop_get_layer_bodybox(CLM_DESKTOP(shell->desktop),&layer);
    // int bodybox_w = gtk_widget_get_width(layer->widget);
    // int bodybox_h = gtk_widget_get_height(layer->widget);
    int end_x=self->info.start_x-(folder_expand_w-self->info.size_w)/2;//(bodybox_w-folder_expand_w)/2;
    if(end_x<=5)end_x=5;
    if(end_x+folder_expand_w>=body_w-5)end_x=body_w-5-folder_expand_w;
    int end_y=self->info.start_y-(folder_expand_h-self->info.size_h)/2;//(bodybox_h-folder_expand_h)/2;
    if(end_y<=5)end_y=5;
    if(end_y+folder_expand_h>=body_h-5)end_y=body_h-5-folder_expand_h;

    int new_x= self->info.start_x+(end_x-self->info.start_x)*(progress/100.00f);
    int new_y= self->info.start_y+(end_y-self->info.start_y)*(progress/100.00f);

    //设置坐标尺寸
    ling_folder_set_pos(folder,new_x,new_y);
    ling_folder_set_size(folder,w,h);
    //clm_desktop_set_wallpaper_blur(CLM_DESKTOP(shell->desktop),(progress/100)*CLM_DESKTOP_BLUR);
}

static void folder_open_finish(LingFolder * folder,GtkWidget * item,GtkWidget * content,gpointer user_data){
    ClmDesktopItem * self =  CLM_DESKTOP_ITEM(item);
    clm_desktop_item_folder_set_app_runable(self,TRUE);
}

static void folder_close(LingFolder * folder,GtkWidget * item,GtkWidget * content,gpointer user_data){
    ClmDesktopItem * self = CLM_DESKTOP_ITEM(item);
    clm_desktop_item_folder_set_app_runable(self,FALSE);
    //gtk_widget_set_visible(self->label,TRUE);
    //gtk_box_insert_child_after(GTK_BOX(self),content,NULL);
    //gtk_box_insert_child_after(GTK_BOX(self),content,NULL);
    GtkWidget * space = gtk_aspect_frame_get_child(GTK_ASPECT_FRAME(self->content));
    gtk_aspect_frame_set_child(GTK_ASPECT_FRAME(self->content),content);
    g_object_unref(space);
    //ling_grid_attach(LING_GRID(self->info.grid),item,self->info.column,self->info.row,self->info.grid_w,self->info.grid_h);
}

GtkWidget * clm_desktop_item_folder_new(LingFolder * folderlayer,uint column,uint row,
                                       GList * applist,const char * folder_name,gboolean label_visible){
    ClmDesktopItem * self =  CLM_DESKTOP_ITEM(g_object_new(CLM_TYPE_DESKTOP_ITEM,NULL));
    self->type = CLM_DESKTOP_ITEM_FOLDER;
    self->info.face=GTK_WIDGET(folderlayer);

    self->folder.box =  gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    gtk_widget_add_css_class(self->folder.box,"desktop-folder");
    self->folder.grid = ling_grid_new(column,row,0,0);
    GList * now = applist;
    for(int c=1;c<=column;c++){
        for(int r=1;r<=row;r++){
            app_info * info = now->data;
            if(info==NULL)continue;
            GtkWidget * app = clm_desktop_item_app_new(info,ICON_SIZE,FALSE);
            now=now->next;
            //clm_desktop_item_app_set_runable(CLM_DESKTOP_ITEM(app),TRUE);
            //gtk_widget_set_hexpand(button,TRUE);
            //gtk_widget_set_vexpand(button,FALSE);
            ling_grid_attach(LING_GRID(self->folder.grid),app,c,r,1,1);
        }
    }
    gtk_widget_set_hexpand(self->folder.grid,TRUE);
    gtk_widget_set_vexpand(self->folder.grid,TRUE);
    gtk_widget_set_margin_top(self->folder.grid,10);
    gtk_widget_set_margin_bottom(self->folder.grid,10);
    gtk_widget_set_margin_start(self->folder.grid,10);
    gtk_widget_set_margin_end(self->folder.grid,10);

    gtk_box_append(GTK_BOX(self->folder.box),self->folder.grid);
    //gtk_box_append(GTK_BOX(self->content),self->folder.box);
    gtk_aspect_frame_set_child(GTK_ASPECT_FRAME(self->content),self->folder.box);
    //label
    self->label_visible = label_visible;
    self->label = gtk_label_new(folder_name);
    gtk_label_set_xalign(GTK_LABEL(self->label),0.5);
    gtk_label_set_yalign(GTK_LABEL(self->label),0.5);
    gtk_widget_add_css_class(self->label,"dark-style");

    //self->folder.label = gtk_label_new(folder_name);
    //gtk_widget_add_css_class(self->folder.label,"desktop-item-folder-label");

    gtk_label_set_ellipsize(GTK_LABEL(self->label),PANGO_ELLIPSIZE_END);

    gtk_box_append(GTK_BOX(self),self->label);

    gtk_widget_set_visible(self->label,label_visible);

    ling_folder_operate(self->op,folderlayer,LING_ACTION_CLICK,
                            folder_open,NULL,folder_ani,NULL,
                            folder_open_finish,NULL,folder_close,NULL);
    return GTK_WIDGET(self);
}
