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


#define LING_TYPE_SHELL (ling_shell_get_type())
G_DECLARE_FINAL_TYPE(LingShell,ling_shell,LING,SHELL,GtkBox)

enum{
    SHELL_MODE_LOCKSCREEN,
    SHELL_MODE_DESKTOP,
};

struct _LingShell{
    GtkBox parent;
    uint mode;  //SHELL_MODE

    GtkWidget * statusbar;
    GtkWidget * desktop;
    GtkWidget * lockscreen;

    GtkWidget * bodybox;
    GtkWidget * drawer;

    GtkWidget * lingoverlay;

    LingOpControler * controler;

    GList * app_info;

    LingSysInfoUpdater * updater;
};

extern LingShell * shell;

typedef struct app_info{
    GString * desktop_path; //".desktop"的位置
    gboolean no_display;
    char * name;
    char * icon;
    char * exec;
}app_info;



GtkWidget * ling_shell_start();

app_info * ling_shell_get_app_info(const char * filepath);

G_END_DECLS
