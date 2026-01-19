#include "desktop.h"
#include <appdrawer.h>
#include <statusbar.h>
#include <taskswitcher.h>
#include <desktopitem.h>
#include <wm/wm.h>

#define DESKTOP_ICON_INI_PATH "~/.local/share/clean/desktop_icon.ini"

enum{
    LAYER_TOP = 0,
    LAYER_DRAWER,
    LAYER_TASK_SWITCH_BAR,
    LAYER_WM,
    LAYER_FOLDER,
    LAYER_BODYBOX,
    LAYER_WALLPAPER,
    LAYER_END = 255
};

struct _ClmDesktop{
    GtkBox parent;

    GtkWidget * overlay;
    //状态栏

    GtkWidget * folder_lay;  //ling_fixed

    //主体
    GtkWidget * bodybox;

    //页面
    GtkWidget * view_pager;

    //底栏
    GtkWidget * dock; //gtkgrid
    //GList * dock_app_items;


    GtkWidget * drawer; //抽屉

    //
    GtkWidget * wm;

    GtkWidget * guide_bar;

    uint app_num;   //只在clm_desktop_page_load_test使用(抛弃)

    style_info style_info;

    //share_info share_info; //共享信息

    GtkWidget * wallpaper;  //GtkPicture
    uint blur;

    //LingDataSaver * data_saver;
};

G_DEFINE_FINAL_TYPE(ClmDesktop,clm_desktop,GTK_TYPE_BOX)


int clm_desktop_set_apps(ClmDesktop * self,const char * apps_dir);
void clm_desktop_set_wallpaper(ClmDesktop * self,const char * path);
GtkWidget * clm_desktop_set_page(ClmDesktop * self,uint pos);
void clm_desktop_load_page(ClmDesktop * self);
void clm_desktop_add_page_test(ClmDesktop * self);

void ling_app_view_pager_save_data(ClmDesktop * self);
int ling_app_view_pager_load_from_data(ClmDesktop * self);

#define PAGE_DIRECTION_LEFT 0
#define PAGE_DIRECTION_RIGHT 1

GtkWidget * clm_desktop_get_beside_page(ClmDesktop * self,gboolean dir,uint * pos);

gboolean clm_desktop_page_load_test(ClmDesktop * self,ClmAppViewPage * page){
    GList * app = shell->app_info;
    for(int i=0;i<self->app_num;i++)app=app->next;

    for(int r=5;r<=self->style_info.row_num;r++){
        int num = self->style_info.column_num;
        // if(r==self->style_info.row_num)num-=1;
        for(int c=3;c<=num;c++){
            app_info * info = (app_info*)app->data;
            GtkWidget * item = clm_desktop_item_app_new(info,self->style_info.icon_size,TRUE);
            clm_desktop_item_app_set_runable(CLM_DESKTOP_ITEM(item),TRUE);
            clm_app_view_page_add_item(page,item,c,r,1,1);
            app=app->next;
            if(app==NULL)return 0;
            self->app_num++;
        }
        GList * list = NULL;
        for(int i=0;i<9;i++){
            app_info * info = (app_info*)app->data;
            app=app->next;
            list = g_list_append(list,info);
        }
        app=app->next;
        for(int j=5;j<=6;j++)
        for(int i=1;i<=2;i++){
            GtkWidget * item = clm_desktop_item_folder_new(LING_FOLDER(self->folder_lay),3,3,list,"folder",TRUE);
            clm_app_view_page_add_item(page,item,i,j,1,1);
        }
    }
    return 1;
}


gboolean clm_desktop_dock_add_item(ClmDesktop * self,app_info * info,uint column){
    ClmDesktopItem * item = CLM_DESKTOP_ITEM(
    clm_desktop_item_app_new(info,self->style_info.icon_size,FALSE));
    clm_desktop_item_app_set_runable(CLM_DESKTOP_ITEM(item),TRUE);

    ling_grid_attach(LING_GRID(self->dock),GTK_WIDGET(item),column,1,1,1);

    return 1;
}

static void clm_desktop_dispose(GObject *object){
    ClmDesktop * self = CLM_DESKTOP(object);
//    ling_data_saver_save_pages_to_db(self->data_saver,LING_VIEW_PAGER(self->view_pager));
    ling_app_view_pager_save_data(self);
    g_print("END\n");

}

static void clm_desktop_class_init(ClmDesktopClass * klass){
    GObjectClass * object_class = G_OBJECT_CLASS(klass);
    object_class->dispose = clm_desktop_dispose;
}

