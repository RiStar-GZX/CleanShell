#include "lingdesktop.h"
#include <lingappviewpage.h>
#include <lingappitem.h>
#include <lingappdrawer.h>
#include <lingstatusbar.h>
#include <lingfixedview.h>
#include <lingdatasaver.h>
#include <lingwallpaper.h>

#define DESKTOP_ICON_INI_PATH "~/.local/share/clean/desktop_icon.ini"

enum{
    LAYER_TOP = 0,
    LAYER_DRAWER,
    LAYER_BODYBOX,
    LAYER_WALLPAPER,
    LAYER_END = 255
};

G_DEFINE_FINAL_TYPE(LingDesktop,ling_desktop,GTK_TYPE_BOX)


int ling_desktop_set_apps(LingDesktop * self,const char * apps_dir);
void ling_desktop_set_wallpaper(LingDesktop * self,const char * path);
void ling_desktop_set_opacity(GtkWidget * body,GtkWidget * up,uint win_height);
GtkWidget * ling_desktop_set_page(LingDesktop * self,uint pos);
void ling_desktop_load_page(LingDesktop * self);
void ling_desktop_add_page_test(LingDesktop * self);

void ling_app_view_pager_save_data(LingDesktop * self);
int ling_app_view_pager_load_from_data(LingDesktop * self);

#define PAGE_DIRECTION_LEFT 0
#define PAGE_DIRECTION_RIGHT 1

GtkWidget * ling_desktop_get_beside_page(LingDesktop * self,gboolean dir,uint * pos);

gboolean ling_desktop_page_load_test(LingDesktop * self,LingAppViewPage * page){
    GList * app = shell->app_info;
    for(int i=0;i<self->app_num;i++)app=app->next;

    for(int r=5;r<=self->style_info.row_num;r++){
        for(int c=1;c<=self->style_info.column_num;c++){
            app_info * info = (app_info*)app->data;
            GtkWidget * item = ling_app_item_new();
            ling_app_item_set(LING_APP_ITEM(item),info,self->style_info.icon_size,TRUE);
            ling_app_view_page_add_item(page,item,c,r,1,1);
            app=app->next;
            if(app==NULL)return 0;
            self->app_num++;
        }
    }

    return 1;
}

//static void drag_start(LingFixedViewItem * item,gpointer user_data){
//    LingFixedView * fv = user_data;
//    ling_fixed_view_item_set_drag_source_content(item);

//    GtkWidget * app_item = ling_fixed_view_item_get_content(item);
//    ling_app_item_set_label_visible(LING_APP_ITEM(app_item),FALSE);
//}

//static void drag_end(LingFixedViewItem * item,gpointer user_data){
//    GtkWidget * app_item = ling_fixed_view_item_get_content(item);
//    ling_app_item_set_label_visible(LING_APP_ITEM(app_item),FALSE);
//}

gboolean ling_desktop_dock_add_item(LingDesktop * self,app_info * info,uint column){
    LingAppItem * item = LING_APP_ITEM(ling_app_item_new());
    ling_app_item_set(item,info,self->style_info.icon_size,FALSE);

    GtkWidget * fixed_item =ling_fixed_view_item_new(GTK_WIDGET(item));
    ling_fixed_view_item_set_remove_on_drag(LING_FIXED_VIEW_ITEM(fixed_item),TRUE);
    ling_fixed_view_add_grid(LING_FIXED_VIEW(self->dock),LING_FIXED_VIEW_ITEM(fixed_item),1,1,column,1);
    ling_fixed_view_item_set_middle_press_cb(LING_FIXED_VIEW_ITEM(fixed_item),ling_app_item_drag_start,ling_app_item_drag_end,self->dock);

    return 1;
}

static void ling_desktop_dispose(GObject *object){
    LingDesktop * self = LING_DESKTOP(object);
//    ling_data_saver_save_pages_to_db(self->data_saver,LING_VIEW_PAGER(self->view_pager));
    ling_app_view_pager_save_data(self);
    g_print("END\n");

}

static void ling_desktop_class_init(LingDesktopClass * klass){
    GObjectClass * object_class = G_OBJECT_CLASS(klass);
    object_class->dispose = ling_desktop_dispose;
}


