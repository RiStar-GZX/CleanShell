#include "lingoperate.h"

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

LingOperate * ling_operate_add(LingOpControler * controler,const char * op_name,
                               ANIMATION animation,gpointer animation_data,ISBREAKED isbreaked,gpointer isbreaked_data,FINISH finish,gpointer finish_data){
    if(op_name==NULL||controler==NULL)return NULL;
//    for(GList * l=controler->operates;l!=NULL;l=l->next){
//        LingOperate * op_now = (LingOperate*)l->data;
//        if(strcmp(op_now->operate_name->str,op_name)==0){
//            return NULL;
//        }
//    }
    char * new_name = ling_operate_new_name(controler,op_name);
    //g_print("new_name:%s\n",new_name);
    LingOperate * op = malloc(sizeof(LingOperate));
    op->controler = controler;
    op->operate_name = g_string_new(new_name);
    free(new_name);
    op->state = LING_OPERATE_STATE_WAITTING;
    ling_operate_set_animation_cb(op,animation,animation_data);
    ling_operate_set_isbreaked_cb(op,isbreaked,isbreaked_data);
    ling_operate_set_finish_cb(op,finish,finish_data);
    op->breaker = NULL;
    controler->operates = g_list_append(controler->operates,op);
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

void ling_operate_set_animation_cb(LingOperate * op,ANIMATION cb,gpointer data){
    //设置动画，定时器每帧运行一次CALLBACK
    op->animation = cb;
    op->animation_data = data;
}

void ling_operate_set_isbreaked_cb(LingOperate * op,ISBREAKED cb,gpointer data){
    //设置动画，定时器每帧运行一次CALLBACK
    op->isbreaked = cb;
    op->isbreaked_data = data;
}

void ling_operate_set_finish_cb(LingOperate * op,FINISH cb,gpointer data){
    //动画结束后或者到位了，操作完成
    if(cb==NULL)return;
    op->finish = cb;
    op->finish_data = data;
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
void ling_operate_run_finish(LingOperate * op){
    if(op==NULL||op->state==LING_OPERATE_STATE_WAITTING)return;
    op->state = LING_OPERATE_STATE_WAITTING;
    if(op->finish!=NULL)op->finish(op->finish_data);
}

gboolean ling_operate_animation_timeout(gpointer user_data){
    LingOperate * op = (LingOperate*)user_data;
    if(op->animation(op->velocity_x,op->velocity_y,op->animation_data)==LING_OPERATE_ANIMATION_REMOVE){
        ling_operate_run_finish(op);
        op->animation_timer_id=0;
        return G_SOURCE_REMOVE;
    }
    return G_SOURCE_CONTINUE;
}

void ling_operate_run_animation(LingOperate * op){
    if(op==NULL||op->state!=LING_OPERATE_STATE_OPERATING)return;
    op->state = LING_OPERATE_STATE_ANIMATION;
    if(op->animation==NULL){
        ling_operate_run_finish(op);
        return;
    }
    op->animation_timer_id = g_timeout_add(1000/op->controler->frames, ling_operate_animation_timeout, op);
}

void ling_operate_run_isbreaked(LingOperate * op){
    if(op==NULL)return;
    if(op->isbreaked==NULL){
        ling_operate_run_finish(op);
        return;
    }
    gboolean to=op->isbreaked(op->isbreaked_data);
    if(op->animation_timer_id!=0){
        g_source_remove(op->animation_timer_id);
        op->animation_timer_id=0;
    }
    if(to==LING_OPERATE_BREAK_TO_ANIMATION&&op->animation!=NULL){
        ling_operate_run_animation(op);
    }
    else{
        ling_operate_run_finish(op);
    }
}

gboolean ling_operate_start_operating(LingOperate * op){
    //1.操作开始(开始霸体) 打断原来的 返回TRUE:能运行 返回FALSE:无法打断其他操作
    if(op==NULL)return FALSE;
    if(op->state==LING_OPERATE_STATE_OPERATING)return TRUE;
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


void ling_operate_set_change(LingOperate * op,const char * opname){
    //1.5.操作改变,如抽屉上下
    if(op==NULL||opname==NULL)return;
    g_string_printf(op->operate_name,"%s",opname);
}

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



