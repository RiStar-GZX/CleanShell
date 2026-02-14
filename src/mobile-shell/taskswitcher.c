#include "taskswitcher.h"
#include <lingviewpager.h>
#include <lingwallpaper.h>
#include <desktop.h>
#include <wm/wm.h>

#define ANI_NAME_TASK_FULL_SHOW "task_full_show"
#define ANI_NAME_TASK_POS       "task_pos"
#define ANI_NAME_TASK_RETURN    "task_teturn"

enum {
    BACK,
    SIGNAL_NUM
};

typedef enum{
    TASK_SWITCHER_WIN =0,
    TASK_SWITCHER_SWITCH,
}TASK_SWITCHER_MODE;

static guint signals[SIGNAL_NUM];

typedef struct{
    gdouble target_x;
    gdouble target_y;
    int target_w;
    int target_h;

    gdouble start_x;
    gdouble start_y;
    int start_w;
    int start_h;

    int offset; //左左右右
}full_show;

struct _ClmTaskSwitcher{
    GtkBox parent;
    uint mode;
    //条
    GtkWidget * bar;
    full_show info;
    uint fs;
};

G_DEFINE_FINAL_TYPE(ClmTaskSwitcher,clm_task_switcher,GTK_TYPE_BOX);

static int target_space=30,task_target_w=300,task_target_h=600;
void clm_task_switcher_show(ClWm * wm,gdouble x,gdouble y,int w,int h,uint space){
    ClWmWindow * win = cl_wm_get_current_window(CL_WM(shell->wm));
    GList * lcrt = cl_wm_get_window_list(wm);
    for(;;lcrt=lcrt->next){
        if(lcrt==NULL)return;
        if(lcrt->data==win)break;
    }

    int ww= gtk_widget_get_width(GTK_WIDGET(wm));
    gdouble px = 30;
    if(w>task_target_w){
        px=30-30*(gdouble)(w-task_target_w)/(gdouble)(ww-task_target_w);
    }


    gdouble new_space = space;
    space*=w/(gdouble)task_target_w;

    cl_wm_set_window_size(win,w,h);
    cl_wm_move_window(win,x,y);
    cl_wm_set_window_radis(win,px);

    int i;
    GList * l;
    for(i=1,l=lcrt->prev;l!=NULL;l=l->prev,i++){
        win = CL_WM_WINDOW(l->data);
        gdouble xx= x-i*(w+space);
        cl_wm_set_window_showable(win,TRUE);
        cl_wm_move_window(win,xx,y);
        cl_wm_set_window_size(win,w,h);
        cl_wm_window_set_gradient_progress(win,1);

        cl_wm_set_window_radis(win,px);
    }
    for(i=1,l=lcrt->next;l!=NULL;l=l->next,i++){
        win = CL_WM_WINDOW(l->data);
        gdouble xx= x+i*(w+space);
        cl_wm_set_window_showable(win,TRUE);
        cl_wm_move_window(win,xx,y);
        cl_wm_set_window_size(win,w,h);
        cl_wm_window_set_gradient_progress(win,1);

        cl_wm_set_window_radis(win,px);
    }
}

/*----------------------------------------------------------------------------------------------------*/

static void clm_task_switcher_class_init(ClmTaskSwitcherClass * klass){
    signals[BACK] = g_signal_new(
        "back",                         // 信号名称
        G_TYPE_FROM_CLASS(klass),       // 发射此信号的类类型
        G_SIGNAL_RUN_FIRST,             // 在其他处理程序之前运行（可选）
        0,                              // 关闭的数据结构（通常为 NULL）
        NULL,                           // 不传递参数给下一个处理者（可选）
        NULL,                           // 不需要参数列表 (可选)
        NULL,
        G_TYPE_NONE,                    // 返回值类型（这里是 void）
        4,
        G_TYPE_DOUBLE,
        G_TYPE_DOUBLE,
        G_TYPE_DOUBLE,
        G_TYPE_DOUBLE
        );
}

