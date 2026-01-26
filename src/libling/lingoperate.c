#include "lingoperate.h"

static void operate_drag_begin(GtkGestureDrag* self,
                               gdouble start_x,gdouble start_y,gpointer user_data);

static void operate_drag_update(GtkGestureDrag* self,
                                gdouble offset_x,gdouble offset_y,gpointer user_data);

static void operate_drag_end(GtkGestureDrag* self,
                             gdouble offset_x,gdouble offset_y,gpointer user_data);

void ling_operate_swipe_cb(GtkGestureSwipe* self,
                           gdouble velocity_x,gdouble velocity_y,gpointer user_data);

LingOpControler * ling_operate_controler_new(uint frame){
    LingOpControler * controler = malloc(sizeof(LingOpControler));
    controler->frames = frame;
    controler->operates = NULL;
    return controler;
}

char * ling_operate_new_name(LingOpControler * controler,const char * name){
    char * new_name=malloc(sizeof(char)*100);
    strcpy(new_name,name);
    for(int i=1;;i++){
        gboolean have_same=0;
        for(GList * l=controler->operates;l!=NULL;l=l->next){
            LingOperate * op_now = (LingOperate*)l->data;
            if(strcmp(op_now->operate_name->str,new_name)==0){
                sprintf(new_name,"%s_%d",name,i);
                have_same=1;
            }
        }
        if(!have_same)return new_name;
    }
    return NULL;
}

LingOperate * ling_operate_add(LingOpControler * controler,const char * op_name,gpointer widget){
    if(op_name==NULL||controler==NULL)return NULL;

    //创建op
    LingOperate * op = malloc(sizeof(LingOperate));
    memset(op,0,sizeof(LingOperate));
    op->able = TRUE;
    op->controler = controler;
    op->state = LING_OPERATE_STATE_WAITTING;

    //创建新的名字防止重名
    char * new_name = ling_operate_new_name(controler,op_name);
    op->operate_name = g_string_new(new_name);
    free(new_name);

    //设置回调函数
    op->breaker = NULL;     //没有人能打断

    //添加进表里，返回
    controler->operates = g_list_append(controler->operates,op);

    //添加控件的event controler
    op->widget = widget;

    // op->drag =gtk_gesture_drag_new();
    // gtk_widget_add_controller(GTK_WIDGET(op->widget), GTK_EVENT_CONTROLLER(op->drag));
    // g_signal_connect(op->drag, "drag-begin", G_CALLBACK(operate_drag_begin), op);
    // g_signal_connect(op->drag, "drag-update", G_CALLBACK(operate_drag_update), op);
    // g_signal_connect(op->drag, "drag-end", G_CALLBACK(operate_drag_end), op);
    // gtk_event_controller_set_propagation_phase(GTK_EVENT_CONTROLLER(op->drag), GTK_PHASE_CAPTURE);

    // op->swipe = gtk_gesture_swipe_new();
    // gtk_widget_add_controller(GTK_WIDGET(op->widget), GTK_EVENT_CONTROLLER(op->swipe));
    // g_signal_connect(op->swipe,"swipe",G_CALLBACK(ling_operate_swipe_cb),op);
    return op;
}

void ling_operate_remove(LingOperate * op){
    LingOpControler * controler = op->controler;
    controler->operates = g_list_remove(controler->operates,op);
}

LingOperate * ling_operate_get(LingOpControler * controler,const char * op_name){
    if(controler==NULL||op_name==NULL)return NULL;
    for(GList * l=controler->operates;l!=NULL;l=l->next){
        LingOperate * op_now = (LingOperate*)l->data;
        if(strcmp(op_now->operate_name->str,op_name)==0)return op_now;
    }
    return NULL;
}


void ling_operate_set_animation_cb(LingOperate * op,uint action_type,ANIMATION cb,gpointer data){
    //设置动画，定时器每帧运行一次CALLBACK

    LingAction * act = &op->actions[op->action_now];
    act->animation = cb;
    act->animate_data = data;
}

void ling_operate_set_isbreaked_cb(LingOperate * op,ISBREAKED cb,gpointer data){
    //设置打断
    op->isbreaked = cb;
    op->isbreaked_data = data;
}

