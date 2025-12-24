#pragma once

#include <gtk/gtk.h>
#include <lingshell.h>
#include <lingoperate.h>
#include <lingnotifyitem.h>
#include <lingdesktop.h>
#include <lingstatusbar.h>
#include <linggrid.h>
#include <lingcontrolbutton.h>
#include <lingcontrolmedia.h>
#include <lingwlanlist.h>
#include <lingblelist.h>
#include <lingsysinfo.h>

G_BEGIN_DECLS

#define LING_TYPE_CONTROL_CENTER (ling_control_center_get_type())
G_DECLARE_FINAL_TYPE(LingControlCenter,ling_control_center,LING,CONTROL_CENTER,GtkBox)

GtkWidget * ling_control_center_new(GtkWidget * overlay);   //还是用gtkoverlay,得改成lingoverlay

void ling_control_center_set_blur(LingControlCenter * self,uint px);

G_END_DECLS
