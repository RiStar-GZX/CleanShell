#ifndef LINGOPERATE_H
#define LINGOPERATE_H

#include <gtk/gtk.h>

typedef struct LingOpControler{
    uint frames;
    GList * operates;
}LingOpControler;

enum ling_operate_state{
    LING_OPERATE_STATE_WAITTING,
    LING_OPERATE_STATE_OPERATING,
    LING_OPERATE_STATE_ANIMATION,
};


#define    LING_OPERATE_ANIMATION_REMOVE       FALSE
#define    LING_OPERATE_ANIMATION_CONTINUE     TRUE

#define    LING_OPERATE_BREAK_TO_ANIMATION     FALSE
#define    LING_OPERATE_BREAK_TO_FINISH        TRUE


typedef gboolean  (*ANIMATION)(gdouble velocity_x,gdouble velocity_y,gpointer user_data);

typedef gboolean  (*ISBREAKED)(gpointer user_data);

typedef void  (*FINISH)(gpointer user_data);

typedef struct LingOperate{
    LingOpControler *controler;

    GString * operate_name;
    uint state;
    GList * breaker;//(operate_name)

    gdouble velocity_x;
    gdouble velocity_y;

    ANIMATION animation;
    gpointer animation_data;
    uint animation_timer_id;
    ISBREAKED isbreaked;
    gpointer isbreaked_data;
    FINISH finish;
    gpointer finish_data;

}LingOperate;

LingOpControler * ling_operate_controler_new(uint frame);

LingOperate * ling_operate_get(LingOpControler * controler,const char * op_name);

void ling_operate_set_animation_cb(LingOperate * op,ANIMATION cb,gpointer data);

void ling_operate_set_isbreaked_cb(LingOperate * op,ISBREAKED cb,gpointer data);

void ling_operate_set_finish_cb(LingOperate * op,FINISH cb,gpointer data);

LingOperate * ling_operate_add(LingOpControler * controler,const char * op_name,
                              ANIMATION animation,gpointer animation_data,
                              ISBREAKED isbreaked,gpointer isbreaked_data,
                              FINISH finish,gpointer finish_data);

void ling_operate_remove(LingOperate * op);

void ling_operate_run_finish(LingOperate * op);

void ling_operate_run_animation(LingOperate * op);

void ling_operate_run_isbreaked(LingOperate * op);

gboolean ling_operate_start_operating(LingOperate * op);

void ling_operate_set_change(LingOperate * op,const char * opname);

void ling_operate_swipe_cb(GtkGestureSwipe* self,
    gdouble velocity_x,gdouble velocity_y,gpointer user_data);

#endif // LINGOPERATE_H