void ling_operate_set_finish_cb(LingOperate * op,uint action_type,FINISH cb,gpointer data){
    //动画结束后或者到位了，操作完成
    if(cb==NULL)return;
    LingAction * act = &op->actions[action_type];
    act->finish_e = cb;
    act->finish_data = data;
}

void ling_operate_add_breaker(LingOperate * op,const char * op_name){
    //打断白名单
    //if(op==)
}

void ling_operate_clear_breaker(LingOperate * op){
    g_list_free(op->breaker);
    //打断白名单
}

void ling_operate_change_breaker(LingOperate * op,const char * op_name){
    //打断白名单
}

//-------------------------------------------------------------------------------------------
static LingActionArgs operate_action_args(LingOperate * op,uint action_type){
    LingActionArgs args;
    args.op = op;
    args.offset_x= op->offset_x;
    args.offset_y= op->offset_y;
    args.start_x = op->start_x;
    args.start_y = op->start_y;
    args.velocity_x = op->velocity_x;
    args.velocity_y = op->velocity_y;
    args.progress = op->actions[action_type].ani_progress;
    args.progress_end = op->actions[action_type].ani_progress_end;
    args.action = action_type;
    args.emit_data = op->emit_data;
    return args;
}

void ling_operate_run_finish(LingOperate * op,gboolean s_e){
    if(op==NULL||op->state==LING_OPERATE_STATE_WAITTING)return;
    op->state = LING_OPERATE_STATE_WAITTING;
    LingAction * act = &op->actions[op->action_now];
    if(s_e==LING_ACTION_FINISH_E&&act->finish_e!=NULL)act->finish_e(op->widget,operate_action_args(op,op->action_now),act->finish_data);
    if(s_e==LING_ACTION_FINISH_S&&act->finish_s!=NULL)act->finish_s(op->widget,operate_action_args(op,op->action_now),act->finish_data);
    op->emit_data = NULL;
}

/**
 * 三次贝塞尔曲线公式实现
 * B(t) = (1-t)*P0 + 3*(1-t)*t*P1 + 3*(1-t)*t*P2 + t*P3
 * 其中 t = progress / 100.0
 */

// 计算三次贝塞尔曲线值
double bezier_curve(double p0, double p1, double p2, double p3, double t) {
    if (t < 0.0) t = 0.0;
    if (t > 1.0) t = 1.0;

    double one_minus_t = 1.0 - t;
    return one_minus_t * one_minus_t * one_minus_t * p0 +
           3.0 * one_minus_t * one_minus_t * t * p1 +
           3.0 * one_minus_t * t * t * p2 +
           t * t * t * p3;
}

// 计算贝塞尔曲线的一阶导数(速度)
double bezier_derivative(double p0, double p1, double p2, double p3, double t) {
    if (t < 0.0) t = 0.0;
    if (t > 1.0) t = 1.0;

    double one_minus_t = 1.0 - t;
    return 3.0 * one_minus_t * one_minus_t * (p1 - p0) +
           6.0 * one_minus_t * t * (p2 - p1) +
           3.0 * t * t * (p3 - p2);
}

// 使用牛顿迭代法求解贝塞尔曲线反向映射
double bezier_inverse(double p0, double p1, double p2, double p3, double target_y) {
    // 归一化目标值到0-1区间
    double normalized_target = target_y / 100.0;

    // 边界检查
    if (normalized_target <= p0) return 0.0;
    if (normalized_target >= p3) return 1.0;

    // 初始猜测值
    double t = normalized_target; // 使用线性插值作为初始值

    // 牛顿迭代法求解
    for (int i = 0; i < 10; i++) { // 最多迭代10次
        double curve_value = bezier_curve(p0, p1, p2, p3, t);
        double derivative = bezier_derivative(p0, p1, p2, p3, t);

        // 防止除零错误
        if (fabs(derivative) < 1e-10) break;

        // 牛顿迭代公式
        double delta = (curve_value - normalized_target) / derivative;
        t -= delta;

        // 边界检查
        if (t < 0.0) t = 0.0;
        if (t > 1.0) t = 1.0;

        // 收敛检查
        if (fabs(delta) < 1e-6) break;
    }

    return t;
}

