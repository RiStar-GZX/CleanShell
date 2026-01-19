#include "linggrid.h"

struct _LingGridPh{
    GtkBox parent;
};

G_DEFINE_FINAL_TYPE(LingGridPh,ling_grid_ph,GTK_TYPE_BOX)

static void ling_grid_ph_class_init(LingGridPhClass * klass){

}

static void ling_grid_ph_init(LingGridPh * self){
    GtkWidget * label = gtk_button_new();
    gtk_widget_set_opacity(label,0);
    gtk_widget_set_hexpand(label,TRUE);
    gtk_widget_set_vexpand(label,TRUE);
    gtk_box_append(GTK_BOX(self),label);
}

GtkWidget * ling_grid_ph_new(){
    return GTK_WIDGET(g_object_new(LING_TYPE_GRID_PH,NULL));
}


struct _LingGrid{
    GtkGrid parent;
    uint column_num;
    uint row_num;
};

G_DEFINE_FINAL_TYPE(LingGrid,ling_grid,GTK_TYPE_GRID)

static void ling_grid_class_init(LingGridClass * klass){

}

static void ling_grid_init(LingGrid * self){
    gtk_grid_set_column_homogeneous(GTK_GRID(self), TRUE); // 列均匀
    gtk_grid_set_row_homogeneous(GTK_GRID(self), TRUE); // 行均匀
}


GtkWidget * ling_grid_new(uint column,uint row,uint column_space,uint row_space){
    LingGrid * self = LING_GRID(g_object_new(LING_TYPE_GRID,NULL));
    //添加占位符
    for(int r=1;r<=row;r++){
        for(int c=1;c<=column;c++){
            GtkWidget * ph = ling_grid_ph_new();
            gtk_grid_attach(GTK_GRID(self),ph,c,r,1,1);
        }
    }
    self->column_num = column;
    self->row_num = row;
    gtk_grid_set_column_spacing(GTK_GRID(self),column_space);
    gtk_grid_set_row_spacing(GTK_GRID(self),row_space);
    return GTK_WIDGET(self);
}

int ling_grid_attach(LingGrid * self,GtkWidget * content,uint column,uint row,uint width,uint height){

    if(column<=0||row<=0)return 0;
    if(column+width-1>self->column_num)return 0;
    if(row+height-1>self->row_num)return 0;

    //检查是否有空间
    int not_ph_num=0;
    for(int r=row;r<row+height;r++){
        for(int c=column;c<column+width;c++){
            GtkWidget * co = gtk_grid_get_child_at(GTK_GRID(self),c,r);
            if(!LING_IS_GRID_PH(co)){
                not_ph_num++;
            }
        }
    }

    if(not_ph_num!=0)return 0;

    //移除占位符
    for(int r=row;r<row+height;r++){
       for(int c=column;c<column+width;c++){
            GtkWidget * co = gtk_grid_get_child_at(GTK_GRID(self),c,r);
            if(LING_IS_GRID_PH(co)){
                gtk_grid_remove(GTK_GRID(self),co);
                g_object_unref(co);
            }
        }
    }

    gtk_grid_attach(GTK_GRID(self),content,column,row,width,height);
    gtk_widget_set_hexpand(content,TRUE);
    gtk_widget_set_vexpand(content,TRUE);
    return 1;
}

GtkWidget * ling_grid_get_child_at(LingGrid * self,int column,int row){
    GtkWidget * child = gtk_grid_get_child_at(GTK_GRID(self),column,row);
    if(LING_IS_GRID_PH(child))return NULL;
    return child;
}

void ling_grid_remove(LingGrid * self,int column,int row){
    GtkWidget * child = ling_grid_get_child_at(self,column,row);
    if(child==NULL)return;
    int c,r,w,h;
    gtk_grid_query_child(GTK_GRID(self),child,&c,&r,&w,&h);

    g_object_ref(child);
    gtk_grid_remove(GTK_GRID(self),child);
    for(int j=r;j<r+h;j++){
        for(int i=c;i<c+w;i++){
            GtkWidget * ph = ling_grid_ph_new();
            gtk_grid_attach(GTK_GRID(self),ph,i,j,1,1);
        }
    }
}

void ling_grid_query_child(LingGrid *self,GtkWidget *child,
                           int *column,int *row,int *width,int *height){
    gtk_grid_query_child(GTK_GRID(self),child,column,row,width,height);
}

// static void grid_data_save(GKeyFile *keyfile,const char * group,LingGrid * grid){
//     int grid_column,grid_row;
//     char page_num[100],grid_c[100],grid_r[100];
//     ling_fixed_view_get_grid_size(grid,&grid_column,&grid_row);
//     sprintf(grid_c,"%d",grid_column);
//     sprintf(grid_r,"%d",grid_row);
//     ling_data_saver_save(DESKTOP_ICON_INI_PATH,keyfile,group,"size",grid_c,grid_r,NULL);

//     //遍历fixed_view下所有成员
//     //for(int r=1;r<grid)
//         uint column,row,width,height;
//         ling_fixed_view_item_get_grid_pos(LING_FIXED_VIEW_ITEM(w),&column,&row);
//         ling_fixed_view_item_get_size(LING_FIXED_VIEW_ITEM(w),&width,&height);

//         //fixed_view_item内容为ling_app_item,获取app_item的信息
//         GtkWidget * app_item=ling_fixed_view_item_get_content(LING_FIXED_VIEW_ITEM(w));
//         app_info * app_info = ling_app_item_get_app_info(LING_APP_ITEM(app_item));

//         //以坐标来命名区别不同图标
//         GString * key_name = g_string_new("");
//         g_string_printf(key_name,"pos%d%d",column,row);
//         char c[100],r[100],w[100],h[100];
//         sprintf(c,"%d",column);
//         sprintf(r,"%d",row);
//         sprintf(w,"%d",width);
//         sprintf(h,"%d",height);
//         ling_data_saver_save(DESKTOP_ICON_INI_PATH,keyfile,group,key_name->str,c,r,w,h,app_info->desktop_path->str,NULL);
//         g_free(key_name);
//     }
// }