gboolean drawerup_animation(gdouble velocity_x,gdouble velocity_y,gpointer data){
    LingShell * shell=(LingShell *)data;
    int top = gtk_widget_get_margin_top(shell->drawer);
    int h = gtk_widget_get_height(shell->bodybox);

    if(top>0){
        gdouble min = top/20;
        min+=5+fabs(velocity_y)/100;
        gtk_widget_set_margin_top(shell->drawer,top-min);
        ling_desktop_set_opacity(shell->bodybox,shell->drawer,h);
    }else{
        return LING_OPERATE_ANIMATION_REMOVE;
    }
    return LING_OPERATE_ANIMATION_CONTINUE;
}

void drawerup_finish(gpointer data){
    LingShell * shell=(LingShell *)(data);
    gtk_widget_set_opacity(shell->drawer,1.0);
    gtk_widget_set_opacity(shell->bodybox,1.0);
    gtk_widget_set_margin_top(shell->drawer,0);
    gtk_widget_set_visible(shell->drawer,TRUE);
    gtk_widget_set_visible(shell->bodybox,FALSE);
}

gboolean drawerdown_animation(gdouble velocity_x,gdouble velocity_y,gpointer user_data){
    LingShell * shell = (LingShell*)user_data;
    int top = gtk_widget_get_margin_top(shell->drawer);
    int h = gtk_widget_get_height(shell->bodybox);

    if(top<h){
        gdouble plus = (h-top)/20;
        plus+=5+fabs(velocity_y)/100;
        gtk_widget_set_margin_top(shell->drawer,top+plus);
        ling_desktop_set_opacity(shell->bodybox,shell->drawer,h);
    }else{
        return LING_OPERATE_ANIMATION_REMOVE;
    }
    return LING_OPERATE_ANIMATION_CONTINUE;
}

void drawerdown_finish(gpointer data){
    LingShell * shell=(LingShell *)data;
    gtk_widget_set_opacity(shell->drawer,1.0);
    gtk_widget_set_opacity(shell->bodybox,1.0);
    gtk_widget_set_margin_top(shell->drawer,gtk_widget_get_height(shell->bodybox));
    gtk_widget_set_visible(shell->drawer,FALSE);
    gtk_widget_set_visible(shell->bodybox,TRUE);
}


static gdouble x,y,ison;
static void on_drag_begin (GtkGestureDrag* gedture,gdouble start_x,
                           gdouble start_y,LingDesktop * self){
    x=start_x;
    y=start_y;
    ison=0;
}

