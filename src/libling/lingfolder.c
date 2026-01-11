#include "lingfolder.h"

typedef struct{
    GtkWidget * content;
    FOLDER_OPEN_CB open;
    gpointer open_data;
    FOLDER_ANI_CB ani;
    gpointer ani_data;
    FOLDER_END_CB open_finish;
    gpointer open_finish_data;
    FOLDER_END_CB close_finish;
    gpointer close_finish_data;
}LingFolderItem;

struct _LingFolder{
    LingFixed parent;
    GtkWidget * folder_box;
    gboolean box_clicked;

    GList * content_list;   //LingFolderItem
};

G_DEFINE_FINAL_TYPE(LingFolder,ling_folder,GTK_TYPE_FIXED)

static LingFolderItem * ling_folder_get_item(LingFolder * folder,GtkWidget * widget){
    for(GList * l=folder->content_list;l!=NULL;l=l->next){
        LingFolderItem * item = (LingFolderItem *)l->data;
        if(item->content == widget)return item;
    }
    return NULL;
}

static gboolean folder_open_release(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    LingFolder * folder = LING_FOLDER(user_data);
    LingFolderItem *item = ling_folder_get_item(folder,widget);
    if(item==NULL)return LING_OPERATE_ANIMATION_DIR_BACK;
    double x,y;
    if(item->open(widget,&x,&y,item->open_data)==LING_FOLDER_NOT_OPEN)return LING_OPERATE_ANIMATION_DIR_BACK;
    ling_fixed_move(LING_FIXED(folder),folder->folder_box,x,y);
    gtk_box_append(GTK_BOX(folder->folder_box),widget);
    gtk_widget_set_visible(GTK_WIDGET(folder),TRUE);
    return LING_OPERATE_ANIMATION_DIR_FORWARD;
}

static void folder_open_animate(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    LingFolder * folder = LING_FOLDER(user_data);
    LingFolderItem *item = ling_folder_get_item(folder,widget);
    if(item==NULL)return;
    item->ani(widget,args.progress,item->ani_data);
}

static void folder_open_finish(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    LingFolder * folder = LING_FOLDER(user_data);
    LingFolderItem *item = ling_folder_get_item(folder,widget);
    if(item==NULL)return;
    item->open_finish(widget,item->open_finish_data);
}

static void folder_close_animate(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    LingFolder * folder =  LING_FOLDER(widget);
    if(folder->box_clicked==TRUE)return;
    GtkWidget * content = gtk_widget_get_first_child(folder->folder_box);
    LingFolderItem *item = ling_folder_get_item(folder,content);
    if(item==NULL)return;
    if(item->ani)item->ani(content,100-args.progress,item->ani_data);
}

static void folder_close_finish(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    LingFolder * folder = LING_FOLDER(user_data);
    if(folder->box_clicked==TRUE){
        folder->box_clicked = FALSE;
        return;
    }
    GtkWidget * content = gtk_widget_get_first_child(folder->folder_box);
    LingFolderItem *item = ling_folder_get_item(folder,content);
    if(item==NULL)return;
    gtk_box_remove(GTK_BOX(folder->folder_box),content);
    if(item->close_finish)item->close_finish(content,item->close_finish_data);
    gtk_widget_set_visible(GTK_WIDGET(folder),FALSE);
}

void ling_folder_class_init(LingFolderClass * klass){

}

static void folder_pressed_blank (GtkGestureClick* gesture,gint n_press,gdouble x,gdouble y,gpointer user_data){
    LingFolder * folder = LING_FOLDER(user_data);
    folder->box_clicked = TRUE;
}

void ling_folder_init(LingFolder * self){
    self->folder_box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    ling_fixed_put(LING_FIXED(self),self->folder_box,0,0,LING_FIXED_TOP);

    GtkGesture * gesture = gtk_gesture_click_new();
    gtk_widget_add_controller(self->folder_box,GTK_EVENT_CONTROLLER(gesture));
    g_signal_connect(gesture,"pressed",G_CALLBACK(folder_pressed_blank),self);

    LingOperate * op = ling_operate_add(shell->controler,"desktop_folder_close",GTK_WIDGET(self));
    ling_operate_add_action(op,LING_ACTION_ALL,
                            NULL,NULL,
                            folder_close_animate,self,
                            NULL,NULL,
                            NULL,folder_close_finish,self);

    gtk_widget_set_visible(GTK_WIDGET(self),FALSE);
}

GtkWidget * ling_folder_new(){
    return g_object_new(LING_TYPE_FOLDER,NULL);
}

void ling_folder_set_pos(LingFolder * self,int x,int y){
    ling_fixed_move(LING_FIXED(self),self->folder_box,x,y);
}

LingOperate * ling_folder_operate(LingFolder * self,GtkWidget * widget,uint action_type,
                                  FOLDER_OPEN_CB open,gpointer open_data,
                                  FOLDER_ANI_CB ani,gpointer ani_data,
                                  FOLDER_END_CB open_finish,gpointer open_finish_data,
                                  FOLDER_END_CB close_finish,gpointer close_finish_data){
    LingFolderItem * item = g_malloc0(sizeof(LingFolderItem));
    item->open = open;
    item->open_data = open_data;
    item->ani = ani;
    item->ani_data = ani_data;
    item->open_finish = open_finish;
    item->open_finish_data = open_finish_data;
    item->close_finish = close_finish;
    item->close_finish_data = close_finish_data;
    item->content = widget;
    self->content_list = g_list_append(self->content_list,item);
    LingOperate * op = ling_operate_add(shell->controler,"desktop_folder_open",widget);
    ling_operate_add_action(op,action_type,
                            NULL,NULL,
                            folder_open_animate,self,
                            folder_open_release,self,
                            NULL,folder_open_finish,self);
    return op;
}
