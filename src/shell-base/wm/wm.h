#pragma once

#include <gtk/gtk.h>
#include <ling.h>

G_BEGIN_DECLS

typedef struct{
    gdouble offset_x;
    gdouble offset_y;
    gdouble velocity_x;
    gdouble velocity_y;
}ClWmBack;

typedef enum{
    CL_WM_WINDOW_OPEN = 0,
    CL_WM_WINDOW_CLOSE,
}CL_WM_WINDOW_ACT;

#define CL_TYPE_WM_WINDOW (cl_wm_window_get_type())
G_DECLARE_FINAL_TYPE(ClWmWindow,cl_wm_window,CL,WM_WINDOW,GtkBox)

//GtkWidget * cl_wm_window_new(const char * name,const char * icon_name);

#define CL_TYPE_WM (cl_wm_get_type())
G_DECLARE_FINAL_TYPE(ClWm,cl_wm,CL,WM,LingFixed)

typedef ClWmWindow * (*WM_OPEN_START_CB)(GtkWidget * widget,gdouble * x,gdouble * y,gpointer user_data);

typedef void (*WM_ANI_CB)(GtkWidget * widget,ClWmWindow * window,LingActionArgs args,CL_WM_WINDOW_ACT act,gpointer user_data);

//typedef void (*WM_ACT_OPEN_CB)(GtkWidget * widget,gpointer user_data);

typedef void (*WM_ACT_CB)(GtkWidget * widget,ClWmWindow * window,LingActionArgs args,gpointer user_data);

void cl_wm_window_add_app_icon(ClWmWindow * window,GtkWidget * widget);

GtkWidget * cl_wm_window_remove_app_icon(ClWmWindow * window);

GtkWidget * cl_wm_new();

ClWmWindow * cl_wm_get_window_by_name(ClWm * self,const char * name);

ClWmWindow * cl_wm_add_window(ClWm * self,const char * icon_name,const char * name);

void cl_wm_set_window_showable(ClWmWindow * window,gboolean showable);

void cl_wm_move_window(ClWmWindow * window,gdouble x,gdouble y);

void cl_wm_set_window_size(ClWmWindow * window,uint w,uint h);

void cl_wm_set_window_level(ClWmWindow * window,int level);

void cl_wm_move_window_by_progress(ClWmWindow * window,int x,int y,int level,gdouble progress);

LingOperate * cl_wm_add_operate(ClWm * wm,GtkWidget * widget,const char * window_name,const char * icon_name,
                               WM_ANI_CB ani,gpointer ani_data,
                               WM_OPEN_START_CB open_start,gpointer open_start_data,
                               WM_ACT_CB open_finish,gpointer open_finish_data,
                               WM_ACT_CB close_start,gpointer close_start_data,
                               WM_ACT_CB close_finish,gpointer close_finish_data);

void cl_wm_window_close(ClWmWindow * window,gdouble offset_x,gdouble offset_y,
                        gdouble velocity_x,gdouble velocity_y);

void cl_wm_close_current_window(ClWm * wm,gdouble offset_x,gdouble offset_y,
                                gdouble velocity_x,gdouble velocity_y);

LingOverlay * cl_wm_window_get_layer_icon(ClWmWindow * self,LingLayer ** layer);

LingOverlay * cl_wm_window_get_layer_window(ClWmWindow * self,LingLayer ** layer);

G_END_DECLS