void center_ani(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    switcher * s = user_data;
    gtk_widget_set_visible(s->sub->widget,TRUE);
    gtk_widget_set_visible(s->main->widget,TRUE);
    gtk_widget_set_margin_top(s->sub->widget,30-(args.progress/100.00f)*30);
    gtk_widget_set_margin_top(s->main->widget,-(args.progress/100.00f)*30);

    gtk_widget_set_opacity(s->sub->widget,(args.progress/100));
    gtk_widget_set_opacity(s->main->widget,1-args.progress/100);
    cl_status_bar_set_status_bar_opacity(CL_STATUS_BAR(shell->statusbar),(args.progress/100));

    if(shell->mode==SHELL_MODE_DESKTOP){

       // clm_desktop_set_wallpaper_blur(clm_desktop(shell->desktop),(1-args.progress/100)*20); //IPT FIX:switch添加user_data
    }
}

static void drawer_ani(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    switcher * s = (switcher*)user_data;
    gtk_widget_set_visible(s->main->widget,TRUE);
    gtk_widget_set_visible(s->sub->widget,TRUE);
    //gtk_widget_set_margin_top(s->main->widget,-(args.progress/100.00f)*30);
    ling_widget_scale(s->main->widget,1-0.1*(args.progress/100));
    gtk_widget_set_margin_top(s->sub->widget,30-(args.progress/100.00f)*30);

    LingLayer * switcher,*bar;

    clm_desktop_get_layer_task_switch_bar(CLM_DESKTOP(shell->desktop),&bar);
    gtk_widget_set_opacity(bar->widget,1-(args.progress*2/100));
    gtk_widget_set_opacity(s->main->widget,1-(args.progress*2/100));
    gtk_widget_set_opacity(s->sub->widget,args.progress*2/100-0.5);

    //ling_status_bar_set_status_bar_opacity(LING_STATUS_BAR(shell->statusbar),(1-args.progress/100));
    //clm_desktop_view_pager_resize(CLM_DESKTOP(shell->desktop));
    clm_desktop_set_wallpaper_blur(CLM_DESKTOP(shell->desktop),(args.progress/100)*20);
}

void desktop_load_ani(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    ClmDesktop * self = CLM_DESKTOP(widget);
    ling_widget_scale(GTK_WIDGET(self),4-3*(args.progress/100));
}

void desktop_load_end(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    ClmDesktop * self = CLM_DESKTOP(widget);
    ling_widget_scale(GTK_WIDGET(self),1);
}