static void on_drag_update(GtkGestureDrag *gesture,
                           gdouble offset_x,
                           gdouble offset_y,
                           LingDesktop * self) {
    if(ison!=0);
    else if((fabs(offset_x)>=fabs(offset_y))&&fabs(offset_x)>10&&y<gtk_widget_get_height(self->view_pager)){
        ison=1;
    }
    else if(offset_y<0){
        ison=2;
    }else if(offset_y>10){   //
        ison=3;
        if(x<gtk_widget_get_width(GTK_WIDGET(self))/2){
            ling_status_bar_set_page(LING_STATUS_BAR(shell->statusbar),SB_PAGE_NOTICE);
        }else {
            ling_status_bar_set_page(LING_STATUS_BAR(shell->statusbar),SB_PAGE_CONTROL);
        }
    }

    /*if(ison==1){
    LingOperate * op=ling_operate_get(shell.controler,"page");
    if(ling_operate_start_operating(op)){
      GtkWidget * visible = gtk_stack_get_visible_child(GTK_STACK(self->stack));
      uint pos;
      int w = gtk_widget_get_width(shell.appview);
      if(offset_x<0){
        ling_operate_set_animation_cb(op,page_left_animation,op->animation_data);
        ling_operate_set_finish_cb(op,page_left_finish,op->finish_data);
      }
      if(offset_x>0){
        ling_operate_set_animation_cb(op,page_right_animation,op->animation_data);
        ling_operate_set_finish_cb(op,page_right_finish,op->finish_data);
      }

      //gtk_widget_set_margin_start(visible,offset_x);
    }
  }*/
    if(ison==2){
        LingOperate * op=ling_operate_get(shell->controler,"drawer");
        if(ling_operate_start_operating(op)){
            gtk_widget_set_visible(shell->drawer,1);
            int h = gtk_widget_get_height(shell->bodybox);
            int top = gtk_widget_get_margin_top(shell->drawer);
            int up = h+offset_y;
            if(up<0)up=0;
            if(up>h)up=h;
            if((h-up)>h/6){
                ling_operate_set_animation_cb(op,drawerup_animation,op->animation_data);
                ling_operate_set_finish_cb(op,drawerup_finish,op->finish_data);
            }
            else{
                ling_operate_set_animation_cb(op,drawerdown_animation,op->animation_data);
                ling_operate_set_finish_cb(op,drawerdown_finish,op->finish_data);
            }
            ling_desktop_set_opacity(shell->bodybox,shell->drawer,h);
            float blur = ((((h-top)*1000)/h)*CENTER_BLUR_PX)/1000;
            ling_desktop_set_wallpaper_blur(LING_DESKTOP(shell->desktop),blur);
            gtk_widget_set_margin_top(shell->drawer,up);
        }
    }
    if(ison==3){
        LingOperate * op=ling_operate_get(shell->controler,"status_bar_drop");
        if(ling_operate_start_operating(op))
        {
            gdouble a= offset_y/300;
            ling_status_bar_set_drop_degree(LING_STATUS_BAR(shell->statusbar),a);
            if(offset_y>100){
                ling_operate_set_animation_cb(op,drop_down_animation,op->animation_data);
                ling_operate_set_finish_cb(op,drop_down_finish,op->finish_data);
            }else{
                ling_operate_set_animation_cb(op,drop_up_animation,op->animation_data);
                ling_operate_set_finish_cb(op,drop_up_finish,op->finish_data);
            }
        }
    }
}



static void on_drag_end(GtkGestureDrag *gesture,
                        gdouble offset_x,
                        gdouble offset_y,
                        LingDesktop * self) {
    ling_operate_run_animation(ling_operate_get(shell->controler,"drawer"));
    ling_operate_run_animation(ling_operate_get(shell->controler,"page"));
    ling_operate_run_animation(ling_operate_get(shell->controler,"status_bar_drop"));
}


void ling_desktop_init(LingDesktop * self){

}


