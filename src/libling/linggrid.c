#include "linggrid.h"

enum{
    GRID_FACE_DRAG,
    GRID_FACE_GRID,
    GRID_FACE_FRAME,
};

typedef struct{
    LingOperate * drag_op;
    uint column,row;
    uint w,h;
    //GtkDragSource * drag_source;
}LingGridItem;

struct _LingGrid{
    LingFixed parent;
    uint column,row;
    uint column_space;
    uint row_space;
    uint top_space; //上下空间
    uint start_space; //左右空间
    gdouble aspect_ratio;
    //gdouble dragable;
    GType dnd_type;
    GtkDropTarget * drop_target;
};

G_DEFINE_FINAL_TYPE(LingGrid,ling_grid,LING_TYPE_FIXED)

static void ling_grid_class_init(LingGridClass * klass){

}

static void ling_grid_init(LingGrid * self){
}


GtkWidget * ling_grid_new(uint column,uint row,uint column_space,uint row_space){//,uint start_space,uint top_space){
    LingGrid * self = LING_GRID(g_object_new(LING_TYPE_GRID,NULL));
    self->column = column;
    self->row = row;
    self->column_space = column_space;
    self->row_space = row_space;
    self->start_space = 0;//start_space;
    self->top_space = 0;//top_space;
    return GTK_WIDGET(self);
}

void ling_grid_set_aspect(LingGrid * grid,gdouble ratio){
    grid->aspect_ratio = ratio;
}

GtkAllocation ling_fixed_grid_adjust(GtkWidget * widget,LingFixedItem *info,
                                     int width,int height,int baseline,graphene_rect_t rect){
    LingGrid * grid = LING_GRID(widget);
    LingGridItem * item = info->ex;

    int w=(rect.size.width+grid->column_space-grid->start_space*2)/grid->column;
    int h=(rect.size.height+grid->row_space-grid->top_space*2)/grid->row;
    GtkAllocation alloc;
    alloc.x = w*(item->column-1)+grid->start_space;
    if(grid->aspect_ratio>0){
        alloc.y = (w-grid->column_space+grid->row_space)*grid->aspect_ratio*(item->row-1)+grid->top_space;
    }
    else alloc.y = h*(item->row-1)+grid->top_space;

    alloc.width = w*item->w - grid->column_space;
    if(grid->aspect_ratio>0){
        alloc.height = (w-grid->column_space)*item->h*grid->aspect_ratio;
    }
    else alloc.height = h*item->h - grid->row_space;
    return alloc;
}



static LingFixedItem * ling_grid_get(GList * list,int column,int row){
    for(GList * l = list;l!=NULL;l=l->next){
        LingFixedItem * item =(LingFixedItem *)l->data;
        if(item==NULL)continue;
        LingGridItem * grid_item =(LingGridItem *)item->ex;
        if(grid_item==NULL)continue;
        if(column<grid_item->column+grid_item->w&&
            column>=grid_item->column&&
            row<grid_item->row+grid_item->h&&
            row>=grid_item->row){
            return item;
        }
    }
    return NULL;
}

GtkWidget * ling_grid_get_child_at(LingGrid * grid,int column,int row){
    GList * list = ling_fixed_get_items_list(LING_FIXED(grid));
    LingFixedItem * item=ling_grid_get(list,column,row);
    if(item==NULL)return NULL;
    return item->widget;
}

