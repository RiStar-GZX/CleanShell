#include "desktopfolder.h"
#include "desktopitem.h"

struct _ClmDesktopFolder{
    LingFixed parent;
    GtkWidget * folder_box;
    GtkWidget * folder;
    LingGrid * grid;
    int c;
    int r;
    int w;
    int h;
    gboolean box_clicked;
};

G_DEFINE_FINAL_TYPE(ClmDesktopFolder,clm_desktop_folder,GTK_TYPE_FIXED)

static void desktop_folder_close_finish(GtkWidget * widget,LingActionArgs args,gpointer user_data);

void clm_desktop_folder_class_init(ClmDesktopFolderClass * klass){

}

static void folder_back_animate(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    g_print("progress:%f\n",args.progress);
    ClmDesktopFolder * self =  CLM_DESKTOP_FOLDER(widget);
    if(self->box_clicked==TRUE)return;
    int  w = 150-(args.progress/100)*50;
    int  h = 150-(args.progress/100)*50;
    clm_desktop_item_folder_set_app_icon_size(CLM_DESKTOP_ITEM(self->folder),52-(args.progress/100)*20);
    clm_desktop_folder_set_size(self,w,h);
}

static void clm_desktop_folder_pressed (GtkGestureClick* gesture,gint n_press,gdouble x,gdouble y,gpointer user_data){
    ClmDesktopFolder * self = CLM_DESKTOP_FOLDER(user_data);
    self->box_clicked = TRUE;
}

void clm_desktop_folder_init(ClmDesktopFolder * self){
    self->folder_box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    ling_fixed_put(LING_FIXED(self),self->folder_box,0,0,LING_FIXED_TOP);
    //gtk_widget_set_size_request(self->folder_box,150,150);

    GtkGesture * gesture = gtk_gesture_click_new();
    gtk_widget_add_controller(self->folder_box,GTK_EVENT_CONTROLLER(gesture));
    g_signal_connect(gesture,"pressed",G_CALLBACK(clm_desktop_folder_pressed),self);

    LingOperate * op = ling_operate_add(shell->controler,"desktop_folder",GTK_WIDGET(self));
    ling_operate_add_action(op,LING_ACTION_ALL,
                            NULL,NULL,
                            folder_back_animate,NULL,
                            NULL,NULL,
                            NULL,desktop_folder_close_finish,self);

    gtk_widget_set_visible(GTK_WIDGET(self),FALSE);
}

GtkWidget * clm_desktop_folder_new(){
    return g_object_new(CLM_TYPE_DESKTOP_FOLDER,NULL);
}

static void desktop_folder_close_finish(GtkWidget * widget,LingActionArgs args,gpointer user_data){
    ClmDesktopFolder * self = CLM_DESKTOP_FOLDER(user_data);
    if(self->box_clicked==TRUE){
        self->box_clicked = FALSE;
        return;
    }
    gtk_box_remove(GTK_BOX(self->folder_box),self->folder);
    ling_grid_attach(LING_GRID(self->grid),self->folder,self->c,self->r,self->w,self->h);
    clm_desktop_item_folder_set_app_runable(CLM_DESKTOP_ITEM(self->folder),FALSE);
    gtk_widget_set_visible(GTK_WIDGET(self),FALSE);
}

void clm_desktop_folder_show(ClmDesktopFolder * self,LingGrid * grid,GtkWidget * folder,uint column,uint row,uint width,uint height){
    self->grid = grid;
    self->c = column;
    self->r = row;
    self->w = width;
    self->h = height;
    self->folder = folder;

    graphene_point_t a,in,out;
    gtk_widget_compute_point(GTK_WIDGET(grid),shell->bodybox,&a,&in);
    g_print("x:%f,y:f\n",in.x,in.y);
    gtk_widget_compute_point(GTK_WIDGET(folder),GTK_WIDGET(grid),&in,&out);
    ling_fixed_move(LING_FIXED(self),self->folder_box,out.x,out.y);


    // gdouble x,y;
    // gtk_widget_translate_coordinates(GTK_WIDGET(folder),GTK_WIDGET(grid),10,10,&x,&y);
    // ling_fixed_move(LING_FIXED(self),self->folder_box,x,y);


    ling_grid_remove(grid,column,row);
    gtk_box_append(GTK_BOX(self->folder_box),folder);
    gtk_widget_set_visible(GTK_WIDGET(self),TRUE);
}


void clm_desktop_folder_set_size(ClmDesktopFolder * self,int w,int h){
    if(w<0)w=self->w;
    if(h<0)h=self->h;
    gtk_widget_set_size_request(self->folder_box,w,h);
}
