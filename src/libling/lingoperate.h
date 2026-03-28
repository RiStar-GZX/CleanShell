#ifndef LINGOPERATE_H
#define LINGOPERATE_H

#include <gtk/gtk.h>

typedef struct LingOpControler {
    uint frames;
    GList * operates;
    uint timeout;

    GList * actions_list; //LingAction
}LingOpControler;

typedef enum {
    LING_OPERATE_STATE_WAITTING,
    LING_OPERATE_STATE_OPERATING,
    LING_OPERATE_STATE_ANIMATION,
}LING_OPERATE_STATE;


typedef enum {
    LING_OPERATE_EMIT_AT_START=0,
    LING_OPERATE_EMIT_AT_RELEASE,
    LING_OPERATE_EMIT_AT_FINISH,
    LING_OPERATE_EMIT_AT_FINISH_S,
    LING_OPERATE_EMIT_AT_FINISH_E,
    LING_OPERATE_EMIT_NUM,
}LING_OPERATE_EMIT;

#define    LING_OPERATE_ANIMATION_REMOVE       FALSE
#define    LING_OPERATE_ANIMATION_CONTINUE     TRUE

#define    LING_OPERATE_BREAK_TO_ANIMATION     FALSE
#define    LING_OPERATE_BREAK_TO_FINISH        TRUE

#define ANI_END_FORWARD  100
#define ANI_END_BACK     0
#define ANI_END_NOT_CHANGE     1000000

#define    LING_ANI_TIME

typedef enum {
    LING_ACTION_CLICK=0,
    LING_ACTION_DRAG_UP,
    LING_ACTION_DRAG_DOWN,
    LING_ACTION_DRAG_LEFT,
    LING_ACTION_DRAG_RIGHT,
    LING_ACTION_DRAG_SOURCE,
    LING_ACTION_DRAG_HORIZONTAL,
    LING_ACTION_DRAG_VERTICAL,
    LING_ACTION_DRAG_ALL,
    LING_ACTION_LONG_PRESS,
    LING_ACTION_LONG_PRESS_UP,
    LING_ACTION_LONG_PRESS_DOWN,
    LING_ACTION_LONG_PRESS_LEFT,
    LING_ACTION_LONG_PRESS_RIGHT,
    LING_ACTION_LONG_PRESS_DRAG_SOURCE,
    LING_ACTION_EMIT,
    LING_ACTION_INSTANT,
    LING_ACTION_ANIMATE,
    LING_ACTION_NUM,
}LING_ACTION;

typedef enum{
    LING_DRAG_SOURCE_INSTANT = LING_ACTION_DRAG_SOURCE,
    LING_DRAG_SOURCE_LONG_PRESS = LING_ACTION_LONG_PRESS_DRAG_SOURCE,
}LING_DRAG_SOURCE_TYPE;

typedef struct LingOperate LingOperate;

typedef struct {
    LingOperate * op;
    gdouble start_x;
    gdouble start_y;
}LingBeginArgs;

typedef struct {
    LingOperate * op;
    gdouble start_x;
    gdouble start_y;
    gdouble offset_x;
    gdouble offset_y;
    gdouble velocity_x;
    gdouble velocity_y;
}LingEndArgs;

typedef struct LingActionArgs{
    LingOperate * op;
    gdouble start_x;
    gdouble start_y;
    gdouble offset_x;
    gdouble offset_y;
    gdouble velocity_x;
    gdouble velocity_y;
    gdouble progress;
    gdouble progress_end;
    gpointer emit_data;
    uint action;
}LingActionArgs;

typedef enum{
    FINISH_DIR_START = 0,
    FINISH_DIR_END = 1,
}FINISH_DIR;

typedef enum{
    EMIT_FINISH_DIR_START = FINISH_DIR_START,
    EMIT_FINISH_DIR_END = FINISH_DIR_END,
    EMIT_FINISH_DIR_REVERSE,
}EMIT_FINISH_DIR;

#define PLEN gdouble

typedef void (*BEGIN)(GtkWidget * widget,LingBeginArgs args,gpointer user_data);  //瞬发