/*----------------------------------------------------------------------------------------------------*/

static ANI_DIR task_return_release(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    ClmTaskSwitcher * ts = CLM_TASK_SWITCHER(user_data);
    ClWmWindow * win = cl_wm_get_current_window(CL_WM(shell->wm));
    cl_wm_window_get_info(win,&ts->info.start_x,&ts->info.start_y,&ts->info.start_w,&ts->info.start_h);

    GList *l,*lcrt = cl_wm_get_window_list(CL_WM(shell->wm));
    if(lcrt==NULL)return ANI_DIR_BACK;

    int i;
    for(i=-1,l=lcrt;l!=NULL;l=l->next,i--);
    ts->info.target_x = i*(ts->info.start_w+target_space);

    return ANI_DIR_FORWARD;
}

static void task_return_ani(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    ClmTaskSwitcher * ts = CLM_TASK_SWITCHER(user_data);
    gdouble x = ts->info.start_x+(ts->info.target_x-ts->info.start_x)*(args.progress/100);
    clm_task_switcher_show(CL_WM(shell->wm),x,ts->info.start_y,ts->info.start_w,ts->info.start_h,target_space);

    clm_desktop_set_wallpaper_blur(CLM_DESKTOP(shell->desktop),(1-args.progress/100)*CLM_DESKTOP_BLUR);
}

static void task_return_finish(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    ClmTaskSwitcher * ts = CLM_TASK_SWITCHER(user_data);
    for(GList * l=cl_wm_get_window_list(CL_WM(shell->wm));l!=NULL;l=l->next){
        ClWmWindow * win = CL_WM_WINDOW(l->data);
        cl_wm_set_window_showable(win,FALSE);
    }
    gtk_widget_set_visible(shell->wm,FALSE);

    ts->mode = TASK_SWITCHER_WIN;
}
/*----------------------------------------------------------------------------------------------------*/

static ANI_DIR task_pos_release(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    ClmTaskSwitcher * ts = CLM_TASK_SWITCHER(user_data);
    ClWmWindow * win = cl_wm_get_current_window(CL_WM(shell->wm));
    cl_wm_window_get_info(win,&ts->info.start_x,&ts->info.start_y,&ts->info.start_w,&ts->info.start_h);
    ts->info.target_w = task_target_w;
    ts->info.target_h = task_target_h;

    int w = gtk_widget_get_width(shell->wm);
    int h = gtk_widget_get_height(shell->wm);

    ts->info.target_x = (w-task_target_w)/2;
    ts->info.target_y = (h-task_target_h)/2;

    return ANI_DIR_FORWARD;
}

static void task_pos_ani(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    ClmTaskSwitcher * ts = CLM_TASK_SWITCHER(user_data);

    gdouble x = ts->info.start_x+(ts->info.target_x-ts->info.start_x)*(args.progress/100);
    gdouble y = ts->info.start_y+(ts->info.target_y-ts->info.start_y)*(args.progress/100);
    int w =  ts->info.start_w+(ts->info.target_w-ts->info.start_w)*(args.progress/100);
    int h =  ts->info.start_h+(ts->info.target_h-ts->info.start_h)*(args.progress/100);

    clm_task_switcher_show(CL_WM(shell->wm),x,y,w,h,target_space);
}

static void task_pos_finish(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    ClmTaskSwitcher * ts = CLM_TASK_SWITCHER(user_data);
    ts->mode = TASK_SWITCHER_SWITCH;
}

/*----------------------------------------------------------------------------------------------------*/
static int start_space = 300;
// static gboolean task_full_show_release(GtkWidget * widget,LingActionArgs args,gpointer user_data){
//     ClmTaskSwitcher * ts = CLM_TASK_SWITCHER(user_data);
//     return LING_OPERATE_ANIMATION_DIR_FORWARD;
// }

