#include "lingwindowstack.h"

typedef struct{
    GtkWidget * window;
    LING_WINDOW_STACK_ANI ani_type;
    gdouble ani_progress;
}StackWindow;

struct _LingWindowStackLayout{
    GtkLayoutManager parent;
};

G_DEFINE_FINAL_TYPE(LingWindowStackLayout,ling_window_stack_layout,GTK_TYPE_LAYOUT_MANAGER);

struct _LingWindowStack{
    GtkWidget parent;
    GtkLayoutManager * manager;
    GList * windows;
};

G_DEFINE_FINAL_TYPE(LingWindowStack,ling_window_stack,GTK_TYPE_WIDGET);

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

    GtkAllocation child_allocation;
    for(GList * l=stack->windows;l!=NULL;l=l->next){
        StackWindow * win = l->data;
        int rw,rh;
        gtk_widget_get_size_request(win->window,&rw,&rh);

        child_allocation.x = 0;
        child_allocation.y = 0;
        child_allocation.width = width;
        child_allocation.height = height;

        if(win->ani_type==LING_WINDOW_STACK_ANI_LEFT){
            child_allocation.x = child_allocation.width*(win->ani_progress/100-1);
        }
        if(win->ani_type==LING_WINDOW_STACK_ANI_TOP){
            child_allocation.y = child_allocation.height*(win->ani_progress/100-1);
        }
        if(win->ani_type==LING_WINDOW_STACK_ANI_RIGHT){
            child_allocation.x = width-child_allocation.width*(win->ani_progress/100);
        }
        if(win->ani_type==LING_WINDOW_STACK_ANI_BOTTOM){
            child_allocation.y = height-child_allocation.height*(win->ani_progress/100);
        }

        gtk_widget_size_allocate(win->window, &child_allocation, baseline);
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

static void ling_window_stack_init(LingWindowStack * self){
    self->manager = ling_window_stack_layout_new();
    gtk_widget_set_layout_manager(GTK_WIDGET(self),self->manager);
}

void ling_window_stack_add_window(LingWindowStack * stack,GtkWidget * window,uint level){
    if(stack == NULL||window == NULL)return;
    gtk_widget_set_parent(window,GTK_WIDGET(stack));
}

void ling_window_stack_remove_window(LingWindowStack * stack,GtkWidget * window){

}

void ling_window_stack_show(LingWindowStack * stack,GtkWidget * window){

}

void ling_window_stack_push(LingWindowStack * stack,GtkWidget * window){

}

void ling_window_stack_pop(LingWindowStack * stack,GtkWidget * window){

}