typedef void (*END)(GtkWidget * widget,LingEndArgs args,gpointer user_data);


/*drag*/
typedef gdouble (*PROGRESS)(GtkWidget * widget,LingActionArgs args,gpointer user_data);  //返回

typedef PLEN (*RELEASE)(GtkWidget * widget,LingActionArgs args,gpointer user_data);  //pos

typedef void (*ANIMATION)(GtkWidget * widget,LingActionArgs args,gpointer user_data);

typedef gboolean  (*ISBREAKED)(gpointer user_data); //废弃

typedef void  (*FINISH)(GtkWidget * widget,LingActionArgs args,gpointer user_data);



typedef GdkContentProvider * (*DRAGSOURCE_PREPARE)(
    GtkDragSource* self,gdouble x,gdouble y,gpointer user_data);

typedef void (*DRAGSOURCE_BEGIN)(GtkDragSource* self,GdkDrag* drag,gpointer user_data);

typedef gboolean (*DRAGSOURCE_CANCEL)(
    GtkDragSource* self,GdkDrag* drag,GdkDragCancelReason* reason,gpointer user_data);

typedef void (*DRAGSOURCE_END)(GtkDragSource* self,
                                    GdkDrag* drag,gboolean delete_data,gpointer user_data);

typedef struct LingAction{
    LingOperate * op;
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
    gdouble ani_progress_start; //记录第一次触发progress的位置
    gboolean ani_start;         //是否已经记录ani_progress_start

    gdouble ani_progress_lenth;
    gdouble ani_progress_end;
    gdouble ani_time;     //进度从0到ani_progress_end所需要的时间
    //ANI_DIR ani_dir;     //方向(正反)  //默认正，打断反

    FINISH_DIR dir; //将往何去,终停于何处
    //gboolean dir;
    //ANI_DIR nature_dir; //

    //曲线

    //完成
    //FINISH_DIR finish_pos;
    FINISH finish_s;
    FINISH finish_e;
    gpointer finish_data;

    //拖拽DND
    DRAGSOURCE_PREPARE prepare;
    gpointer prepare_data;
    DRAGSOURCE_BEGIN ds_begin;
    gpointer ds_begin_data;
    DRAGSOURCE_CANCEL cancel;
    gpointer cancel_data;
    DRAGSOURCE_END end;
    gpointer end_data;

    //连带触发
    LingOperate * emit[LING_OPERATE_EMIT_NUM];
    gpointer emit_data[LING_OPERATE_EMIT_NUM];
    gboolean emit_dir[LING_OPERATE_EMIT_NUM];
}LingAction;

typedef struct LingOperate{
    LingOpControler *controler;

    gboolean able;
    GtkDragSource * drag_source;
    GtkDropTarget * drop_target;
    GtkGesture * drag;
    GtkGesture * swipe;
    //GtkGesture * instant;   //点击的瞬间触发的手势
    GtkWidget * widget;

    GString * operate_name;
    uint state;
    GList * breaker;//(operate_name)

    gboolean force_run;
    //开始
    gdouble start_x;
    gdouble start_y;
    gdouble offset_x;
    gdouble offset_y;
    gdouble velocity_x;
    gdouble velocity_y;

    LingAction actions[LING_ACTION_NUM];    //LingAction
    uint action_now;

    gdouble time;         //当前时间0-100
    //uint animation_timer_id;    //每个周期执行动画(以后用一个timer，串起所有的动画)

    //瞬发
    BEGIN begin;
    gpointer begin_data;

    //结束
    END end;
    gpointer end_data;

    //打断(废弃)
    ISBREAKED isbreaked;
    gpointer isbreaked_data;

    gpointer emit_data;

    int longpress_id;
    gboolean longpress_status;
}LingOperate;

LingOpControler * ling_operate_controler_new(uint frame);

LingOperate * ling_operate_get(LingOpControler * controler,const char * op_name);

void ling_operate_set_animation_cb(LingOperate * op,uint action_type,ANIMATION cb,gpointer data);