GtkWidget * ling_desktop_new(){
    LingDesktop * self =  LING_DESKTOP(g_object_new(LING_TYPE_DESKTOP,NULL));

    gtk_orientable_set_orientation(GTK_ORIENTABLE(self),GTK_ORIENTATION_VERTICAL);
    self->overlay = ling_overlay_new();
    gtk_box_append(GTK_BOX(self),self->overlay);

    //桌面(bodybox)
    self->bodybox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    ling_overlay_add_layer(LING_OVERLAY(self->overlay),self->bodybox,LAYER_BODYBOX);
    shell->bodybox = self->bodybox;

    //设置页面style(后续改成根据显示器大小自动调整)
    self->style_info.column_num= 4;
    self->style_info.row_num = 6;
    self->style_info.column_space = 50;
    self->style_info.row_space = 50;
    self->style_info.icon_size = 64;

    //新建页面
    self->view_pager = ling_view_pager_new();
    ling_view_pager_set_page_cycle(LING_VIEW_PAGER(self->view_pager),FALSE);

    //底部dock创建
    GtkWidget * dock_box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    //gtk_widget_set_vexpand(dock_box,1);
    gtk_widget_set_hexpand(dock_box,1);
    self->dock = ling_fixed_view_new(LING_FIXED_VIEW_ARRANGE_GRID,4,1,self->style_info.column_space,self->style_info.row_space);
    int * drop_data=malloc(sizeof(int));
    *drop_data=0;   //不显示文字
    ling_fixed_view_set_item_drop_data(LING_FIXED_VIEW(self->dock),drop_data);
    ling_fixed_view_set_margin(LING_FIXED_VIEW(self->dock),10,20,20,20);


    gtk_widget_set_hexpand(self->dock,1);

    gtk_box_append(GTK_BOX(dock_box),self->dock);

    gtk_widget_add_css_class(dock_box,"desktop-dock");


    //添加控件进box
    gtk_box_append(GTK_BOX(self->bodybox),self->view_pager);
    gtk_box_append(GTK_BOX(self->bodybox),dock_box);


    //抽屉
    self->drawer = ling_app_drawer_new();
    gtk_box_set_homogeneous(GTK_BOX(self->drawer),1);
    gtk_widget_set_vexpand(self->drawer, FALSE);
    shell->drawer = self->drawer;

    //添加dock的应用(测试)
    GList * app_now = shell->app_info;
    app_now = shell->app_info;
    for(int r=1;r<=10;r++){
        for(int c=1;c<=5;c++){
            app_info * info = (app_info*)app_now->data;
            ling_app_drawer_add_item(LING_APP_DRAWER(self->drawer),info,c,r,1,1);
            app_now=app_now->next;
            if(app_now==NULL)break;
        }
    }

    ling_overlay_add_layer(LING_OVERLAY(self->overlay),shell->drawer,LAYER_DRAWER);
    gtk_widget_set_margin_top(shell->drawer,1000);
    //gtk_widget_set_visible(shell->drawer,0);


    //壁纸
    ling_desktop_set_wallpaper(self,"/home/gzx/Pictures/wallpaper3.png");

    //页面拖拽手势
    LingOperate * op_drawerup=ling_operate_add(shell->controler,"drawer",
                                               drawerdown_animation,shell,
                                               NULL,NULL,
                                               drawerdown_finish,shell);

    GtkGesture * drag = gtk_gesture_drag_new();
    gtk_widget_add_controller(GTK_WIDGET(self->bodybox), GTK_EVENT_CONTROLLER(drag));
    g_signal_connect(drag, "drag-begin", G_CALLBACK(on_drag_begin), self);
    g_signal_connect(drag, "drag-update", G_CALLBACK(on_drag_update), self);
    g_signal_connect(drag, "drag-end", G_CALLBACK(on_drag_end), self);

    GtkGesture * swipe = gtk_gesture_swipe_new();
    gtk_widget_add_controller(GTK_WIDGET(self->bodybox), GTK_EVENT_CONTROLLER(swipe));
    g_signal_connect(swipe,"swipe",G_CALLBACK(ling_operate_swipe_cb),op_drawerup);

    //self->data_saver = ling_data_saver_new();
    //ling_data_saver_save_pages_to_db(self->data_saver,LING_VIEW_PAGER(self->view_pager));

    ling_desktop_load_page(self);

    //g_timeout_add(500,f_timeout,self);
    return GTK_WIDGET(self);
}


void ling_desktop_load_page(LingDesktop * self){
   // if(!ling_app_view_pager_load_from_data(self)){
        g_print("NEW!\n");
        //ling_desktop_add_page_test(self);
        GtkWidget * app_page = ling_app_view_page_new(self->style_info);
        // for(int i=1;i<=6;i++){
        //     GtkWidget * item = gtk_label_new("AAAA");
        //     gtk_widget_set_hexpand(item,TRUE);
        //     gtk_widget_add_css_class(item,"nolabel");
        //     ling_app_view_page_add_item(LING_APP_VIEW_PAGE(app_page),item,1,i,4,1);
        // }
        //ling_view_pager_add_page(LING_VIEW_PAGER(self->view_pager),app_page);

        ling_desktop_add_page_test(self);
        ling_desktop_add_page_test(self);
        ling_desktop_add_page_test(self);
        ling_desktop_add_page_test(self);
        GList * app_now;
        for(int i=1;i<=24*1;i++)app_now=shell->app_info;
        for(int i=1;i<=4;i++){
            app_info * info=app_now->data;
            ling_desktop_dock_add_item(self,info,i);
            app_now=app_now->next;
        }
    //}
}

void ling_desktop_add_page_test(LingDesktop * self){
    GtkWidget * app_page = ling_app_view_page_new(self->style_info);
    ling_desktop_page_load_test(self,LING_APP_VIEW_PAGE(app_page));
    ling_view_pager_add_page(LING_VIEW_PAGER(self->view_pager),app_page);
}

