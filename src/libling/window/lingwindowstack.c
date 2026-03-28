#include "lingwindowstack.h"

typedef struct{
    GtkWidget * window;
    LING_WINDOW_STACK_ANI ani_type;
}StackWindow;

struct _LingWindowStackLayout{
    GtkLayoutManager parent;
};

G_DEFINE_FINAL_TYPE(LingWindowStackLayout,ling_window_stack_layout,GTK_TYPE_LAYOUT_MANAGER);

struct _LingWindowStack{
    GtkWidget parent;
    GtkLayoutManager * manager;
    GList * windows;
    LingOperate * ani;
    gdouble ani_progress;
    //GList只保存window的信息,不讲究顺序,LingWindowStack.child是一个栈
};

G_DEFINE_FINAL_TYPE(LingWindowStack,ling_window_stack,GTK_TYPE_WIDGET);

static GList * ling_window_stack_get_list(LingWindowStack * stack,GtkWidget * window);

static StackWindow * ling_window_stack_get_win(LingWindowStack * stack,GtkWidget * window);

static void ling_window_stack_layout_allocate(GtkLayoutManager *manager,GtkWidget *widget,
                                        int width,int height,int baseline){
    GtkWidget * parent = gtk_widget_get_parent(widget);
    if(parent==NULL)return;

    if(width<0){
        width = gtk_widget_get_width(widget);
    }
    if(height<0){
        height = gtk_widget_get_height(widget);
    }

    graphene_rect_t rect;
    if(!gtk_widget_compute_bounds(parent,widget,&rect))return;
    int start = gtk_widget_get_margin_start(widget);
    int end = gtk_widget_get_margin_end(widget);
    int top = gtk_widget_get_margin_top(widget);
    int bottom = gtk_widget_get_margin_bottom(widget);
    rect.size.height-=top+bottom;
    rect.size.width-=start+end;
    LingWindowStack * stack = LING_WINDOW_STACK(widget);

    GtkAllocation win2_alloc = {
        .x = 0,.y = 0,
        .width = width,.height = height};
    GtkAllocation win1_alloc = {
        .x = 0,.y = 0,
        .width = width,.height = height};

    GtkWidget * win1 = gtk_widget_get_last_child(widget);
    if(win1==NULL){
        return;
    }
    StackWindow * sw = ling_window_stack_get_win(stack,win1);
    GtkWidget * win2 = gtk_widget_get_prev_sibling(win1);
    // GtkWidget * t = win2;
    // win2 = win1;
    // win1 = t;
    if(win1)gtk_widget_set_visible(win1,TRUE);
    if(win2)gtk_widget_set_visible(win2,TRUE);
    if(stack->ani_progress>=100||win2==NULL){
        //只显示最上层
        gtk_widget_set_visible(win2,FALSE);
        gtk_widget_size_allocate(win1, &win1_alloc, baseline);
        return;
    }
    else{
        if(sw->ani_type==LING_WINDOW_STACK_ANI_LEFT){
            win2_alloc.x = win2_alloc.width*(stack->ani_progress/100-1);
            win1_alloc.x = win2_alloc.x+win2_alloc.width;
        }
        if(sw->ani_type==LING_WINDOW_STACK_ANI_TOP){
            win2_alloc.y = win2_alloc.height*(stack->ani_progress/100-1);
            win1_alloc.y = win2_alloc.y+win2_alloc.height;
        }
        if(sw->ani_type==LING_WINDOW_STACK_ANI_RIGHT){
            win2_alloc.x = width-win2_alloc.width*(stack->ani_progress/100);
            win1_alloc.x = win2_alloc.x-win2_alloc.width;
        }
        if(sw->ani_type==LING_WINDOW_STACK_ANI_BOTTOM){
            win2_alloc.y = height-win2_alloc.height*(stack->ani_progress/100);
            win1_alloc.y = win2_alloc.y-win2_alloc.height;
        }

        gtk_widget_size_allocate(win2, &win1_alloc, baseline);
        gtk_widget_size_allocate(win1, &win2_alloc, baseline);
    }
    //g_print("progress:%f %d %d\n",stack->ani_progress,win2_alloc.width,win2_alloc.height);
    //剩下的窗口全部不显示
    for(GtkWidget * w = gtk_widget_get_prev_sibling(win2);
         w!=NULL;w = gtk_widget_get_prev_sibling(w)){
        gtk_widget_set_visible(w,FALSE);
    }
}

static void ling_window_stack_layout_class_init(LingWindowStackLayoutClass * klass){
    GtkLayoutManagerClass * manager_class = GTK_LAYOUT_MANAGER_CLASS(klass);
    manager_class->allocate = ling_window_stack_layout_allocate;
}