static void task_full_show_ani(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    ClmTaskSwitcher * ts = CLM_TASK_SWITCHER(user_data);
    full_show * info = &ts->info;
    gdouble start_x,start_y;
    int w,h;
    ClWmWindow * win = cl_wm_get_current_window(CL_WM(shell->wm));
    cl_wm_window_get_info(win,&start_x,&start_y,&w,&h);

    clm_task_switcher_show(CL_WM(shell->wm),start_x,start_y,w,h,start_space-(start_space-target_space)*(args.progress/100));
}

static void task_full_finish(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    ClmTaskSwitcher * ts = CLM_TASK_SWITCHER(user_data);
    ts->fs=1;
}

/*----------------------------------------------------------------------------------------------------*/

static gdouble task_switch_progress(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    ClmTaskSwitcher * ts = CLM_TASK_SWITCHER(user_data);
    if(args.offset_y>0)args.offset_y=0;

    if(cl_wm_get_current_window(CL_WM(shell->wm))==NULL)return 100;
    uint w=gtk_widget_get_width(shell->wm);
    uint h=gtk_widget_get_height(shell->wm);
    gdouble aspect=(gdouble)w/(gdouble)h;
    int new_w=w,new_h=h;
    gdouble new_x=0,new_y=0;

    if(ts->mode==TASK_SWITCHER_WIN){
        if(args.offset_y<-300)args.offset_y=-300;
        if(args.offset_y<=0)new_h+=args.offset_y*2;
        new_w=new_h*aspect;
        new_x = (w-new_w)/2+args.offset_x;
        if(args.offset_y<0)new_y = -args.offset_y/2;
        // if(fabs(args.velocity_y)>100){
        //     return 100;
        // }
        // else{
            //if(ts->fs==0)ling_operate_emit_end(ling_operate_get(shell->controler,ANI_NAME_TASK_FULL_SHOW),LING_ACTION_ANIMATE,NULL,TRUE);
            // else{
                clm_task_switcher_show(CL_WM(shell->wm),new_x,new_y,new_w,new_h,target_space);
            //}
        //}
    }
    if(ts->mode==TASK_SWITCHER_SWITCH){
        if(args.offset_y<-300)args.offset_y=-300;
        if(args.offset_y<=0)new_h=task_target_h+args.offset_y;

        new_w = new_h*aspect;
        new_x = (w-new_w)/2+args.offset_x;
        if(args.offset_y<=0)new_y = (h-task_target_h)/2+args.offset_y/50;
        ClWmWindow * win = cl_wm_get_current_window(CL_WM(shell->wm));
        clm_task_switcher_show(CL_WM(shell->wm),new_x,new_y,new_w,new_h,target_space);
    }
    return 100;
}

static ANI_DIR task_switch_release(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    ClmTaskSwitcher * ts = CLM_TASK_SWITCHER(user_data);
    ts->fs = 0;
    return ANI_DIR_FORWARD;
}

static void task_switch_finish_e(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    ClmTaskSwitcher * ts = CLM_TASK_SWITCHER(user_data);
    g_signal_emit_by_name(ts,"back",args.offset_x,args.offset_y,args.velocity_x,args.velocity_y);

    ClWmWindow * wincur = cl_wm_get_current_window(CL_WM(shell->wm));
    if(wincur==NULL)return;

    if(fabs(args.velocity_y)<50&&fabs(args.offset_y)<100){
        ling_operate_emit_end(ling_operate_get(shell->controler,ANI_NAME_TASK_POS),LING_ACTION_ANIMATE,NULL,TRUE);
        return;
    }

    if(ts->mode==TASK_SWITCHER_WIN){
        //关闭其他窗口
        for(GList * l=cl_wm_get_window_list(CL_WM(shell->wm));l!=NULL;l=l->next){
            ClWmWindow * win = CL_WM_WINDOW(l->data);
            if(win!=wincur)cl_wm_set_window_showable(win,FALSE);
        }
        cl_wm_close_current_window(CL_WM(shell->wm),args.offset_x,args.offset_y,args.velocity_x,args.velocity_y);
    }
    if(ts->mode==TASK_SWITCHER_SWITCH){
        // for(GList * l=cl_wm_get_window_list(CL_WM(shell->wm));l!=NULL;l=l->next){
        //     ClWmWindow * win = CL_WM_WINDOW(l->data);
        //     if(win!=wincur)cl_wm_set_window_showable(win,FALSE);
        // }
        //cl_wm_close_current_window(CL_WM(shell->wm),args.offset_x,args.offset_y,args.velocity_x,args.velocity_y);
        ling_operate_emit_end(ling_operate_get(shell->controler,ANI_NAME_TASK_RETURN),LING_ACTION_ANIMATE,NULL,TRUE);
        //ts->mode=TASK_SWITCHER_WIN;
    }
}
/*----------------------------------------------------------------------------------------------------*/

