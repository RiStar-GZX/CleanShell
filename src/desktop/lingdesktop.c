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


// void drawerup_animation(gdouble velocity_x,gdouble velocity_y,gdouble progress,gpointer data){
//     LingShell * shell=(LingShell *)data;
//     int top = gtk_widget_get_margin_top(shell->drawer);
//     int h = gtk_widget_get_height(shell->bodybox);
//     //FIX
//     gdouble min = top/20;
//     min+=5+fabs(velocity_y)/100;
//     gtk_widget_set_margin_top(shell->drawer,top-min);
//     ling_desktop_set_opacity(shell->bodybox,shell->drawer,h);
// }



// void drawerdown_animation(gdouble velocity_x,gdouble velocity_y,gdouble progress,gpointer user_data){
//     LingShell * shell = (LingShell*)user_data;
//     int top = gtk_widget_get_margin_top(shell->drawer);
//     int h = gtk_widget_get_height(shell->bodybox);

//     if(top<h){
//         gdouble plus = (h-top)/20;
//         plus+=5+fabs(velocity_y)/100;
//         gtk_widget_set_margin_top(shell->drawer,top+plus);
//         ling_desktop_set_opacity(shell->bodybox,shell->drawer,h);
//     }/*else{
//         return LING_OPERATE_ANIMATION_REMOVE;
//     }
//     return LING_OPERATE_ANIMATION_CONTINUE;*/
// }

// void drawerup_finish(gpointer data){
//     LingShell * shell=(LingShell *)(data);
//     gtk_widget_set_opacity(shell->drawer,1.0);
//     gtk_widget_set_opacity(shell->bodybox,1.0);
//     gtk_widget_set_margin_top(shell->drawer,0);
//     gtk_widget_set_visible(shell->drawer,TRUE);
//     gtk_widget_set_visible(shell->bodybox,FALSE);
// }

// void drawerdown_finish(gpointer data){
//     LingShell * shell=(LingShell *)data;
//     gtk_widget_set_opacity(shell->drawer,1.0);
//     gtk_widget_set_opacity(shell->bodybox,1.0);
//     gtk_widget_set_margin_top(shell->drawer,gtk_widget_get_height(shell->bodybox));
//     gtk_widget_set_visible(shell->drawer,FALSE);
//     gtk_widget_set_visible(shell->bodybox,TRUE);
// }


// static void lks_ani_up(gdouble velocity_x,gdouble velocity_y,gdouble progress,gpointer user_data,uint mode){
//     if(shell->mode==SHELL_MODE_LOCKSCREEN){
//         // int level;
//         // gtk_widget_set_visible(s->main->widget,TRUE);
//         // gtk_widget_set_visible(s->sub->widget,TRUE);
//         // gtk_widget_set_margin_top(s->main->widget,-(progress/100.00f)*30);
//         // gtk_widget_set_margin_top(s->sub->widget,30-(progress/100.00f)*30);

//         // gtk_widget_set_opacity(s->main->widget,1-(progress/100));
//         // gtk_widget_set_opacity(s->sub->widget,progress/100);
//         // ling_status_bar_set_status_bar_opacity(LING_STATUS_BAR(shell->statusbar),(progress/100));
//     }
//     if(shell->mode==SHELL_MODE_DESKTOP){
//         switcher * s = (switcher*)user_data;
//         gtk_widget_set_visible(s->main->widget,TRUE);
//         gtk_widget_set_visible(s->sub->widget,TRUE);
//         gtk_widget_set_margin_top(s->main->widget,-(progress/100.00f)*30);
//         gtk_widget_set_margin_top(s->sub->widget,30-(progress/100.00f)*30);

//         gtk_widget_set_opacity(s->main->widget,1-(progress/100));
//         gtk_widget_set_opacity(s->sub->widget,progress/100);
//         ling_status_bar_set_status_bar_opacity(LING_STATUS_BAR(shell->statusbar),(progress/100));

//         ling_desktop_set_wallpaper_blur(LING_DESKTOP(shell->desktop),(1-progress/100)*20);
//     }
// }

void center_ani(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    switcher * s = user_data;
    gtk_widget_set_visible(s->sub->widget,TRUE);
    gtk_widget_set_visible(s->main->widget,TRUE);
    gtk_widget_set_margin_top(s->sub->widget,30-(args.progress/100.00f)*30);
    gtk_widget_set_margin_top(s->main->widget,-(args.progress/100.00f)*30);

    gtk_widget_set_opacity(s->sub->widget,(args.progress/100));
    gtk_widget_set_opacity(s->main->widget,1-args.progress/100);
    ling_status_bar_set_status_bar_opacity(LING_STATUS_BAR(shell->statusbar),(args.progress/100));

    if(shell->mode==SHELL_MODE_DESKTOP){
       // ling_desktop_set_wallpaper_blur(LING_DESKTOP(shell->desktop),(1-args.progress/100)*20); //IPT FIX:switch添加user_data
    }
}

// static void lks_ani2(GtkWidget * widget,LingActionArgs args,gpointer user_data){
//     switcher * s = user_data;
//     gtk_widget_set_visible(s->sub->widget,TRUE);
//     gtk_widget_set_visible(s->main->widget,TRUE);
//     gtk_widget_set_margin_top(s->sub->widget,30-(args.progress/100.00f)*30);
//     gtk_widget_set_margin_top(s->main->widget,-(args.progress/100.00f)*30);