static void ling_window_stack_layout_init(LingWindowStackLayout * self){
}

GtkLayoutManager * ling_window_stack_layout_new(){
    return g_object_new(LING_TYPE_WINDOW_STACK_LAYOUT,NULL);
}

static void ling_window_stack_class_init(LingWindowStackClass * klass){
}

// static void ling_window_update_allocate(LingWindowStack * stack){
//     gtk_layout_manager_allocate(stack->manager,GTK_WIDGET(stack),-1,-1,-1);
// }

static void window_stack_switch_ani(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    LingWindowStack * stack = LING_WINDOW_STACK(user_data);
    GtkWidget * top = gtk_widget_get_last_child(GTK_WIDGET(stack));
    if(top==NULL)return;
    stack->ani_progress = args.progress;

    //g_print("act->ani_progress:%f\n",args.progress);
    gtk_layout_manager_allocate(stack->manager,GTK_WIDGET(stack),-1,-1,-1);
}

static void window_stack_pop(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    LingWindowStack * stack = LING_WINDOW_STACK(user_data);
    ling_window_stack_pop(stack);
}

static void ling_window_stack_init(LingWindowStack * self){
    self->manager = ling_window_stack_layout_new();
    gtk_widget_set_layout_manager(GTK_WIDGET(self),self->manager);
    self->ani_progress = 100;

    //默认方向0->100是添加新页面(push)0完全不显示,100完全显示
    self->ani = ling_operate_add_animate(shell->controler,"switch",NULL,NULL,
                                         window_stack_switch_ani,self,
                                         NULL,NULL,self);
    ling_operate_set_ani_time(self->ani,LING_ACTION_ANIMATE,2);
}

GtkWidget * ling_window_stack_new(){
    return g_object_new(LING_TYPE_WINDOW_STACK,NULL);
}

static GList * ling_window_stack_get_list(LingWindowStack * stack,GtkWidget * window){
    if(stack==NULL||window==NULL)return NULL;
    for(GList * l = stack->windows;l!=NULL;l=l->next){
        StackWindow * sw = l->data;
        if(sw->window!=window)continue;
        return l;
    }
    return NULL;
}

static StackWindow * ling_window_stack_get_win(LingWindowStack * stack,GtkWidget * window){
    GList * l = ling_window_stack_get_list(stack,window);
    if(l==NULL)return NULL;
    return l->data;
}

void ling_window_stack_add_window(LingWindowStack * stack,GtkWidget * window){
    if(stack == NULL||window == NULL)return;
    StackWindow * sw = g_malloc0(sizeof(StackWindow));
    sw->window = window;
    //sw->ani_progress = 100;
    stack->windows = g_list_append(stack->windows,sw);
}

//彻底移除
void ling_window_stack_remove_window(LingWindowStack * stack,GtkWidget * window){
    GList * l = ling_window_stack_get_list(stack,window);
    if(l==NULL)return;
    StackWindow * sw = l->data;
    gtk_widget_unparent(sw->window);
    stack->windows = g_list_remove(stack->windows,l->data);
}

//设置一个窗口在顶部显示
int ling_window_stack_push(LingWindowStack * stack,GtkWidget * window){
    StackWindow * sw = ling_window_stack_get_win(stack,window);
    if(sw==NULL)return 0;
    if(gtk_widget_get_parent(sw->window)==GTK_WIDGET(stack)){
        gtk_widget_insert_after(sw->window,GTK_WIDGET(stack),
                   gtk_widget_get_last_child(GTK_WIDGET(stack)));
    }
    else gtk_widget_set_parent(sw->window,GTK_WIDGET(stack));

    return 1;
}

//移走顶部窗口,但不在GList中移除
int ling_window_stack_pop(LingWindowStack * stack){
    GtkWidget * win = gtk_widget_get_last_child(GTK_WIDGET(stack));
    if(win==NULL)return 0;
    gtk_widget_unparent(win);
    return 1;
}

void ling_window_stack_set_ani(LingWindowStack * stack,
                               GtkWidget * window,LING_WINDOW_STACK_ANI ani_type){
    StackWindow * sw = ling_window_stack_get_win(stack,window);
    if(sw==NULL)return;
    sw->ani_type = ani_type;
}

void ling_window_stack_push_ani(LingWindowStack * stack,GtkWidget * window){
    //先添加,后动画
    if(ling_window_stack_push(stack,window)){
        ling_operate_emit(stack->ani,LING_ACTION_ANIMATE,NULL,TRUE,EMIT_FINISH_DIR_REVERSE);
    }
}

void ling_window_stack_pop_ani(LingWindowStack * stack){
    //先动画,后移除
    ling_operate_emit_start(stack->ani,LING_ACTION_ANIMATE,NULL,TRUE);
}
