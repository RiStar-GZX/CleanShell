#include "mobileshell.h"
#include <wordexp.h>
#include <statusbar.h>
#include <lockscreen.h>
#include <desktop.h>
#include <lingkeyborad.h>
#include <lingsysinfo.h>
#include <lingtimeviewer.h>
#include <lingfixed.h>

ClmShell * shell;

enum{
    LAYER_TOP = 0,
    LAYER_STATUSBAR,

    LAYER_LOCKSCREEN,
    LAYER_DESKTOP,

    LAYER_WALLPAPER,
    LAYER_END = 255
};

static void update_time_info_cb(GList * objs,gpointer user_data){
    char * str = ling_sys_info_get_time_label();
    for(GList * l=objs;l!=NULL;l=l->next){
        LingTimeViewer * tv = (LingTimeViewer*)l->data;
        ling_time_viewer_set_time(tv,str);
    }
    free(str);
}

/*------------------------------------------------------------------------------------*/
G_DEFINE_FINAL_TYPE(ClmShell,clm_shell,GTK_TYPE_BOX);

app_info * clm_shell_get_app_info(const char * filepath){
    app_info * info = g_malloc(sizeof(app_info));
    GKeyFile *keyfile = g_key_file_new();
    g_key_file_load_from_file(keyfile, filepath, G_KEY_FILE_NONE, NULL);
    info->no_display=0;
    info->desktop_path = g_string_new(filepath);

    //排除不显示的应用
    char * str = g_key_file_get_string(keyfile, "Desktop Entry", "NoDisplay", NULL);
    if(str!=NULL){
        if(strcmp(str,"true")==0){
            info->no_display=1;
            free(info);
            return NULL;
        }
    }

    //添加基本信息
    info->name = g_key_file_get_string(keyfile, "Desktop Entry", "Name[zh_CN]", NULL);
    if(info->name==NULL)info->name =g_key_file_get_string(keyfile, "Desktop Entry", "Name", NULL);
    info->icon = g_key_file_get_string(keyfile, "Desktop Entry", "Icon", NULL);
    info->exec = g_key_file_get_string(keyfile, "Desktop Entry", "Exec", NULL);

    //self->shell.app_info = g_list_append(self->shell.app_info,info);
    if(info==NULL){
        g_print("sb\n");
    }
    return info;
}

int clm_shell_load_apps(ClmShell * self,const char * apps_dir){
    //添加的应用来源 /usr/share/application  ~/.local/share/applications/
    //补全路径
    GString * path = g_string_new("");
    wordexp_t exp;
    wordexp(apps_dir,&exp,0);
    g_string_printf(path,"%s",exp.we_wordv[0]);

    if(!g_file_test(path->str, G_FILE_TEST_EXISTS))return -1;

    //遍历所有文件
    GFile * file = g_file_new_for_path(path->str);
    GFileInfo* info = g_file_query_info(file,G_FILE_ATTRIBUTE_STANDARD_TYPE,
                                        G_FILE_QUERY_INFO_NONE,NULL,NULL);
    GFileEnumerator *enumerator;
    GError *error = NULL;

    enumerator = g_file_enumerate_children(file,
                                           "standard::*",
                                           G_FILE_QUERY_INFO_NONE,
                                           NULL, &error);

    if (error) {
        g_printerr("Error: %s\n", error->message);
        g_error_free(error);
        return -1;
    }

    GString * fullpath = g_string_new("");
    while ((info = g_file_enumerator_next_file(enumerator, NULL, &error)) != NULL) {
        const char * filename = g_file_info_get_name(info);
        const char * typename=NULL;
        for(int c=strlen(filename);c>0;c--){
            //文件名第一个字符为'.',为隐藏文件
            if(filename[c-1]=='.'&&c-1!=0){
                typename=filename+c;
                break;
            }
        }
        if(typename==NULL)continue;
        if(!g_file_info_get_is_hidden(info)&&strcmp("desktop",typename)==0){
            g_string_printf(fullpath,"%s%s",path->str,g_file_info_get_name(info));
            app_info * infodd = clm_shell_get_app_info(fullpath->str);
            if(infodd!=NULL)self->app_info = g_list_append(self->app_info,infodd);
        }
    }
    g_object_unref(enumerator);
    g_object_unref(file);
    return 1;
}

void clm_shell_class_init(ClmShellClass * klass){

}

void clm_shell_lock_screen_passed(ClLockScreen * lockscreen,gpointer user_data){
    ClmShell * self = (ClmShell *)user_data;
    gtk_widget_set_visible(self->lockscreen,FALSE);
    gtk_widget_set_opacity(shell->statusbar,1);
    self->mode = SHELL_MODE_DESKTOP;
    gtk_widget_set_visible(self->desktop,TRUE);
}

void clm_shell_init(ClmShell * self){

}

typedef struct switch3{
    LingLayer * statusbar;
    LingLayer * desktop;
    LingLayer * lockscreen;
}switch3;