gboolean ling_operate_animation_timeout(gpointer user_data){
    LingOperate * op = (LingOperate*)user_data;
    LingAction * act = &op->actions[op->action_now];

    //gdouble offset = bezier_curve(0,25,75,100,act->time/1000.0000f)/act->ani_time;
    //act->time+=1000/op->controler->frames;
    //g_print("time:%f %f\n",act->time,offset);
    //IPT FIX
    if(act->ani_dir==LING_OPERATE_ANIMATION_DIR_FORWARD){
        //act->ani_time*op->controler->frames;
        //act->ani_progress = offset;
        act->ani_progress+=1+0.05*(act->ani_progress_end-act->ani_progress);//00/op->controler->frames;
        if(act->ani_progress>=act->ani_progress_end){
            act->ani_progress = act->ani_progress_end;
            act->animation(op->widget,operate_action_args(op,op->action_now),act->animate_data);
            ling_operate_run_finish(op,LING_ACTION_FINISH_E);
            op->animation_timer_id=0;
            return G_SOURCE_REMOVE;
        }
    }
    else{
        //act->ani_progress = -offset;
        act->ani_progress-=1+0.05*(act->ani_progress);//00/op->controler->frames;
        if(act->ani_progress<=0){
            act->ani_progress = 0;
            act->animation(op->widget,operate_action_args(op,op->action_now),act->animate_data);
            ling_operate_run_finish(op,LING_ACTION_FINISH_S);
            op->animation_timer_id=0;
            return G_SOURCE_REMOVE;
        }
    }
    act->animation(op->widget,operate_action_args(op,op->action_now),act->animate_data);


    // if(op->action_now==LING_ACTION_ALL)g_print("progress:%f\n",act->ani_progress);
    // if(op->animation(op->velocity_x,op->velocity_y,0,op->animation_data)==LING_OPERATE_ANIMATION_REMOVE){
    //     ling_operate_run_finish(op);
    //     op->animation_timer_id=0;
    //     return G_SOURCE_REMOVE;
    // }
    return G_SOURCE_CONTINUE;
}

void ling_operate_run_animation(LingOperate * op){
    if(op==NULL||op->state!=LING_OPERATE_STATE_OPERATING)return;
    op->state = LING_OPERATE_STATE_ANIMATION;
    LingAction * act = &op->actions[op->action_now];
    if(act->release!=NULL)act->ani_dir = act->release(op->widget,operate_action_args(op,op->action_now),act->release_data);
    else act->ani_dir = LING_OPERATE_ANIMATION_DIR_FORWARD;
    if(act->animation==NULL){
        if(act->ani_dir==LING_OPERATE_ANIMATION_DIR_FORWARD){
            ling_operate_run_finish(op,LING_ACTION_FINISH_E);
        }
        else{
            ling_operate_run_finish(op,LING_ACTION_FINISH_S);
        }
        return;
    }
    //act->time=bezier_inverse(0,25,75,100,act->ani_progress);
    act->animation(op->widget,operate_action_args(op,op->action_now),act->animate_data);
    op->animation_timer_id = g_timeout_add(1000/op->controler->frames, ling_operate_animation_timeout, op);
}

void ling_operate_run_isbreaked(LingOperate * op){
    if(op==NULL)return;
    if(op->isbreaked==NULL){
        ling_operate_run_finish(op,LING_ACTION_FINISH_S);
        return;
    }
    LingAction * act = &op->actions[op->action_now];
    gboolean to=op->isbreaked(op->isbreaked_data);
    if(op->animation_timer_id!=0){
        g_source_remove(op->animation_timer_id);
        op->animation_timer_id=0;
    }
    if(to==LING_OPERATE_BREAK_TO_ANIMATION&&act->animation!=NULL){
        ling_operate_run_animation(op);
    }
    else{
        ling_operate_run_finish(op,LING_ACTION_FINISH_S);
    }
}

