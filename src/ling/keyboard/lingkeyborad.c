#include "lingkeyborad.h"
#include <linggrid.h>

enum {
    CLICKED,
    SIGNAL_NUM
};

static guint signals[SIGNAL_NUM];

struct _LingKeyboard{
    GtkBox parent;
    GtkWidget * fv;
};

struct _LingKeyboardClass{
    GObject parent_class;
    void (*clicked)(LingKeyboard *keyboard,const char * c);
};

G_DEFINE_FINAL_TYPE(LingKeyboard,ling_keyboard,GTK_TYPE_BOX);


void ling_keyboard_class_init(LingKeyboardClass * klass){
    signals[CLICKED] = g_signal_new(
        "clicked",                   // 信号名称
        G_TYPE_FROM_CLASS(klass),      // 发射此信号的类类型
        G_SIGNAL_RUN_FIRST,            // 在其他处理程序之前运行（可选）
        0,                              // 关闭的数据结构（通常为 NULL）
        NULL,                           // 不传递参数给下一个处理者（可选）
        NULL,                           // 不需要参数列表 (可选)
        g_cclosure_marshal_VOID__CHAR, // 指定默认的回调数据转换器 (void -> void)
        G_TYPE_NONE,                   // 返回值类型（这里是 void）
        1,G_TYPE_CHAR
        );
}

void ling_keyboard_init(LingKeyboard * self){

}

typedef struct{
    LingKeyboard * keyboard;
    char c;
}click_args;

static void button_clicked (GtkButton* self,gpointer user_data){
    click_args * arg = (click_args*)user_data;
    g_signal_emit_by_name(arg->keyboard,"clicked",arg->c);
}

static void add_item(LingKeyboard * self,LingGrid * grid,char * label,char out,uint column,uint row){
    GtkWidget * button = gtk_button_new_with_label(label);
    gtk_widget_set_vexpand(button,TRUE);
    gtk_widget_set_hexpand(button,TRUE);
    //GtkWidget * item = ling_fixed_view_item_new(button);
    ling_grid_attach_item(LING_GRID(grid),button,column,row,1,1);
    click_args * arg = malloc(sizeof(click_args));
    arg->c = out;
    arg->keyboard=self;
    g_signal_connect(button,"clicked",G_CALLBACK(button_clicked),arg);
    gtk_widget_add_css_class(GTK_WIDGET(button),"keyboard");
}

static void keyboard_init(LingKeyboard * self){
    self->fv = ling_grid_new(3,4,30,30);
    gtk_box_append(GTK_BOX(self),self->fv);
    gtk_widget_set_vexpand(GTK_WIDGET(self->fv),TRUE);
    gtk_widget_set_hexpand(GTK_WIDGET(self->fv),TRUE);
    add_item(self,LING_GRID(self->fv),"0",'0',2,4);
    add_item(self,LING_GRID(self->fv),"1",'1',1,1);
    add_item(self,LING_GRID(self->fv),"2",'2',2,1);
    add_item(self,LING_GRID(self->fv),"3",'3',3,1);
    add_item(self,LING_GRID(self->fv),"4",'4',1,2);
    add_item(self,LING_GRID(self->fv),"5",'5',2,2);
    add_item(self,LING_GRID(self->fv),"6",'6',3,2);
    add_item(self,LING_GRID(self->fv),"7",'7',1,3);
    add_item(self,LING_GRID(self->fv),"8",'8',2,3);
    add_item(self,LING_GRID(self->fv),"9",'9',3,3);
    add_item(self,LING_GRID(self->fv),"X",'X',1,4);
}

GtkWidget * ling_keyboard_new(){
    LingKeyboard * self = LING_KEYBOARD(g_object_new(LING_TYPE_KEYBOARD,NULL));
    keyboard_init(self);
    return GTK_WIDGET(self);
}