void ling_grid_query_child(LingGrid *grid,GtkWidget *child,
                           int *column,int *row,int *width,int *height){
    LingFixedItem * item=ling_fixed_get_item_info(LING_FIXED(grid),child);
    if(item==NULL||item->ex==NULL)return;
    LingGridItem * grid_item = (LingGridItem*)item->ex;
    *column = grid_item->column;
    *row = grid_item->row;
    *width = grid_item->w;
    *height = grid_item->h;
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

//static gdouble sx,sy;

// static GdkContentProvider * grid_drag_source_prepare (
//     GtkDragSource* self,gdouble x,gdouble y,gpointer user_data){
//     LingFixedItem * fi = (LingFixedItem *)user_data;
//     LingGridItem * gi = (LingGridItem *)fi->ex;
//     LingGrid * grid = LING_GRID(gtk_widget_get_parent(fi->widget));
//     if(grid==NULL)return NULL;
//     grid->dnd_type = LING_TYPE_GRID;

//     if(grid->dnd_type==0)return NULL;
//     sx=x;
//     sy=y;
//     return gdk_content_provider_new_union((GdkContentProvider *[1]) {
//                                             gdk_content_provider_new_typed (grid->dnd_type, fi),
//                                             }, 1);
// }

// static void grid_drag_source_begin(GtkDragSource* self,GdkDrag* drag,gpointer user_data){
//     g_print("begin\n");
//     LingFixedItem * fi = (LingFixedItem *)user_data;
//     LingGridItem * gi = (LingGridItem *)fi->ex;
//     GdkPaintable *paintable = gtk_widget_paintable_new (fi->widget);
//     gtk_drag_source_set_icon (self, paintable, sx, sy);

// }

// static gboolean grid_drag_source_cancel (
//     GtkDragSource* self,GdkDrag* drag,GdkDragCancelReason* reason,gpointer user_data){

//     return 0;
// }

// static void grid_drag_source_end(GtkDragSource* self,
//                                     GdkDrag* drag,gboolean delete_data,gpointer user_data){

// }

int ling_grid_attach(LingGrid * grid,GtkWidget * widget,uint column,uint row,uint width,uint height){
    LingFixedItem * fi = ling_fixed_put(LING_FIXED(grid),widget,ling_fixed_grid_adjust,0,0,0,GRID_FACE_GRID);
    fi->ex = g_malloc0(sizeof(LingGridItem));
    LingGridItem * gi = fi->ex;
    gi->column = column;
    gi->row = row;
    gi->w = width;
    gi->h = height;
    return 1;
}

void ling_grid_remove_by_pos(LingGrid * grid,int column,int row){
    GList * list = ling_fixed_get_items_list(LING_FIXED(grid));
    LingFixedItem * item=ling_grid_get(list,column,row);
    if(item==NULL)return;
    ling_fixed_remove(LING_FIXED(grid),item->widget);
    g_free(item->ex);
}

void ling_grid_remove(LingGrid * grid,GtkWidget * widget){
    LingFixedItem * item=ling_fixed_get_item_info(LING_FIXED(grid),widget);
    if(item==NULL)return;
    g_free(item->ex);
    ling_fixed_remove(LING_FIXED(grid),item->widget);
}


static gboolean ling_grid_drop_accept(GtkDropTarget* self,GdkDrop* drop,gpointer user_data){
    return TRUE;
}

static gboolean ling_grid_drop_drop(GtkDropTarget* self,const GValue* value,gdouble x,gdouble y,gpointer user_data){
    g_print("drop:%f %f\n",x,y);
    exit(1);
    return TRUE;
}

static GdkDragAction ling_grid_drop_enter(GtkDropTarget* self,gdouble x,gdouble y,gpointer user_data){
    g_print("enter\n");
}

static void ling_grid_drop_leave(GtkDropTarget* self,gpointer user_data){
    g_print("leave\n");
}

static GdkDragAction ling_grid_drop_motion(GtkDropTarget* self,gdouble x,gdouble y,gpointer user_data){
    g_print("motion:%f %f\n",x,y);
    LingGrid * grid = LING_GRID(user_data);
    int c=0,r=0;

    int w = gtk_widget_get_width(GTK_WIDGET(grid));//-ms-me;
    int h = gtk_widget_get_height(GTK_WIDGET(grid));//-mt-mb;
    int iw=(w+grid->column_space)/grid->column-grid->column_space;  //item_width
    int ih=(h+grid->row_space)/grid->row-grid->row_space;

    int s=0,e;
    for(int cc=1;cc<=grid->column;cc++){
        e=iw*cc+grid->column_space;
        if(cc==1)e-=grid->column_space/2;
        if(x>=s&&x<=e){
            c=cc;
            break;
        }
        s=e;
    }

    s=0;
    for(int rr=1;rr<=grid->row;rr++){
        e=ih*rr+grid->row_space;
        if(rr==1)e-=grid->row_space/2;
        if(y>=s&&y<=e){
            r=rr;
            break;
        }
        s=e;
    }
    if(c==0||r==0)return 0;
    ling_grid_add_frame(grid,c,r,1,1);
    return 0;
}

void ling_grid_set_drop_target(LingGrid * grid,GType type,GdkDragAction action){
    grid->drop_target = gtk_drop_target_new(G_TYPE_STRING,action);

    gtk_widget_add_controller(GTK_WIDGET(shell->desktop),GTK_EVENT_CONTROLLER(grid->drop_target));
    //g_signal_connect(grid->drop_target,"accept",G_CALLBACK(ling_grid_drop_accept),grid);
    g_signal_connect(grid->drop_target,"drop",G_CALLBACK(ling_grid_drop_drop),grid);
    //g_signal_connect(grid->drop_target,"enter",G_CALLBACK(ling_grid_drop_enter),grid);
    //g_signal_connect(grid->drop_target,"leave",G_CALLBACK(ling_grid_drop_leave),grid);
    //g_signal_connect(grid->drop_target,"motion",G_CALLBACK(ling_grid_drop_motion),grid);
}

void ling_grid_add_frame(LingGrid * grid,uint column,uint row,uint w,uint h){
    //改变旧的提示框的css样式为渐渐消失
    GList * list = ling_fixed_get_items_list(LING_FIXED(grid));
    for(GList * l = list;l!=NULL;l=l->next){
        LingFixedItem * fi =(LingFixedItem *)list->data;
        if(fi==NULL||fi->level1!=GRID_FACE_FRAME)continue;
        ling_grid_remove(grid,fi->widget);
        //gtk_widget_remove_css_class(fi->widget,"desktop_item_frame_show");
        //gtk_widget_add_css_class(fi->widget,"desktop_item_frame_hide");
    }
    //添加提示框并且加入css渐渐浮现样式
    GtkWidget * frame = gtk_button_new();
    gtk_widget_add_css_class(frame,"desktop_item_frame_show");
    LingFixedItem * fi = ling_fixed_put(LING_FIXED(grid),frame,ling_fixed_grid_adjust,0,0,GRID_FACE_FRAME,0);
    fi->ex = g_malloc0(sizeof(LingGridItem));
    LingGridItem * gi = fi->ex;
    gi->column = column;
    gi->row = row;
    gi->w = w;
    gi->h = h;
}

void ling_grid_remove_all_frame(LingGrid * grid){
    //完全放入后去除所有框
    GList * list = ling_fixed_get_items_list(LING_FIXED(grid));
    for(GList * l = list;l!=NULL;l=l->next){
        LingFixedItem * fi =(LingFixedItem *)list->data;
        if(fi==NULL||fi->level1!=GRID_FACE_FRAME)continue;
        ling_grid_remove(grid,fi->widget);
    }
}

// void ling_grid_set_dragable(LingGrid * grid,gboolean dragable){
//     grid->dragable = dragable;
// }

// gboolean ling_grid_get_dragable(LingGrid * grid){
//     return grid->dragable;
// }

//位置提示框
// void ling_grid_set_(){

// }
