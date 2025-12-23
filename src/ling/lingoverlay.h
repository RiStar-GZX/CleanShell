#pragma once

#include <gtk/gtk.h>
#include <lingoperate.h>
#include <lingshell.h>

G_BEGIN_DECLS

typedef struct LingLayer{
    GtkWidget * widget;
    uint8_t level;

    //GtkGesture * drag;
    //GtkGesture * swipe;
    //LingOperate * op;

    //GList * switchers;

}LingLayer;

// typedef struct switcher{
//     LingLayer * main;
//     LingLayer * sub;
//     uint op_type;   //操作类型
//     //uint ani_type;  //动画类型

//     ANIMATION ani;
//     FINISH main_f;
//     //ANIMATION sub_a;
//     FINISH sub_f;
// }switcher;

typedef struct  switcher{
    LingLayer * main;
    LingLayer * sub;
}switcher;

#define LING_TYPE_OVERLAY (ling_overlay_get_type())
G_DECLARE_FINAL_TYPE(LingOverlay,ling_overlay,LING,OVERLAY,GtkBox)

GtkWidget * ling_overlay_new();

int ling_overlay_add_layer(LingOverlay * self,GtkWidget * widget,uint8_t level);

void ling_layer_add_switch(LingOperate * op_m,LingOverlay * overlay_m,uint lay_main,
                           LingOperate * op_s,LingOverlay * overlay_s,uint lay_sub,
                           uint op_type,
                           ANIMATION ani,PROGRESS progress,RELEASE release,FINISH main_f,FINISH sub_f);

LingLayer * ling_overlay_get_layer(LingOverlay * overlay,uint lay);

gdouble ling_layer_progress(GtkWidget * widget,LingActionArgs args,gpointer user_data);
gboolean ling_layer_release(GtkWidget * widget,LingActionArgs args,uint user_data);
void ling_layer_main_finish(GtkWidget * widget,gpointer user_data);
void ling_layer_sub_finish(GtkWidget * widget,gpointer user_data);
G_END_DECLS
