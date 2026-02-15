#pragma once

#include <gtk/gtk.h>
#include <lingoverlay.h>
#include <lingoperate.h>
#include <lingsysinfoupdater.h>

G_BEGIN_DECLS

enum{
    INFO_UPDATER_TIME=0,
    INFO_UPDATER_NOTIFY,
    INFO_UPDATER_WEATHER
};


#define CLM_TYPE_SHELL (clm_shell_get_type())
G_DECLARE_FINAL_TYPE(ClmShell,clm_shell,CLM,SHELL,GtkBox)

enum{
    SHELL_MODE_LOCKSCREEN,
    SHELL_MODE_DESKTOP,
};

struct _ClmShell{
    GtkBox parent;
    uint mode;  //SHELL_MODE

    GtkWidget * statusbar;
    GtkWidget * desktop;
    GtkWidget * lockscreen;

    GtkWidget * wm;

    GtkWidget * lingoverlay;

    LingOpControler * controler;

    GList * app_info;

    LingSysInfoUpdater * updater;

    //fps
    GtkWidget * fps_label;
    GdkFrameClock * clock;
};

extern ClmShell * shell;

typedef struct app_info{
    GString * desktop_path; //".desktop"的位置
    gboolean no_display;
    char * name;
    char * icon;
    char * exec;
}app_info;



GtkWidget * clm_shell_start();

app_info * clm_shell_get_app_info(const char * filepath);

G_END_DECLS
