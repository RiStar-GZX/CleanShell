#pragma once

#include <gtk/gtk.h>
#include <lingshell.h>
#include <lingoperate.h>
#include <lingdesktop.h>
#include <lingstatusbar.h>

G_BEGIN_DECLS

#define LING_STATUSBAR_CENTERBOX_OP_NAME "statusbar_centerbox"
#define LING_STATUSBAR_BAR_OP_NAME "statusbar_bar"

#define LING_TYPE_STATUS_BAR (ling_status_bar_get_type())
G_DECLARE_FINAL_TYPE(LingStatusBar,ling_status_bar,LING,STATUS_BAR,GtkBox)


#define LING_TYPE_CENTER_HEAD (ling_center_head_get_type())
G_DECLARE_FINAL_TYPE(LingCenterHead,ling_center_head,LING,CENTER_HEAD,GtkBox)

void ling_center_head_append(LingCenterHead * self,GtkWidget * widget);

void ling_center_head_box_append(LingCenterHead * self,GtkWidget * widget);

GtkWidget * ling_center_head_new();

#define SB_PAGE_NOTICE 1
#define SB_PAGE_CONTROL 2

#define CENTER_BLUR_PX 40

GtkWidget * ling_status_bar_new();

void ling_status_bar_set_drop_degree(LingStatusBar *self,gdouble opacity);

void ling_status_bar_set_page(LingStatusBar * self,uint center_mode);

void drop_up_animation(gdouble velocity_x,gdouble velocity_y,gdouble progress,gpointer data,uint mode);
void drop_down_animation(gdouble velocity_x,gdouble velocity_y,gdouble progress,gpointer data,uint mode);
void drop_down_finish(gpointer data);
void drop_up_finish(gpointer data);

LingOverlay * ling_status_bar_get_layer_center(LingStatusBar * self,uint * level);

LingOverlay * ling_status_bar_get_layer_bar(LingStatusBar * self,uint * level);

void ling_status_bar_set_status_bar_opacity(LingStatusBar * self,gdouble opacity);

G_END_DECLS