// static gboolean wm_release(GtkWidget * widget,LingActionArgs args,gpointer user_data){
//     ClmTaskSwitcher * ts = CLM_TASK_SWITCHER(user_data);
//     if(ts->mode!=TASK_SWITCHER_SWITCH)return LING_OPERATE_ANIMATION_DIR_BACK;

// }

static gdouble wm_progress(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    ClmTaskSwitcher * ts = CLM_TASK_SWITCHER(user_data);
    if(ts->mode!=TASK_SWITCHER_SWITCH)return 0;

    //获取当前尺寸
    ClWmWindow * win = cl_wm_get_current_window(CL_WM(shell->wm));
    int w,h;
    cl_wm_window_get_info(win,NULL,NULL,&w,&h);


    GList *l,*lcrt = cl_wm_get_window_list(CL_WM(shell->wm));
    for(;;lcrt=lcrt->next){
        if(lcrt==NULL)return 0;
        if(lcrt->data==win)break;
    }

    int i;
    gdouble x,sx,ex;
    ts->info.offset = 0;
    ling_operate_set_ani_progress_end(args.op,LING_ACTION_DRAG_ALL,0);
    for(i=1,l=lcrt->prev;l!=NULL;l=l->prev,i++){
        x = i*(w+target_space);
        sx=x-w/2,ex=x+w/2;
        if((args.offset_x>=sx&&args.offset_x<=ex)||
            (l->prev==NULL&&args.offset_x>ex)){
            ts->info.offset = i;
            ling_operate_set_ani_progress_end(args.op,LING_ACTION_DRAG_ALL,x);
            g_print("%f sx:%f ex:%f %f %d\n",args.offset_x,sx,ex,x,i);
        }
    }
    for(i=-1,l=lcrt->next;l!=NULL;l=l->next,i--){
        x = i*(w+target_space);
        sx=x-w/2,ex=x+w/2;
        if((args.offset_x>=sx&&args.offset_x<=ex)||
            (l->next==NULL&&args.offset_x<sx)){
            ts->info.offset = i;
            ling_operate_set_ani_progress_end(args.op,LING_ACTION_DRAG_ALL,x);
            g_print("%f sx:%f ex:%f %f %d\n",args.offset_x,sx,ex,x,i);
        }
    }

    ling_operate_set_ani_dir(args.op,args.action,ANI_DIR_NEAR);
    return args.offset_x;
}

static void wm_ani(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    ClmTaskSwitcher * ts = CLM_TASK_SWITCHER(user_data);
    if(ts->mode!=TASK_SWITCHER_SWITCH)return;

    //g_print("arg:%f %f\n",args.offset_x,args.offset_y);

    ClWmWindow * win = cl_wm_get_current_window(CL_WM(shell->wm));
    int w,h;
    cl_wm_window_get_info(win,NULL,NULL,&w,&h);

    int ww = gtk_widget_get_width(shell->wm);
    int hh = gtk_widget_get_height(shell->wm);
    gdouble start_x=(ww-task_target_w)/2,start_y=(hh-task_target_h)/2;
    // start_x
    clm_task_switcher_show(CL_WM(shell->wm),start_x+args.progress,start_y,w,h,target_space);
}