//     if(shell->mode==SHELL_MODE_LOCKSCREEN){
//         gtk_widget_set_opacity(s->sub->widget,(args.progress/100));
//         gtk_widget_set_opacity(s->main->widget,1-args.progress/100);
//         ling_status_bar_set_status_bar_opacity(LING_STATUS_BAR(shell->statusbar),(args.progress/100));
//     }
//     //ling_desktop_set_wallpaper_blur(LING_DESKTOP(shell->desktop),(args.progress/100)*20); //IPT FIX:switch添加user_data
// }

static void drawer_ani(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    switcher * s = (switcher*)user_data;
    gtk_widget_set_visible(s->main->widget,TRUE);
    gtk_widget_set_visible(s->sub->widget,TRUE);
    gtk_widget_set_margin_top(s->main->widget,-(args.progress/100.00f)*30);
    gtk_widget_set_margin_top(s->sub->widget,30-(args.progress/100.00f)*30);

    gtk_widget_set_opacity(s->main->widget,1-(args.progress/100));
    gtk_widget_set_opacity(s->sub->widget,args.progress/100);
    //ling_status_bar_set_status_bar_opacity(LING_STATUS_BAR(shell->statusbar),(1-progress/100));

    ling_desktop_set_wallpaper_blur(LING_DESKTOP(shell->desktop),(args.progress/100)*20);
}



// static void lks_ani_down2(gdouble velocity_x,gdouble velocity_y,gdouble progress,gpointer user_data,uint mode){
//     switcher * s = (switcher*)user_data;
//     gtk_widget_set_visible(s->sub->widget,TRUE);
//     gtk_widget_set_visible(s->main->widget,TRUE);
//     gtk_widget_set_margin_top(s->sub->widget,-(progress/100.00f)*30);
//     gtk_widget_set_margin_top(s->main->widget,30-(progress/100.00f)*30);

//     gtk_widget_set_opacity(s->sub->widget,1-(progress/100));
//     gtk_widget_set_opacity(s->main->widget,progress/100);
//     //ling_status_bar_set_status_bar_opacity(LING_STATUS_BAR(shell->statusbar),(1-progress/100));

//     ling_desktop_set_wallpaper_blur(LING_DESKTOP(shell->desktop),(progress/100)*20); //IPT FIX:switch添加user_data
// }


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

    //抽屉拖拽手势
    //uint level;
    //LingOverlay * sb_overlay =  ling_status_bar_get_layer_center(LING_STATUS_BAR(shell->statusbar),&level);
    LingOperate * bodybox_op = ling_operate_add(shell->controler,LING_DESKTOP_BODYBOX_OP_NAME,self->bodybox);
    LingOperate * drawer_op  = ling_operate_add(shell->controler,"drawer_switch",self->drawer);

    ling_layer_add_switch(bodybox_op,LING_OVERLAY(self->overlay),LAYER_BODYBOX,
                          drawer_op,LING_OVERLAY(self->overlay),LAYER_DRAWER,
                          LING_ACTION_DRAG_UP,drawer_ani,ling_layer_progress,ling_layer_release,
                          ling_layer_main_finish,ling_layer_sub_finish);

    // uint level;
    // LingOverlay * sb_overlay =  ling_status_bar_get_layer_center(LING_STATUS_BAR(shell->statusbar),&level);

    // ling_layer_add_switch(ling_operate_get(shell->controler,LING_STATUSBAR_CENTERBOX_OP_NAME),sb_overlay,level,
    //                       bodybox_op,LING_OVERLAY(self->overlay),LAYER_BODYBOX,
    //                       LING_ACTION_DRAG_UP,center_ani,ling_layer_progress,ling_layer_release,
    //                       ling_layer_main_finish,ling_layer_sub_finish);

    // LingLayer * cover=ling_overlay_get_layer(LING_OVERLAY(self->overlay),LAYER_COVER);
    // LingLayer * verify=ling_overlay_get_layer(LING_OVERLAY(self->overlay),LAYER_VERIFY);
    // sb * s = malloc(sizeof(sb));
    // s->main = cover->widget;
    // s->sub = verify->widget;

    // LingOperate * op = ling_operate_add(shell->controler,"drawer",verify->widget);
    // ling_operate_add_action(op2,LING_ACTION_DRAG_DOWN,ling_overlay_layer_progress,
    //                         verify->widget,lks_ani,s,
    //                         lks_release,1,
    //                         lks_main_finish,lks_sub_finish,s);
    // ling_overlay_add_switch(LING_OVERLAY(self->overlay),LAYER_BODYBOX,
    //                         OP_UPDATE_TYPE_UP,LING_OVERLAY(self->overlay),LAYER_DRAWER,
    //                         lks_ani_up2,switch_main_finish,switch_sub_finish);

    // ling_overlay_add_switch(LING_OVERLAY(self->overlay),LAYER_DRAWER,
    //                         OP_UPDATE_TYPE_DOWN,LING_OVERLAY(self->overlay),LAYER_BODYBOX,
    //                         lks_ani_down2,switch_main_finish,switch_sub_finish);

    //self->data_saver = ling_data_saver_new();
    //ling_data_saver_save_pages_to_db(self->data_saver,LING_VIEW_PAGER(self->view_pager));

    ling_desktop_load_page(self);

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

LingOverlay * ling_desktop_get_layer_bodybox(LingDesktop * self,uint * level){
    *level = LAYER_BODYBOX;
    return LING_OVERLAY(self->overlay);
}