void ling_desktop_set_wallpaper(LingDesktop * self,const char * path){
    self->wallpaper = ling_wallpaper_new(path);
    ling_overlay_add_layer(LING_OVERLAY(self->overlay),self->wallpaper,LAYER_WALLPAPER);
}


void ling_desktop_set_opacity(GtkWidget * body,GtkWidget * up,uint win_height){
    //全程渐变
    gdouble up_h = (gdouble)gtk_widget_get_margin_top(up);
    gdouble body_op = up_h/win_height;
    gdouble up_op = 1.0 - body_op;
    gtk_widget_set_opacity(body,body_op);
    gtk_widget_set_opacity(up,up_op);
}

void ling_desktop_set_wallpaper_blur(LingDesktop *self,uint blur){
    GString * str=g_string_new("");
    g_string_printf(str,"picture,box { filter: blur(%dpx); }",blur);
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
                                    str->str, -1);
    gtk_style_context_add_provider(gtk_widget_get_style_context(self->wallpaper),
                                   GTK_STYLE_PROVIDER(provider),GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    gtk_style_context_add_provider(gtk_widget_get_style_context(self->bodybox),
                                   GTK_STYLE_PROVIDER(provider),GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_free(str);
    g_object_unref(provider);
    self->blur = blur;
}

uint ling_desktop_get_wallpaper_blur(LingDesktop *self){
    return self->blur;
}


static void fv_data_save(GKeyFile *keyfile,const char * group,LingFixedView * fixed_view){
    int grid_column,grid_row;
    char page_num[100],grid_c[100],grid_r[100];
    ling_fixed_view_get_grid_size(fixed_view,&grid_column,&grid_row);
    sprintf(grid_c,"%d",grid_column);
    sprintf(grid_r,"%d",grid_row);
    ling_data_saver_save(DESKTOP_ICON_INI_PATH,keyfile,group,"size",grid_c,grid_r,NULL);

    //遍历fixed_view下所有成员
    for(GtkWidget * w=gtk_widget_get_first_child(gtk_widget_get_first_child(GTK_WIDGET(fixed_view)));w!=NULL;w=gtk_widget_get_next_sibling(w)){
        uint column,row,width,height;
        ling_fixed_view_item_get_grid_pos(LING_FIXED_VIEW_ITEM(w),&column,&row);
        ling_fixed_view_item_get_size(LING_FIXED_VIEW_ITEM(w),&width,&height);

        //fixed_view_item内容为ling_app_item,获取app_item的信息
        GtkWidget * app_item=ling_fixed_view_item_get_content(LING_FIXED_VIEW_ITEM(w));
        app_info * app_info = ling_app_item_get_app_info(LING_APP_ITEM(app_item));

        //以坐标来命名区别不同图标
        GString * key_name = g_string_new("");
        g_string_printf(key_name,"pos%d%d",column,row);
        char c[100],r[100],w[100],h[100];
        sprintf(c,"%d",column);
        sprintf(r,"%d",row);
        sprintf(w,"%d",width);
        sprintf(h,"%d",height);
        ling_data_saver_save(DESKTOP_ICON_INI_PATH,keyfile,group,key_name->str,c,r,w,h,app_info->desktop_path->str,NULL);
        g_free(key_name);
    }
}

//存储桌面应用页面的数据
void ling_app_view_pager_save_data(LingDesktop * self){
    GKeyFile *keyfile = g_key_file_new();
    //写入桌面的基本信息
    int num = ling_view_pager_get_page_num(LING_VIEW_PAGER(self->view_pager));
    char page_num[100],grid_c[100],grid_r[100];
    sprintf(page_num,"%d",num);
    ling_data_saver_save(DESKTOP_ICON_INI_PATH,keyfile,"pager_info","page_num",page_num,NULL);

    //写入各页面的应用图标信息
    for(int  i=1;i<=num;i++){
        GString * group = g_string_new("");
        g_string_printf(group,"page%d",i);
        GtkWidget * page=ling_view_pager_get_page_by_pos(LING_VIEW_PAGER(self->view_pager),i);
        GtkWidget * fixed_view = ling_app_view_page_get_fixed_view(LING_APP_VIEW_PAGE(page));
        fv_data_save(keyfile,group->str,LING_FIXED_VIEW(fixed_view));
        g_free(group);
    }

    //添加dock的
    fv_data_save(keyfile,"dock",LING_FIXED_VIEW(self->dock));
}

static void fv_data_load(LingDesktop *self,const char * page_name){
    int arg_num=0,column,row;
    char ** data=ling_data_saver_get(DESKTOP_ICON_INI_PATH,page_name,"size",&arg_num);
    if(arg_num!=2)return;
    column=atoi(data[0]);
    row=atoi(data[1]);
    ling_data_saver_ret_free(data,arg_num);

    //添加新页
    GtkWidget * app_page = ling_app_view_page_new(self->style_info);
    ling_view_pager_add_page(LING_VIEW_PAGER(self->view_pager),app_page);
    //行列
    for(int r=1;r<=row;r++){
        for(int c=1;c<=column;c++){
            char pos_name[30];
            sprintf(pos_name,"pos%d%d",c,r);
            data=ling_data_saver_get(DESKTOP_ICON_INI_PATH,page_name,pos_name,&arg_num);
            if(data==NULL)continue; //有的位置可能没有图标
            //g_print("item:%s %s %s %s %s\n",data[0],data[1],data[2],data[3],data[4]);

            char * desktop_file_path = data[4];
            int item_c,item_r,item_w,item_h;
            item_c=atoi(data[0]);
            item_r=atoi(data[1]);
            item_w=atoi(data[2]);
            item_h=atoi(data[3]);

            app_info * info = ling_shell_get_app_info(desktop_file_path);
            GtkWidget * item = ling_app_item_new();
            ling_app_item_set(LING_APP_ITEM(item),info,self->style_info.icon_size,TRUE);
            ling_app_view_page_add_item(LING_APP_VIEW_PAGE(app_page),item,
                                        item_c,item_r,item_w,item_h);

            ling_data_saver_ret_free(data,arg_num);
        }
    }
}

static void fv_data_load_dock(LingDesktop *self){
    int arg_num=0,column,row;
    char ** data=ling_data_saver_get(DESKTOP_ICON_INI_PATH,"dock","size",&arg_num);
    if(arg_num!=2)return;
    column=atoi(data[0]);
    row=atoi(data[1]);
    ling_data_saver_ret_free(data,arg_num);

    //行列
    for(int r=1;r<=row;r++){
        for(int c=1;c<=column;c++){
            char pos_name[30];
            sprintf(pos_name,"pos%d%d",c,r);
            data=ling_data_saver_get(DESKTOP_ICON_INI_PATH,"dock",pos_name,&arg_num);
            if(data==NULL)continue; //有的位置可能没有图标
            //g_print("item:%s %s %s %s %s\n",data[0],data[1],data[2],data[3],data[4]);

            char * desktop_file_path = data[4];
            int item_c,item_r,item_w,item_h;
            item_c=atoi(data[0]);
            item_r=atoi(data[1]);
            item_w=atoi(data[2]);
            item_h=atoi(data[3]);

            ling_desktop_dock_add_item(LING_DESKTOP(self),ling_shell_get_app_info(desktop_file_path),
                                            item_c);

            ling_data_saver_ret_free(data,arg_num);
        }
    }
}

//获取桌面应用页面的数据
int ling_app_view_pager_load_from_data(LingDesktop * self){
    GKeyFile *keyfile = g_key_file_new();
    int arg_num=0;

    //获取基础数据
    char ** data = ling_data_saver_get(DESKTOP_ICON_INI_PATH,"pager_info","page_num",&arg_num);
    if(data==NULL)return 0;
    int num = atoi(data[0]);
    int column,row;
    ling_data_saver_ret_free(data,arg_num);

    //页面
    for(int p=1;p<=num;p++){
        //获取页面的信息
        char page_name[30];
        sprintf(page_name,"page%d",p);
        fv_data_load(self,page_name);
    }

    fv_data_load_dock(self);

    return 1;
}
