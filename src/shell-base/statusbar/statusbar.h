#pragma once

#include <gtk/gtk.h>
#include <ling.h>
#include <mobileshell.h>
#include <desktop.h>
#include <statusbar.h>

G_BEGIN_DECLS

#define CL_STATUSBAR_CENTERBOX_OP_NAME "statusbar_centerbox"
#define CL_STATUSBAR_VIEWPAGER_OP_NAME "statusbar_viewpager"
#define CL_STATUSBAR_BAR_OP_NAME "statusbar_bar"


#define CL_TYPE_STATUS_BAR (cl_status_bar_get_type())
G_DECLARE_FINAL_TYPE(ClStatusBar,cl_status_bar,CL,STATUS_BAR,GtkBox)


#define CL_TYPE_CENTER_HEAD (cl_center_head_get_type())
G_DECLARE_FINAL_TYPE(ClCenterHead,cl_center_head,CL,CENTER_HEAD,GtkBox)

void cl_center_head_append(ClCenterHead * self,GtkWidget * widget);

void cl_center_head_box_append(ClCenterHead * self,GtkWidget * widget);

GtkWidget * cl_center_head_new();

#define SB_PAGE_NOTICE 1
#define SB_PAGE_CONTROL 2

#define CENTER_BLUR_PX 40

GtkWidget * cl_status_bar_new();

void cl_status_bar_set_drop_degree(ClStatusBar *self,gdouble opacity);

void cl_status_bar_set_page(ClStatusBar * self,uint center_mode);

void drop_up_animation(gdouble velocity_x,gdouble velocity_y,gdouble progress,gpointer data,uint mode);
void drop_down_animation(gdouble velocity_x,gdouble velocity_y,gdouble progress,gpointer data,uint mode);
void drop_down_finish(gpointer data);
void drop_up_finish(gpointer data);

LingOverlay * cl_status_bar_get_layer_center(ClStatusBar * self,LingLayer ** layer);

LingOverlay * cl_status_bar_get_layer_bar(ClStatusBar * self,LingLayer ** layer);

GtkWidget * cl_status_bar_get_folder(ClStatusBar * self);

void cl_status_bar_set_status_bar_opacity(ClStatusBar * self,gdouble opacity);

G_END_DECLS