void ling_operate_set_isbreaked_cb(LingOperate * op,ISBREAKED cb,gpointer data);

void ling_operate_set_finish_cb(LingOperate * op,uint action_type,FINISH cb,gpointer data);

LingOperate * ling_operate_add(LingOpControler * controler,const char * op_name,gpointer widget);

void ling_operate_set_able(LingOperate * op,gboolean able);

void ling_operate_remove(LingOperate * op);

void ling_operate_run_finish(LingOperate * op,FINISH_DIR dir);

void ling_operate_run_animation(LingOperate * op);

void ling_operate_run_isbreaked(LingOperate * op);

gboolean ling_operate_start_operating(LingOperate * op);

int ling_operate_get_action_now(LingOperate * op);

void ling_operate_set_ani_progress(LingOperate * op,int action,gdouble progress);

gdouble ling_operate_get_ani_progress(LingOperate * op,int action);

void ling_operate_ani_end_now(LingOperate * op,int action);

void ling_operate_set_ani_progress_end(LingOperate * op,int action,gdouble progress_end);

gdouble ling_operate_get_ani_progress_end(LingOperate * op,int action);

void ling_operate_set_ani_progress_lenth(LingOperate * op,int action,double lenth);

gdouble ling_operate_get_ani_progress_lenth(LingOperate * op,int action);

gdouble ling_operate_get_dir(LingOperate * op,int action);

void ling_operate_set_dir(LingOperate * op,int action,FINISH_DIR dir);

void ling_operate_set_ani_time(LingOperate * op,LING_ACTION action,gdouble time);

gdouble ling_opertae_get_ani_time(LingOperate * op,LING_ACTION action);

//void ling_operate_set_ani_dir(LingOperate * op,int action,ANI_DIR ani_dir);

//ANI_DIR ling_operate_get_ani_dir(LingOperate * op,int action);

//GtkGesture * ling_operate_click_ignore(LingOperate * op,GtkWidget * widget);

GtkGesture * ling_operate_drag_ignore(LingOperate * op,GtkWidget * widget);

void ling_operate_add_action(LingOperate * op,uint type,
                             PROGRESS progress,gpointer progress_data,
                             ANIMATION animate,gpointer animate_data,
                             RELEASE release,gpointer release_data,
                             FINISH finish_s,FINISH finish_e,gpointer finish_data);

void ling_operate_set_drag_propagation_phase(LingOperate * op,GtkPropagationPhase phase);

void ling_operate_add_dragsource(LingOperate * op,LING_DRAG_SOURCE_TYPE type,
                                 DRAGSOURCE_PREPARE prepare,gpointer prepare_data,
                                 DRAGSOURCE_BEGIN begin,gpointer begin_data,
                                 DRAGSOURCE_CANCEL cancel,gpointer cancel_data,
                                 DRAGSOURCE_END end,gpointer end_data);

void ling_operate_set_force_run(LingOperate * op,gboolean force_run);//暂时的方案，以后改成优先级

void ling_operate_emit_start(LingOperate * op,LING_ACTION action,gpointer emit_data,gboolean ani);

void ling_operate_emit_end(LingOperate * op,LING_ACTION action,gpointer emit_data,gboolean ani);

void ling_operate_emit(LingOperate * op,LING_ACTION action,gpointer emit_data,gboolean ani,EMIT_FINISH_DIR dir);

void ling_operate_emit_connect(LingOperate * source,LING_ACTION action,LING_OPERATE_EMIT emit,LingOperate * target,EMIT_FINISH_DIR dir,gpointer emit_data);

LingOperate * ling_operate_add_animate(LingOpControler * controler,const char * ani_name,
                                      RELEASE release,gpointer release_data,ANIMATION ani,gpointer ani_data,
                                      FINISH finish_s,FINISH finish_e,gpointer finish_data);

void ling_operate_add_begin(LingOperate * op,BEGIN begin,gpointer begin_data);

void ling_operate_add_end(LingOperate * op,END end,gpointer end_data);

gboolean ling_operate_controler_timeout(gpointer user_data);//暂时先这么写

#endif // LINGOPERATE_H