void clm_desktop_init(ClmDesktop * self){

    gtk_orientable_set_orientation(GTK_ORIENTABLE(self),GTK_ORIENTATION_VERTICAL);
    self->overlay = ling_overlay_new();
    gtk_box_append(GTK_BOX(self),self->overlay);

    //桌面(bodybox)
    self->bodybox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    ling_overlay_add_layer(LING_OVERLAY(self->overlay),self->bodybox,LAYER_BODYBOX);
    //shell->bodybox = self->bodybox;

    //窗口管理器
    self->wm = cl_wm_new();
    shell->wm = self->wm;
    ling_overlay_add_layer(LING_OVERLAY(self->overlay),self->wm,LAYER_WM);
    gtk_widget_set_visible(self->wm,FALSE);

    //设置页面style(后续改成根据显示器大小自动调整)
    self->style_info.column_num= 4;
    self->style_info.row_num = 6;
    self->style_info.column_space = 40;
    self->style_info.top_space = 40;
    self->style_info.row_space = 0;
    self->style_info.icon_size = 64;

    //新建页面
    self->view_pager = ling_view_pager_new_with_op(TRUE,CLM_DESKTOP_VIEWPAGER_OP_NAME);
    ling_view_pager_set_page_cycle(LING_VIEW_PAGER(self->view_pager),FALSE);

    //底部dock创建
    GtkWidget * dock_box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    gtk_widget_set_vexpand(dock_box,FALSE);
    gtk_widget_set_hexpand(dock_box,TRUE);
    self->dock = ling_grid_new(4,1,self->style_info.column_space,self->style_info.row_space);
    gtk_widget_set_hexpand(self->dock,1);

    gtk_widget_set_margin_top(self->dock,10);
    gtk_widget_set_margin_bottom(self->dock,10);
    //gtk_widget_set_valign(self->dock,GTK_ALIGN_END);
    //gtk_widget_set_valign(dock_box,GTK_ALIGN_END);

    gtk_box_append(GTK_BOX(dock_box),self->dock);

    gtk_widget_add_css_class(dock_box,"desktop-dock");


    //添加控件进box
    gtk_box_append(GTK_BOX(self->bodybox),self->view_pager);
    gtk_box_append(GTK_BOX(self->bodybox),dock_box);

    //抽屉
    self->drawer = clm_app_drawer_new();
    gtk_box_set_homogeneous(GTK_BOX(self->drawer),1);
    gtk_widget_set_vexpand(self->drawer, FALSE);
    //shell->drawer = self->drawer;



    //导航
    GtkWidget * ts = clm_task_switcher_new();
    self->guide_bar = clm_task_switcher_get_bar(CLM_TASK_SWITCHER(ts));
    ling_overlay_add_layer(LING_OVERLAY(self->overlay),
                           self->guide_bar,LAYER_TASK_SWITCH_BAR);


    //添加dock的应用(测试)
    GList * app_now = shell->app_info;
    app_now = shell->app_info;
    for(int r=1;r<=10;r++){
        for(int c=1;c<=5;c++){
            app_info * info = (app_info*)app_now->data;
            clm_app_drawer_add_item(LING_APP_DRAWER(self->drawer),info,c,r,1,1);
            app_now=app_now->next;
            if(app_now==NULL)break;
        }
    }

    ling_overlay_add_layer(LING_OVERLAY(self->overlay),self->drawer,LAYER_DRAWER);
    gtk_widget_set_margin_top(self->drawer,1000);
    //gtk_widget_set_visible(shell->drawer,0);


    //壁纸
    clm_desktop_set_wallpaper(self,"/home/gzx/Pictures/wallpaper3.png");

    //抽屉拖拽手势
    LingOperate * bodybox_op = ling_operate_add(shell->controler,CLM_DESKTOP_BODYBOX_OP_NAME,self->bodybox);
    LingOperate * drawer_op  = ling_operate_add(shell->controler,"drawer_switch",self->drawer);

    ling_layer_add_switch(bodybox_op,LING_OVERLAY(self->overlay),LAYER_BODYBOX,
                          drawer_op,LING_OVERLAY(self->overlay),LAYER_DRAWER,
                          LING_ACTION_DRAG_UP,drawer_ani,ling_layer_progress,ling_layer_release,
                          ling_layer_main_finish,ling_layer_sub_finish);


    //self->data_saver = ling_data_saver_new();
    //ling_data_saver_save_pages_to_db(self->data_saver,LING_VIEW_PAGER(self->view_pager));

    //文件夹层
    self->folder_lay = ling_folder_new();
    ling_overlay_add_layer(LING_OVERLAY(self->overlay),self->folder_lay,LAYER_FOLDER);

    clm_desktop_load_page(self);

    //刚进入桌面时候的动画

    LingOperate * op = ling_operate_add(shell->controler,"desktop_load",self->bodybox);
    ling_operate_add_action(op,LING_ACTION_EMIT,
                            NULL,NULL,
                            desktop_load_ani,NULL,
                            NULL,NULL,
                            NULL,desktop_load_end,NULL);
}


GtkWidget * clm_desktop_new(){
    return g_object_new(CLM_TYPE_DESKTOP,NULL);
}


void clm_desktop_load_page(ClmDesktop * self){
   // if(!ling_app_view_pager_load_from_data(self)){
        g_print("NEW!\n");
        //clm_desktop_add_page_test(self);
        GtkWidget * app_page = clm_app_view_page_new(self->style_info);
        // for(int i=1;i<=6;i++){
        //     GtkWidget * item = gtk_label_new("AAAA");
        //     gtk_widget_set_hexpand(item,TRUE);
        //     gtk_widget_add_css_class(item,"nolabel");
        //     ling_app_view_page_add_item(LING_APP_VIEW_PAGE(app_page),item,1,i,4,1);
        // }
        //ling_view_pager_add_page(LING_VIEW_PAGER(self->view_pager),app_page);

        clm_desktop_add_page_test(self);
        clm_desktop_add_page_test(self);
        clm_desktop_add_page_test(self);
        clm_desktop_add_page_test(self);
        GList * app_now;
        for(int i=1;i<=24*1;i++)app_now=shell->app_info;
        for(int i=1;i<=4;i++){
            app_info * info=app_now->data;
            clm_desktop_dock_add_item(self,info,i);
            app_now=app_now->next;
        }
    //}
}

void clm_desktop_add_page_test(ClmDesktop * self){
    GtkWidget * app_page = clm_app_view_page_new(self->style_info);
    clm_desktop_page_load_test(self,CLM_APP_VIEW_PAGE(app_page));
    ling_view_pager_add_page(LING_VIEW_PAGER(self->view_pager),app_page);
}

void clm_desktop_set_wallpaper(ClmDesktop * self,const char * path){
    self->wallpaper = ling_wallpaper_new(path);
    ling_overlay_add_layer(LING_OVERLAY(self->overlay),self->wallpaper,LAYER_WALLPAPER);
}