gboolean ling_operate_start_operating(LingOperate * op){
    //1.操作开始(开始霸体) 打断原来的 返回TRUE:能运行 返回FALSE:无法打断其他操作
    if(op==NULL)return FALSE;
    if(op->state==LING_OPERATE_STATE_OPERATING)return TRUE;
    if(op->force_run==TRUE){
        op->state = LING_OPERATE_STATE_OPERATING;
        return TRUE;
    }
    int op_is_on_num = 0;

    for(GList * l=op->controler->operates;l!=NULL;l=l->next){
        LingOperate * op_now = (LingOperate*)l->data;
        if(op==op_now){
            if(op_now->state!=LING_OPERATE_STATE_WAITTING)op_is_on_num++;
            continue;
        }
        if(op_now->state==LING_OPERATE_STATE_WAITTING)continue;
        if(op_now->state==LING_OPERATE_STATE_OPERATING){  //正在操作的有霸体
            return FALSE;
        }
        //STATE_ANIMATION
        op_is_on_num++;
        for(GList * b=op_now->breaker;b!=NULL;b=b->next){
            char * breaker_opname = (char *)b->data;
            if(strcmp(breaker_opname,op->operate_name->str)==0){
                ling_operate_run_isbreaked(op_now);
                op->state = LING_OPERATE_STATE_OPERATING;
                return TRUE;
            }
        }
    }
    if(op_is_on_num==0){
        op->state = LING_OPERATE_STATE_OPERATING;
        return TRUE;
    }
    return FALSE;
}

int ling_operate_get_action_now(LingOperate * op){
    return op->action_now;
}

void ling_operate_set_ani_progress_end(LingOperate * op,int action,gdouble progress_end){
    op->actions[action].ani_progress_end = progress_end;
}

gdouble ling_operate_get_ani_progress_end(LingOperate * op,int action){
    return op->actions[action].ani_progress_end;
}

// void ling_operate_set_change(LingOperate * op,const char * opname){
//     //1.5.操作改变,如抽屉上下
//     if(op==NULL||opname==NULL)return;
//     g_string_printf(op->operate_name,"%s",opname);
// }

// 1.6 改变打断者 ling_operate_change_breaker

//2.(非必需)动画 ANIMATION(cb)

//2.5 被打断    ISBREAKED(cb) (cb内执行3.6 3.7 3.8）

//2.6.设置反动画    ling_operate_set_animation_cb()

//2.7.设置反完成 ling_operate_set_finish_cb()

//2.8 重新设置打断 ling_operate_change_breaker()

//3->0.操作完成 FINISH(cb)

void ling_operate_swipe_cb(GtkGestureSwipe* self,
    gdouble velocity_x,gdouble velocity_y,gpointer user_data){
    LingOperate * op = user_data;
    op->velocity_x=velocity_x;
    op->velocity_y=velocity_y;
}

/*
 *
 */
// void ling_operate_add_dur(uint type,gboolean dir,){

// }



static gboolean longpress_timeout(gpointer user_data){
    LingOperate * op = (LingOperate*)user_data;
    op->longpress_id = 0;
    op->action_now = LING_ACTION_LONG_PRESS;
    op->actions[op->action_now].ani_progress = 0;
    ling_operate_start_operating(op);
    ling_operate_run_animation(op);
    return G_SOURCE_REMOVE;
}
//new

static void operate_drag_begin(GtkGestureDrag* self,
                             gdouble start_x,gdouble start_y,gpointer user_data){
    LingOperate * op = (LingOperate*)user_data;
    if(!op->able)return;
    if(op->state==LING_OPERATE_STATE_OPERATING)return;
    if(op->state==LING_OPERATE_STATE_ANIMATION)return;
    op->action_now=LING_ACTION_CLICK;
    op->start_x = start_x;
    op->start_y = start_y;
    op->longpress_id = g_timeout_add(500,longpress_timeout,op);
}