static void wm_finish(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    ClmTaskSwitcher * ts = CLM_TASK_SWITCHER(user_data);
    if(ts->mode!=TASK_SWITCHER_SWITCH)return;

    ClWmWindow * win = cl_wm_get_current_window(CL_WM(shell->wm));
    GList *l,*lcrt = cl_wm_get_window_list(CL_WM(shell->wm));
    for(;;lcrt=lcrt->next){
        if(lcrt==NULL)return;
        if(lcrt->data==win)break;
    }
    int i;

    i=1;
    for(l=lcrt->prev;l!=NULL;l=l->prev){
        if(i==ts->info.offset){
            win = CL_WM_WINDOW(l->data);
            cl_wm_set_current_window(CL_WM(shell->wm),win);
            break;
        }
        i++;
    }

    i=-1;
    for(l=lcrt->next;l!=NULL;l=l->next){
        if(i==ts->info.offset){
            win = CL_WM_WINDOW(l->data);
            cl_wm_set_current_window(CL_WM(shell->wm),win);
            break;
        }
        i--;
    }

    // for(i=-1,l=lcrt->next;l!=NULL&&i!=ts->info.offset;l=l->next,i--);
    // if(l!=NULL){
    //     win = CL_WM_WINDOW(l->data);
    //     cl_wm_set_current_window(CL_WM(shell->wm),win);
    // }

    //cl_wm_set_current_window_size();
}

/*----------------------------------------------------------------------------------------------------*/

static void clm_task_switcher_init(ClmTaskSwitcher * self){
    gtk_orientable_set_orientation(GTK_ORIENTABLE(self),GTK_ORIENTATION_VERTICAL);
    gtk_widget_set_valign(GTK_WIDGET(self),GTK_ALIGN_END);
    gtk_widget_set_halign(GTK_WIDGET(self),GTK_ALIGN_CENTER);

    //显示所有任务缩略图动画
    ling_operate_add_animate(shell->controler,ANI_NAME_TASK_FULL_SHOW,
                             NULL,NULL,
                             task_full_show_ani,self,
                             NULL,task_full_finish,self);

    //所有任务缩略图归位动画
    ling_operate_add_animate(shell->controler,ANI_NAME_TASK_POS,
                             task_pos_release,self,
                             task_pos_ani,self,
                             NULL,task_pos_finish,self);

    //所有任务缩略图左移退出
    ling_operate_add_animate(shell->controler,ANI_NAME_TASK_RETURN,
                             task_return_release,self,
                             task_return_ani,self,
                             NULL,task_return_finish,self);

    //任务切换条
    self->bar = gtk_label_new("");
    gtk_widget_add_css_class(self->bar,"task_switcher_bar");
    gtk_widget_set_halign(self->bar,GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(self->bar,120,10);
    gtk_box_append(GTK_BOX(self),self->bar);

    LingOperate * op = ling_operate_add(shell->controler,"task_switcher_bar_click",self->bar);
    ling_operate_add_action(op,LING_ACTION_DRAG_ALL,task_switch_progress,self,
                            NULL,NULL,
                            task_switch_release,self,
                            NULL,task_switch_finish_e,self);

    //
    LingOperate * wmop = ling_operate_add(shell->controler,"wm_op",shell->wm);
    ling_operate_add_action(wmop,LING_ACTION_DRAG_ALL,wm_progress,self,
                            wm_ani,self,
                            NULL,NULL,
                            wm_finish,wm_finish,self);

    // ling_operate_add_action(wmop,LING_ACTION_INSTANT,NULL,NULL,
    //                         NULL,NULL,
    //                         wm_release,self,
    //                         NULL,NULL,NULL);
}

GtkWidget * clm_task_switcher_new(){
    return g_object_new(CLM_TYPE_TASK_SWITCHER,NULL);
}


