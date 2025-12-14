#include "lingshell.h"
#include <wordexp.h>
#include <lingstatusbar.h>
#include <linglockscreen.h>
#include <lingdesktop.h>
#include <lingkeyborad.h>
#include <lingsysinfo.h>
#include <lingtimeviewer.h>

LingShell * shell;

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
G_DEFINE_FINAL_TYPE(LingShell,ling_shell,GTK_TYPE_BOX);

LingShell CleanShell;

app_info * ling_shell_get_app_info(const char * filepath){
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

int ling_shell_load_apps(LingShell * self,const char * apps_dir){
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
            app_info * infodd = ling_shell_get_app_info(fullpath->str);
            if(infodd!=NULL)self->app_info = g_list_append(self->app_info,infodd);
        }
    }
    g_object_unref(enumerator);
    g_object_unref(file);
    return 1;
}

void ling_shell_class_init(LingShellClass * klass){

}

void ling_shell_lock_screen_passed(LingLockScreen * lockscreen,gpointer user_data){
    LingShell * self = (LingShell *)user_data;
    gtk_widget_set_visible(self->lockscreen,FALSE);
}

void ling_shell_init(LingShell * self){

}

void ling_shell_setting(LingShell * self){
    //加载所有应用
    ling_shell_load_apps(self,"/usr/share/applications/");
    ling_shell_load_apps(self,"~/.local/share/applications/");

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



    self->desktop = ling_desktop_new();
    ling_overlay_add_layer(LING_OVERLAY(self->lingoverlay),self->desktop,LAYER_DESKTOP);

    //状态栏
    self->statusbar = ling_status_bar_new();
    gtk_widget_set_vexpand(self->statusbar,TRUE);
    gtk_widget_set_hexpand(self->statusbar,TRUE);
    ling_overlay_add_layer(LING_OVERLAY(self->lingoverlay),self->statusbar,LAYER_STATUSBAR);
    gtk_widget_set_valign(self->statusbar,GTK_ALIGN_START);

    //锁屏
    self->lockscreen = ling_lock_screen_new();
    ling_overlay_add_layer(LING_OVERLAY(self->lingoverlay),self->lockscreen,LAYER_LOCKSCREEN);
    g_signal_connect(self->lockscreen,"passed",G_CALLBACK(ling_shell_lock_screen_passed),self);

    // GtkWidget * fixed = gtk_fixed_new();
    // gtk_widget_set_vexpand(fixed,TRUE);
    // gtk_widget_set_hexpand(fixed,TRUE);
    // GtkWidget * button = gtk_label_new("das");
    // gtk_widget_set_valign(button,GTK_ALIGN_FILL);
    // gtk_widget_set_halign(button,GTK_ALIGN_FILL);
    // gtk_widget_add_css_class(button,"keyboard");
    // gtk_fixed_put(GTK_FIXED(fixed),button,20,20);

    // gtk_box_append(GTK_BOX(self),fixed);
}

GtkWidget * ling_shell_start(){
    shell = g_object_new(LING_TYPE_SHELL,NULL);
    ling_shell_setting(shell);
    return GTK_WIDGET(shell);
}