void clm_desktop_set_wallpaper_blur(ClmDesktop *self,uint blur){
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

uint clm_desktop_get_wallpaper_blur(ClmDesktop *self){
    return self->blur;
}

void clm_desktop_hide_body_and_set_blur(ClmDesktop *self,gdouble progress,uint blur){
    clm_desktop_set_wallpaper_blur(self,blur*(100/progress));
    gtk_widget_set_opacity(self->bodybox,1-(100/progress));
}




// static void grid_data_save(GKeyFile *keyfile,const char * group,LingGrid * grid){
//     int grid_column,grid_row;
//     char page_num[100],grid_c[100],grid_r[100];
//     ling_fixed_view_get_grid_size(grid,&grid_column,&grid_row);
//     sprintf(grid_c,"%d",grid_column);
//     sprintf(grid_r,"%d",grid_row);
//     ling_data_saver_save(DESKTOP_ICON_INI_PATH,keyfile,group,"size",grid_c,grid_r,NULL);

//     //遍历fixed_view下所有成员
//     //for(int r=1;r<grid)
//         uint column,row,width,height;
//         ling_fixed_view_item_get_grid_pos(LING_FIXED_VIEW_ITEM(w),&column,&row);
//         ling_fixed_view_item_get_size(LING_FIXED_VIEW_ITEM(w),&width,&height);

//         //fixed_view_item内容为ling_app_item,获取app_item的信息
//         GtkWidget * app_item=ling_fixed_view_item_get_content(LING_FIXED_VIEW_ITEM(w));
//         app_info * app_info = ling_app_item_get_app_info(LING_APP_ITEM(app_item));

//         //以坐标来命名区别不同图标
//         GString * key_name = g_string_new("");
//         g_string_printf(key_name,"pos%d%d",column,row);
//         char c[100],r[100],w[100],h[100];
//         sprintf(c,"%d",column);
//         sprintf(r,"%d",row);
//         sprintf(w,"%d",width);
//         sprintf(h,"%d",height);
//         ling_data_saver_save(DESKTOP_ICON_INI_PATH,keyfile,group,key_name->str,c,r,w,h,app_info->desktop_path->str,NULL);
//         g_free(key_name);
//     }
// }

//存储桌面应用页面的数据
void ling_app_view_pager_save_data(ClmDesktop * self){
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
        GtkWidget * fixed_view = clm_app_view_page_get_fixed_view(CLM_APP_VIEW_PAGE(page));
        //grid_data_save(keyfile,group->str,LING_FIXED_VIEW(fixed_view));
        g_free(group);
    }

    //添加dock的
    //grid_data_save(keyfile,"dock",LING_FIXED_VIEW(self->dock));
}

static void fv_data_load(ClmDesktop *self,const char * page_name){
    int arg_num=0,column,row;
    char ** data=ling_data_saver_get(DESKTOP_ICON_INI_PATH,page_name,"size",&arg_num);
    if(arg_num!=2)return;
    column=atoi(data[0]);
    row=atoi(data[1]);
    ling_data_saver_ret_free(data,arg_num);

    //添加新页
    GtkWidget * app_page = clm_app_view_page_new(self->style_info);
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

            app_info * info = clm_shell_get_app_info(desktop_file_path);
            GtkWidget * item = clm_desktop_item_app_new(info,self->style_info.icon_size,TRUE);
            clm_app_view_page_add_item(CLM_APP_VIEW_PAGE(app_page),item,
                                        item_c,item_r,item_w,item_h);

            ling_data_saver_ret_free(data,arg_num);
        }
    }
}

static void fv_data_load_dock(ClmDesktop *self){
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

            clm_desktop_dock_add_item(CLM_DESKTOP(self),clm_shell_get_app_info(desktop_file_path),
                                            item_c);

            ling_data_saver_ret_free(data,arg_num);
        }
    }
}

//获取桌面应用页面的数据
int ling_app_view_pager_load_from_data(ClmDesktop * self){
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

LingOverlay * clm_desktop_get_layer_bodybox(ClmDesktop * self,LingLayer ** layer){
    *layer = ling_overlay_get_layer(LING_OVERLAY(self->overlay),LAYER_BODYBOX);
    return LING_OVERLAY(self->overlay);
}


// LingOverlay * clm_desktop_get_layer_wm(ClmDesktop * self,LingLayer ** layer){
//     *layer = ling_overlay_get_layer(LING_OVERLAY(self->overlay),LAYER_WM);
//     return LING_OVERLAY(self->overlay);
// }

LingOverlay * clm_desktop_get_layer_task_switch_bar(ClmDesktop * self,LingLayer ** layer){
    *layer = ling_overlay_get_layer(LING_OVERLAY(self->overlay),LAYER_TASK_SWITCH_BAR);
    return LING_OVERLAY(self->overlay);
}

// void clm_desktop_view_pager_resize(ClmDesktop * self){
//     ling_view_pager_resize(LING_VIEW_PAGER(self->view_pager));
// }