static void operate_drag_update(GtkGestureDrag* self,
                              gdouble offset_x,gdouble offset_y,gpointer user_data){
    LingOperate * op = (LingOperate*)user_data;
    if(!op->able)return;
    if(op->state==LING_OPERATE_STATE_ANIMATION)return;
    if(op->longpress_id!=0){
        g_source_remove(op->longpress_id);
        op->longpress_id=0;
    }
    op->offset_x = offset_x;
    op->offset_y = offset_y;
    // if(offset_y<=0&&op->offset_last>=0||offset_y>=0&&op->offset_last<=0){
    //     //及时结束不同方向的切换
    //     ling_operate_run_finish(op);
    // }

    if(op->action_now!=LING_ACTION_CLICK);
    else if(fabs(offset_x)>fabs(offset_y)){
        if(offset_x<0)op->action_now=LING_ACTION_DRAG_LEFT;
        else if(offset_x>0)op->action_now=LING_ACTION_DRAG_RIGHT;
    }
    else{
        if(offset_y<0)op->action_now=LING_ACTION_DRAG_UP;
        else if(offset_y>0)op->action_now=LING_ACTION_DRAG_DOWN;
    }

    if(op->actions[op->action_now].able==FALSE){
        //op->action_now=LING_ACTION_CLICK;
        return;
    }

    if(op->action_now==LING_ACTION_CLICK)return;

    if(op->action_now==LING_ACTION_DRAG_LEFT&&offset_x>0)return;
    if(op->action_now==LING_ACTION_DRAG_RIGHT&&offset_x<0)return;
    if(op->action_now==LING_ACTION_DRAG_UP&&offset_y>0)return;
    if(op->action_now==LING_ACTION_DRAG_DOWN&&offset_y<0)return;

    LingAction * act = &op->actions[op->action_now];
    if(ling_operate_start_operating(op)){
        if(act->progress!=NULL)act->ani_progress = act->progress(op->widget,operate_action_args(op,op->action_now),op->actions[op->action_now].progress_data);
        else act->ani_progress = 0;
        if(act->ani_progress>100)act->ani_progress=100;
        if(act->ani_progress<0)act->ani_progress=0;
        if(act->animation!=NULL)act->animation(op->widget,operate_action_args(op,op->action_now),act->animate_data);
        //g_print("update\n");
    }
}

static void operate_drag_end(GtkGestureDrag* self,
                             gdouble offset_x,gdouble offset_y,gpointer user_data){
    LingOperate * op = (LingOperate*)user_data;
    if(!op->able)return;

    if(op->state==LING_OPERATE_STATE_ANIMATION)return;
    if(op->longpress_id!=0){
        g_source_remove(op->longpress_id);
        op->longpress_id=0;
    }
    if(op->action_now == LING_ACTION_CLICK && op->actions[op->action_now].able){
        op->actions[op->action_now].ani_progress = 0;
        ling_operate_start_operating(op);
    }
    else if(op->actions[LING_ACTION_ALL].able){
        op->action_now = LING_ACTION_ALL;
        op->actions[op->action_now].ani_progress = 0;
        ling_operate_start_operating(op);
    }
    // if(op->longpress_status){

    // }
    ling_operate_run_animation(op);
}

void ling_operate_set_able(LingOperate * op,gboolean able){
    op->able = able;
}

static void ignore_drag_begin(GtkGestureDrag* self,
                              gdouble start_x,gdouble start_y,gpointer user_data){
    LingOperate * op = user_data;
    op->able = FALSE;
}

static void ignore_drag_end(GtkGestureDrag* self,
                            gdouble offset_x,gdouble offset_y,gpointer user_data){
    LingOperate * op = user_data;
    op->able = TRUE;
}

GtkGesture * ling_operate_drag_ignore(LingOperate * op,GtkWidget * widget){
    GtkGesture * drag = gtk_gesture_drag_new();
    gtk_widget_add_controller(widget, GTK_EVENT_CONTROLLER(drag));

    gtk_event_controller_set_propagation_phase(GTK_EVENT_CONTROLLER(drag), GTK_PHASE_CAPTURE);
    g_signal_connect(drag, "drag-begin", G_CALLBACK(ignore_drag_begin), op);
    g_signal_connect(drag, "drag-end", G_CALLBACK(ignore_drag_end), op);
    return drag;
}



void ling_operate_add_action(LingOperate * op,uint type,
                             PROGRESS progress,gpointer progress_data,
                             ANIMATION animate,gpointer animate_data,
                             RELEASE release,gpointer release_data,
                             FINISH finish_s,FINISH finish_e,gpointer finish_data){
    LingAction * act = &op->actions[type];
    act->progress = progress;
    act->progress_data = progress_data;
    act->animation = animate;
    act->animate_data = animate_data;
    act->release = release;
    act->release_data = release_data;
    act->able = TRUE;
    act->finish_e = finish_e;
    act->finish_s = finish_s;
    act->finish_data = finish_data;
    act->ani_progress_end = 100;
    act->ani_time = 0.2;

    if(op->drag==NULL){
        op->drag =gtk_gesture_drag_new();
        gtk_widget_add_controller(GTK_WIDGET(op->widget), GTK_EVENT_CONTROLLER(op->drag));
        g_signal_connect(op->drag, "drag-begin", G_CALLBACK(operate_drag_begin), op);
        g_signal_connect(op->drag, "drag-update", G_CALLBACK(operate_drag_update), op);
        g_signal_connect(op->drag, "drag-end", G_CALLBACK(operate_drag_end), op);
        gtk_event_controller_set_propagation_phase(GTK_EVENT_CONTROLLER(op->drag), GTK_PHASE_CAPTURE);
    }
    if(op->swipe==NULL){
        op->swipe = gtk_gesture_swipe_new();
        gtk_widget_add_controller(GTK_WIDGET(op->widget), GTK_EVENT_CONTROLLER(op->swipe));
        g_signal_connect(op->swipe,"swipe",G_CALLBACK(ling_operate_swipe_cb),op);
    }
}