static void statusbar_center_ani(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    switch3 * s = user_data;
    GtkWidget * main=s->statusbar->widget,*sub;

    LingLayer * switcher,*bar;
    cl_status_bar_get_layer_bar(CL_STATUS_BAR(shell->statusbar),&switcher);
    gtk_widget_set_visible(switcher->widget,TRUE);

    if(shell->mode==SHELL_MODE_LOCKSCREEN){
        sub=s->lockscreen->widget;
        cl_lock_screen_set_wallpaper_blur(CL_LOCK_SCREEN(shell->lockscreen),(1-args.progress/100)*20);
    }
    if(shell->mode==SHELL_MODE_DESKTOP){
        sub=s->desktop->widget;
        clm_desktop_set_wallpaper_blur(CLM_DESKTOP(shell->desktop),(1-args.progress/100)*20);

        clm_desktop_get_layer_task_switcher(CLM_DESKTOP(shell->desktop),&switcher);
        clm_desktop_get_layer_task_switch_bar(CLM_DESKTOP(shell->desktop),&bar);
        gtk_widget_set_opacity(switcher->widget,(args.progress/100));
        gtk_widget_set_opacity(bar->widget,(args.progress/100));
    }

    gtk_widget_set_visible(main,TRUE);
    gtk_widget_set_visible(sub,TRUE);
    gtk_widget_set_margin_top(main,-(args.progress/100.00f)*30);
    gtk_widget_set_margin_top(sub,30-(args.progress/100.00f)*30);

    gtk_widget_set_opacity(sub,(args.progress/100));
    gtk_widget_set_opacity(main,1-args.progress/100);
    cl_status_bar_set_status_bar_opacity(CL_STATUS_BAR(shell->statusbar),(args.progress/100));
}

void statusbar_center_s_finish(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    switch3 * s = user_data;
    GtkWidget * main=s->statusbar->widget,*sub;
    if(shell->mode==SHELL_MODE_LOCKSCREEN){
        sub=s->lockscreen->widget;
        //gtk_widget_set_visible(s->desktop->widget,TRUE);
    }
    if(shell->mode==SHELL_MODE_DESKTOP){
        sub=s->desktop->widget;
        //gtk_widget_set_visible(s->lockscreen->widget,TRUE);
    }

    LingLayer * layer;
    cl_status_bar_get_layer_bar(CL_STATUS_BAR(shell->statusbar),&layer);
    gtk_widget_set_visible(layer->widget,FALSE);

    gtk_widget_set_visible(main,TRUE);
    gtk_widget_set_visible(sub,FALSE);
    gtk_widget_set_margin_top(main,0);
    gtk_widget_set_margin_top(sub,0);
    gtk_widget_set_opacity(main,1);
    gtk_widget_set_opacity(sub,1);
}

void statusbar_center_e_finish(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    switch3 * s = user_data;
    GtkWidget * main=s->statusbar->widget,*sub;
    if(shell->mode==SHELL_MODE_LOCKSCREEN){
        sub=s->lockscreen->widget;
        //gtk_widget_set_visible(s->desktop->widget,TRUE);
    }
    if(shell->mode==SHELL_MODE_DESKTOP){
        sub=s->desktop->widget;
        //gtk_widget_set_visible(s->lockscreen->widget,TRUE);
    }
    LingLayer * layer;
    cl_status_bar_get_layer_bar(CL_STATUS_BAR(shell->statusbar),&layer);
    gtk_widget_set_visible(layer->widget,TRUE);

    gtk_widget_set_visible(main,FALSE);
    gtk_widget_set_visible(sub,TRUE);
    gtk_widget_set_margin_top(main,0);
    gtk_widget_set_margin_top(sub,0);
    gtk_widget_set_opacity(main,1);
    gtk_widget_set_opacity(sub,1);
}

static void test(ClmShell * shell);

