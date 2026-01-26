#pragma once

#include <gtk/gtk.h>
#include <lingdatasaver.h>
#include <lingdots.h>
#include <lingfixed.h>
#include <linggrid.h>
#include <lingoverlay.h>
#include <lingoperate.h>
#include <lingsysinfo.h>
#include <lingsysinfoupdater.h>
#include <lingviewpager.h>
#include <lingwallpaper.h>
#include <lingnotifyitem.h>
#include <lingnotifyviewer.h>
#include <lingkeyborad.h>
#include <lingtimeviewer.h>
#include <lingfolder.h>
#include <lingdragface.h>

typedef struct{
    int column;
    int row;
    int grid_w;     //占格数
    int grid_h;
    uint size_w;    //大小(像素)
    uint size_h;
    gdouble start_x;
    gdouble start_y;
    LingGrid * grid;
    GtkWidget * face;   //层:例如clwm,lingfolder
    //GtkWidget * content; //显示的控件
    GtkWidget * coordinates;
}LingGridOriPos;

void ling_widget_scale(GtkWidget * self,gdouble scale);

void ling_widget_border_radis(GtkWidget * widget,gdouble px);