static GdkContentProvider * operate_drag_source_prepare (
    GtkDragSource* self,gdouble x,gdouble y,gpointer user_data){
    LingOperate * op = (LingOperate*)user_data;
    if(!op->actions[LING_ACTION_DRAG_SOURCE].able)return NULL;

    if(op->actions[LING_ACTION_DRAG_SOURCE].prepare!=NULL){
        return op->actions[LING_ACTION_DRAG_SOURCE].prepare(op->widget,
            operate_action_args(op,op->action_now),op->actions[LING_ACTION_DRAG_SOURCE].prepare_data);
    }
    return NULL;
}

static void operate_drag_source_begin(GtkDragSource* self,GdkDrag* drag,gpointer user_data){
    LingOperate * op = (LingOperate*)user_data;
    if(!op->actions[LING_ACTION_DRAG_SOURCE].able)return;
    //Ling
}

static gboolean operate_drag_source_cancel (
    GtkDragSource* self,GdkDrag* drag,GdkDragCancelReason* reason,gpointer user_data){

}

static void operate_drag_source_end(GtkDragSource* self,
    GdkDrag* drag,gboolean delete_data,gpointer user_data){

}

void ling_operate_add_drag_source(LingOperate * op,uint type,
                             PREPARE prepare,gpointer prepare_data,
                             ANIMATION animate,gpointer animate_data,
                             RELEASE release,gpointer release_data,
                             FINISH finish,gpointer finish_data){
    LingAction * act = &op->actions[type];
    act->prepare = prepare;
    act->prepare_data = prepare_data;
    act->animation = animate;
    act->animate_data = animate_data;
    act->release = release;
    act->release_data = release_data;
    act->able = TRUE;
    act->finish_e = finish;
    act->finish_data = finish_data;
    act->ani_progress_end = 100;
    act->ani_time = 0.2;

    op->drag_source = gtk_drag_source_new();
    gtk_widget_add_controller(op->widget,GTK_EVENT_CONTROLLER(op->drag_source));
    g_signal_connect(op->drag_source,"prepare",G_CALLBACK(operate_drag_source_prepare),op);
    g_signal_connect(op->drag_source,"drag_begin",G_CALLBACK(operate_drag_source_begin),op);
    g_signal_connect(op->drag_source,"drag-cancel",G_CALLBACK(operate_drag_source_cancel),op);
    g_signal_connect(op->drag_source,"drag-end",G_CALLBACK(operate_drag_source_end),op);
}

void ling_operate_emit(LingOperate * op,gpointer emit_data){
    if(op->actions[LING_ACTION_ALL].able){
        op->action_now = LING_ACTION_ALL;
    }
    else{
        op->action_now = LING_ACTION_EMIT;
    }
    op->emit_data = emit_data;
    op->actions[op->action_now].ani_progress = 0;
    ling_operate_start_operating(op);
    ling_operate_run_animation(op);
}

void ling_operate_emit_close(LingOperate * op,gpointer emit_data,gboolean S_E){
    if(op->actions[LING_ACTION_ALL].able){
        op->action_now = LING_ACTION_ALL;
    }
    else{
        op->action_now = LING_ACTION_EMIT;
    }
    op->emit_data = emit_data;
    op->actions[op->action_now].ani_progress = 0;
    ling_operate_start_operating(op);
    ling_operate_run_finish(op,S_E);
}


void ling_operate_set_force_run(LingOperate * op,gboolean force_run){
    op->force_run = force_run;
}