void clm_shell_setting(ClmShell * self){
    // test(self);
    // return;

    //加载所有应用
    clm_shell_load_apps(self,"/usr/share/applications/");
    clm_shell_load_apps(self,"~/.local/share/applications/");

    //操作控制器
    self->controler = ling_operate_controler_new(144);

    //更新器
    self->updater = ling_sys_info_updater_new();
    ling_sys_info_updater_add_type(self->updater,INFO_UPDATER_TIME,update_time_info_cb,NULL);

    //层叠
    self->lingoverlay = ling_overlay_new();
    gtk_widget_set_hexpand(self->lingoverlay,1);
    gtk_widget_set_vexpand(self->lingoverlay,1);
    gtk_box_append(GTK_BOX(self),self->lingoverlay);

    //状态栏
    self->statusbar = cl_status_bar_new();
    gtk_widget_set_vexpand(self->statusbar,TRUE);
    gtk_widget_set_hexpand(self->statusbar,TRUE);
    ling_overlay_add_layer(LING_OVERLAY(self->lingoverlay),self->statusbar,LAYER_STATUSBAR);
    gtk_widget_set_valign(self->statusbar,GTK_ALIGN_START);

    //桌面
    self->desktop = clm_desktop_new();
    ling_overlay_add_layer(LING_OVERLAY(self->lingoverlay),self->desktop,LAYER_DESKTOP);


    //锁屏
    self->lockscreen = cl_lock_screen_new();
    ling_overlay_add_layer(LING_OVERLAY(self->lingoverlay),self->lockscreen,LAYER_LOCKSCREEN);
    g_signal_connect(self->lockscreen,"passed",G_CALLBACK(clm_shell_lock_screen_passed),self);
    self->mode = SHELL_MODE_LOCKSCREEN;
    gtk_widget_set_visible(self->desktop,FALSE);

    clm_shell_lock_screen_passed(CL_LOCK_SCREEN(self->lockscreen),self);

    //下拉状态栏
    switch3 * s = malloc(sizeof(switch3));
    //uint sb_level,dt_level,ls_level;
    LingLayer * sb_lay,*dt_lay,*ls_lay;
    LingOverlay * sb_overlay =  cl_status_bar_get_layer_center(CL_STATUS_BAR(shell->statusbar),&sb_lay);
    //LingLayer * sb_lay=ling_overlay_get_layer(sb_overlay,sb_level);
    s->statusbar = sb_lay;
    LingOverlay * dt_overlay =  clm_desktop_get_layer_bodybox(CLM_DESKTOP(shell->desktop),&dt_lay);
    //LingLayer * dt_lay=ling_overlay_get_layer(dt_overlay,dt_level);
    s->desktop = dt_lay;
    LingOverlay * ls_overlay =  cl_lock_screen_get_layer_cover(CL_LOCK_SCREEN(shell->lockscreen),&ls_lay);
    //LingLayer * ls_lay=ling_overlay_get_layer(ls_overlay,ls_level);
    s->lockscreen = ls_lay;

    ling_operate_add_action(ling_operate_get(shell->controler,CL_LOCK_SCREEN_COVER_OP_NAME),LING_ACTION_DRAG_DOWN,
                            ling_layer_progress,NULL,
                            statusbar_center_ani,s,
                            ling_layer_release,NULL,
                            statusbar_center_s_finish,statusbar_center_e_finish,s);

    ling_operate_add_action(ling_operate_get(shell->controler,CLM_DESKTOP_BODYBOX_OP_NAME),LING_ACTION_DRAG_DOWN,
                            ling_layer_progress,NULL,
                            statusbar_center_ani,s,
                            ling_layer_release,NULL,
                            statusbar_center_s_finish,statusbar_center_e_finish,s);

    ling_operate_add_action(ling_operate_get(shell->controler,CL_STATUSBAR_CENTERBOX_OP_NAME),LING_ACTION_DRAG_UP,
                            ling_layer_progress,NULL,
                            statusbar_center_ani,s,
                            ling_layer_release,NULL,
                            statusbar_center_s_finish,statusbar_center_e_finish,s);

    //uint sbb_level;
    //ling_status_bar_get_layer_center(LING_STATUS_BAR(shell->statusbar),&sbb_level);
    ling_operate_add_action(ling_operate_get(shell->controler,CL_STATUSBAR_BAR_OP_NAME),LING_ACTION_DRAG_DOWN,
                            ling_layer_progress,NULL,
                            statusbar_center_ani,s,
                            ling_layer_release,NULL,
                            statusbar_center_s_finish,statusbar_center_e_finish,s);
}

GtkWidget * clm_shell_start(){
    shell = g_object_new(CLM_TYPE_SHELL,NULL);
    clm_shell_setting(shell);
    return GTK_WIDGET(shell);
}

// static void clicked (GtkButton* self,gpointer user_data){

//     GtkLayoutManager * manager = user_data;//gtk_box_layout_new(GTK_ORIENTATION_VERTICAL);
//     int min,nar,min_b,nar_b;
//     gtk_layout_manager_measure(manager,GTK_WIDGET(self),GTK_ORIENTATION_VERTICAL,-1,&min,&nar,&min_b,&nar_b);
//     g_print("%d %d %d %d\n",min,nar,min_b,nar_b);
// }


static void clicked (GtkButton* self,gpointer user_data){
    gdouble x,y;
    //graphene_point_t point,out;
    //gtk_widget_compute_point(GTK_WIDGET(self),GTK_WIDGET(grid),&point,&out);

    gtk_widget_translate_coordinates(GTK_WIDGET(self),GTK_WIDGET(user_data),0,0,&x,&y);
    g_print("x:%f y:%f\n",x,y);
}

static void test(ClmShell * shell){
    GtkWidget * grid = gtk_grid_new();
    gtk_box_append(GTK_BOX(shell),grid);
    for(int j=1;j<4;j++){
        for(int i=1;i<4;i++){
            GtkWidget * button = gtk_button_new();
            gtk_grid_attach(GTK_GRID(grid),button,i,j,1,1);
            g_signal_connect(button,"clicked",G_CALLBACK(clicked),grid);
        }
    }
}
