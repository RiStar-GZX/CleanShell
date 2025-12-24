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


#define    LING_OPERATE_ANIMATION_DIR_FORWARD  TRUE
#define    LING_OPERATE_ANIMATION_DIR_BACK     FALSE

#define    LING_ACTION_FINISH_S                 FALSE
#define    LING_ACTION_FINISH_E                 TRUE


enum{
    LING_ACTION_DRAG_NONE=0,
    LING_ACTION_DRAG_UP,
    LING_ACTION_DRAG_DOWN,
    LING_ACTION_DRAG_LEFT,
    LING_ACTION_DRAG_RIGHT,
    LING_ACTION_LONG_PRESS,
    LING_ACTION_LONG_PRESS_UP,
    LING_ACTION_LONG_PRESS_DOWN,
    LING_ACTION_LONG_PRESS_LEFT,
    LING_ACTION_LONG_PRESS_RIGHT,
    LING_ACTION_NUM,
};

typedef struct LingActionArgs{
    gdouble start_x;
    gdouble start_y;
    gdouble offset_x;
    gdouble offset_y;
    gdouble velocity_x;
    gdouble velocity_y;
    gdouble progress;
    uint action;
}LingActionArgs;

typedef gdouble (*PROGRESS)(GtkWidget * widget,LingActionArgs args,gpointer user_data);  //返回

typedef gboolean (*RELEASE)(GtkWidget * widget,LingActionArgs args,gpointer user_data);  //pos

typedef void (*ANIMATION)(GtkWidget * widget,LingActionArgs args,gpointer user_data);

typedef gboolean  (*ISBREAKED)(gpointer user_data);

typedef void  (*FINISH)(GtkWidget * widget,gpointer user_data);

typedef struct LingAction{
    gboolean able;
    //进度
    PROGRESS progress;
    gpointer progress_data;

    //松手
    RELEASE release;
    gpointer release_data;
    //gpointer release_data;

    //动画
    ANIMATION animation;
    gpointer animate_data;

    gdouble ani_progress; //进度
    gdouble ani_time;     //进度从0到100所需要的时间
    gboolean ani_dir;     //方向(正反)  //默认正，打断反

    //曲线

    //完成
    FINISH finish_s;
    FINISH finish_e;
    gpointer finish_data;

}LingAction;

typedef struct LingOperate{
    LingOpControler *controler;

    GtkGesture * drag;
    GtkGesture * swipe;
    GtkWidget * widget;

    GString * operate_name;
    uint state;
    GList * breaker;//(operate_name)

    //开始
    gdouble start_x;
    gdouble start_y;
    gdouble offset_x;
    gdouble offset_y;
    gdouble velocity_x;
    gdouble velocity_y;

    LingAction actions[LING_ACTION_NUM];    //LingAction
    uint action_now;

    uint animation_timer_id;    //每个周期执行动画(以后用一个timer，串起所有的动画)

    //打断(废弃)
    ISBREAKED isbreaked;
    gpointer isbreaked_data;
}LingOperate;

LingOpControler * ling_operate_controler_new(uint frame);

LingOperate * ling_operate_get(LingOpControler * controler,const char * op_name);

void ling_operate_set_animation_cb(LingOperate * op,uint action_type,ANIMATION cb,gpointer data);

void ling_operate_set_isbreaked_cb(LingOperate * op,ISBREAKED cb,gpointer data);

void ling_operate_set_finish_cb(LingOperate * op,uint action_type,FINISH cb,gpointer data);

LingOperate * ling_operate_add(LingOpControler * controler,const char * op_name,gpointer widget/*,
                              ANIMATION animation,gpointer animation_data,
                              //UPDATE update,gpointer update_data,
                              ISBREAKED isbreaked,gpointer isbreaked_data,
                              FINISH finish,gpointer finish_data*/);


void ling_operate_remove(LingOperate * op);

void ling_operate_run_finish(LingOperate * op,gboolean s_e);

void ling_operate_run_animation(LingOperate * op);

void ling_operate_run_isbreaked(LingOperate * op);

gboolean ling_operate_start_operating(LingOperate * op);

void ling_operate_set_change(LingOperate * op,const char * opname);

void ling_operate_swipe_cb(GtkGestureSwipe* self,
    gdouble velocity_x,gdouble velocity_y,gpointer user_data);


void ling_operate_add_action(LingOperate * op,uint type,
                             PROGRESS progress,gpointer progress_data,
                             ANIMATION animate,gpointer animate_data,
                             RELEASE release,gpointer release_data,
                             FINISH finish_s,FINISH finish_e,gpointer finish_data);

#endif // LINGOPERATE_H
