#include <lingfixedview.h>

/******************** LingFixedViewItem ********************/

struct _LingFixedViewItem{
    GtkBox parent;
    GtkWidget * content;
    LingFixedView * fixed_view;//所属的fixed_view

    LingFixedView * origin_fixed_view;//被移除后放在无效位置自动回到原来的fixed_view
    //const char * op_namespace;

    gdouble x;
    gdouble y;
    uint column;    //0时候表示不使用对齐
    uint row;       //0时候表示不使用对齐
    uint width;
    uint height;

    ITEM_DRAG_CALLBACK drag_start;
    ITEM_DROP_CALLBACK drag_end;
    gpointer drag_data;

    gboolean remove_on_drag;   //拖动删除原来图标

    //复制到别的位置时候复制自己
    CONTENT_COPY content_copy;

    //短按松开瞬间触发
    PRESS_CALLBACK click;
    gpointer click_data;

    //长按触发
    ITEM_DRAG_CALLBACK long_press_start;
    ITEM_DROP_CALLBACK long_press_end;
    gpointer long_press_data;

    GtkGesture * drag;
    GtkDragSource * drag_source;
    gdouble start_x;
    gdouble start_y;
};


G_DEFINE_FINAL_TYPE(LingFixedViewItem,ling_fixed_view_item,GTK_TYPE_BOX)

static void ling_fixed_view_item_class_init(LingFixedViewItemClass * klass){

}

static void ling_fixed_view_item_init(LingFixedViewItem * self){

}


GtkWidget * ling_fixed_view_item_new(GtkWidget * content){
    LingFixedViewItem * self = LING_FIXED_VIEW_ITEM(g_object_new(LING_TYPE_FIXED_VIEW_ITEM,NULL));
    self->content = content;
    gtk_box_append(GTK_BOX(self),self->content);
    return GTK_WIDGET(self);
}

void ling_fixed_view_item_set_middle_press_cb(LingFixedViewItem * self,ITEM_DRAG_CALLBACK start,ITEM_DROP_CALLBACK end,gpointer data){
    self->drag_start = start;
    self->drag_end = end;
    self->drag_data = data;
}

void ling_fixed_view_item_set_long_press_cb(LingFixedViewItem * self,ITEM_DRAG_CALLBACK start,ITEM_DROP_CALLBACK end,gpointer data){
    self->long_press_start = start;
    self->long_press_end = end;
    self->long_press_data = data;
}

void ling_fixed_view_item_set_remove_on_drag(LingFixedViewItem * self,gboolean remove_on_drag){
    self->remove_on_drag = remove_on_drag;
}

void ling_fixed_view_item_set_content_copy_func(LingFixedViewItem * self,CONTENT_COPY func){
    self->content_copy = func;
}

/******************** LingFixedView ********************/

enum{
    ISON_NONE = 0,
    ISON_DRAG,
    ISON_NO_DRAG_MOVE,
    ISON_LONG_PRESS
};

struct _LingFixedView{
    GtkBox parent;
    GtkWidget * fixed;

    //LingOpControler * controler;
    LingOperate * chain_move_op;

    gpointer item_drop_data;    //传入ITEM_DROP_CALLBACK

    gboolean is_moving; //停止state_flags_changed（临时方案）
    //排布
    int arrange;
    int column_num;
    int row_num;
    int column_space;
    int row_space;

    int margin_top;
    int margin_bottom;
    int margin_start;
    int margin_end;


    int width;
    int height;
    int item_width;
    int item_height;


    //
    int ison;


    int middle_press_timer_id;
    uint middle_press_time; //默认500

    int long_press_timer_id;
    uint long_press_time; //默认1000

    int chain_move_timer_id;
    //droptarget现在的位置
    gdouble now_x;
    gdouble now_y;
};


G_DEFINE_FINAL_TYPE(LingFixedView,ling_fixed_view,GTK_TYPE_BOX)

int ling_fixed_view_get_available_grid_pos(LingFixedView * fv,gdouble x,gdouble y,uint width,uint height,uint * column,uint * row);

GtkWidget * ling_fixed_view_item_get_content(LingFixedViewItem * item){
    return item->content;
}

void ling_fixed_view_item_get_size(LingFixedViewItem * item,uint * width,uint * height){
    *width=item->width;
    *height=item->height;
}

int ling_fixed_view_item_set_grid_pos(LingFixedViewItem * item,uint column,uint row){
    LingFixedView * fv = item->fixed_view;
    if(fv==NULL)return 0;
    if(column>fv->column_num||row>fv->row_num)return 0;

    GtkWidget * i=ling_fixed_view_get_item_by_grid_pos(fv,column,row);
    if(i!=NULL&&i!=GTK_WIDGET(item)){
        return 0;
    }

    double x=(column-1)*fv->column_space+(column-1)*fv->item_width+fv->margin_start;
    double y=(row-1)*fv->row_space+(row-1)*fv->item_height+fv->margin_top;
    //g_print("x:%f y:%f",x,y);
    item->column = column;
    item->row = row;
    ling_fixed_view_item_set_free_pos(item,x,y);
    return 1;
}

int ling_fixed_view_item_get_grid_pos(LingFixedViewItem * item,uint * column,uint * row){
    LingFixedView * fv = item->fixed_view;
    if(fv==NULL)return 0;
    if(fv->arrange==LING_FIXED_VIEW_ARRANGE_FREE)return 0;

    *column=item->column;
    *row=item->row;
    return 1;
}

int ling_fixed_view_item_set_free_pos(LingFixedViewItem * item,gdouble x,gdouble y){
    //内部移动函数
    LingFixedView * fv = item->fixed_view;
    if(fv==NULL)return 0;
    if(x>fv->width||x<0||y>fv->height||y<0)return 0;
    gtk_fixed_move(GTK_FIXED(item->fixed_view->fixed),GTK_WIDGET(item),x,y);
    item->x = x;
    item->y = y;
    return 1;
}

void ling_fixed_view_item_get_free_pos(LingFixedViewItem * item,gdouble * x,gdouble * y){
    *x = item->x;
    *y = item->y;
}

//通过位置获取item

GtkWidget * ling_fixed_view_get_item_by_grid_pos(LingFixedView * self,uint column,uint row){
    if(self->arrange!=LING_FIXED_VIEW_ARRANGE_GRID)return NULL;
    for(GtkWidget * i=gtk_widget_get_first_child(GTK_WIDGET(self->fixed));i!=NULL;i=gtk_widget_get_next_sibling(i)){
        LingFixedViewItem * item = LING_FIXED_VIEW_ITEM(i);
        for(int c=item->column;c<item->column+item->width;c++){
            for(int r=item->row;r<item->row+item->height;r++){
                if(column==c&&row==r)return i;
            }
        }
        //if(column==item->column&&row==item->row)return i;
    }
    return NULL;
}


//吸附函数
int ling_fixed_view_item_adsorb_to_grid(LingFixedViewItem * item){
    LingFixedView * fv = item->fixed_view;
    if(fv==NULL)return 0;
    if(fv->arrange!=LING_FIXED_VIEW_ARRANGE_GRID)return 0;
    if(item->column>0&&item->row>0){
        ling_fixed_view_item_set_grid_pos(item,item->column,item->row);
        return 1;
    }
    return 0;
}


//void ling_fixed_view_set_middle_press(LingFixedView * fv,){

//}

void ling_fixed_view_item_set_drag_source_content(LingFixedViewItem * item){
    //LingFixedViewItem * source = item;
    //if(item->content_copy!=NULL)source = LING_FIXED_VIEW_ITEM(item->content_copy(item->content));
    gtk_drag_source_set_content(item->drag_source,gdk_content_provider_new_typed(LING_TYPE_FIXED_VIEW_ITEM,item));
}

static gboolean middle_timeout(gpointer data){
    LingFixedViewItem * item = data;
    LingFixedView * fv = item->fixed_view;
    if(fv==NULL)return G_SOURCE_REMOVE;
    fv->ison = ISON_DRAG;
    if(item->drag_start!=NULL)item->drag_start(item,item->drag_data);
    fv->middle_press_timer_id=0;

    //gtk_drag_source_set_content(item->drag_source,gdk_content_provider_new_typed(LING_TYPE_FIXED_VIEW_ITEM,item));
    //gtk_drag_source_set_content(item->drag_source,gdk_content_provider_new_typed(G_TYPE_POINTER,item));
    return G_SOURCE_REMOVE;
}

static gboolean long_press_timeout(gpointer data){
    LingFixedViewItem * item = data;
    LingFixedView * fv = item->fixed_view;
    if(fv==NULL)return G_SOURCE_REMOVE;
    fv->ison = ISON_LONG_PRESS;
    if(item->long_press_start!=NULL)item->long_press_start(item,item->long_press_data);
    fv->long_press_timer_id=0;
    return G_SOURCE_REMOVE;
}

static void on_drag_begin(GtkGestureDrag* drag,gdouble start_x,gdouble start_y,LingFixedViewItem * item){
    item->start_x=start_x;
    item->start_y=start_y;
    LingFixedView * fv=item->fixed_view;
    if(fv==NULL)return;
    fv->ison=ISON_NONE;
    fv->middle_press_timer_id = g_timeout_add(300,middle_timeout,item);
    fv->long_press_timer_id =  g_timeout_add(1000,long_press_timeout,item);
}

static void on_drag_update (GtkGestureDrag* drag,gdouble offset_x,gdouble offset_y,LingFixedViewItem * item){
    LingFixedView * fv=item->fixed_view;
    if(fv==NULL)return;

    //g_print("offset:%f %f\n",offset_x,offset_y);
    //if(fv->ison!=ISON_NONE);
    //else
    if(fv->ison==ISON_NONE/*&&fabs(offset_x)<10&&fabs(offset_)<10*/){
        //dragtime没到就乱动
        fv->ison=ISON_NO_DRAG_MOVE;
        if(fv->middle_press_timer_id!=0){
            g_source_remove(fv->middle_press_timer_id);
            fv->middle_press_timer_id=0;
        }
        if(fv->long_press_timer_id!=0){
            g_source_remove(fv->long_press_timer_id);
            fv->long_press_timer_id=0;
        }
    }else if(fv->ison==ISON_DRAG){
        //拖动
        if(fv->long_press_timer_id!=0){
            g_source_remove(fv->long_press_timer_id);
            fv->long_press_timer_id=0;
        }
    }else if(fv->ison==ISON_LONG_PRESS){
        if(fabs(item->x+offset_x)>10||fabs(item->y+offset_y)>10){
            //长按中拖动
            //if(item->long_press_end!=NULL)item->long_press_end(item,item->long_press_data,);
            fv->ison=ISON_DRAG;
        }
    }

    //    if(fv->ison==ISON_DRAG||fv->ison==ISON_LONG_PRESS){
    //        ling_fixed_view_item_set_free_pos(item,item->x+offset_x,item->y+offset_y);
    //    }
}

static void on_drag_end (GtkGestureDrag* drag,gdouble offset_x,gdouble offset_y,LingFixedViewItem * item){
    LingFixedView * fv=item->fixed_view;
    if(fv==NULL)return;
    fv->ison=ISON_NO_DRAG_MOVE;
    if(fv->middle_press_timer_id!=0){
        g_source_remove(fv->middle_press_timer_id);
        fv->middle_press_timer_id=0;
    }
    if(fv->long_press_timer_id!=0){
        g_source_remove(fv->long_press_timer_id);
        fv->long_press_timer_id=0;
    }
    //if(item->drag_end!=NULL)item->drag_end(item,item->drag_data,item->fixed_view->item_drop_data);
    if(item->fixed_view->arrange==LING_FIXED_VIEW_ARRANGE_GRID){
        ling_fixed_view_item_adsorb_to_grid(item);
    }
}

//static gdouble sx,sy;

//static GdkContentProvider *
//on_drag_source_prepare (GtkDragSource *source,
//                 double         x,
//                 double         y,
//                 LingFixedViewItem * item)
//{
//    sx=x;
//    sy=y;
//    g_print("sx:%f sy:%f\n",x,y);
//    return NULL;
//}drag_source

//GdkPaintable* copy_paintable(GdkPaintable* original) {
//    if (GDK_IS_TEXTURE(original)) {
//        GdkPixbuf *pixbuf = gdk_pixbuf_get_from_texture(GDK_TEXTURE(original));
//        return GDK_PAINTABLE(gdk_texture_new_for_pixbuf(pixbuf));
//    }
//    return NULL;
//}

//GdkPaintable* copy_paintable(GdkPaintable* original, int width, int height) {
//    // 创建ARGB表面
//    cairo_surface_t* surface = cairo_image_surface_create(
//        CAIRO_FORMAT_ARGB32, width, height);
//    cairo_t* cr = cairo_create(surface);

//    // 渲染原始内容
//    gdk_paintable_snapshot(original,
//        gtk_snapshot_new(),
//        width, height);
//    cairo_destroy(cr);

//    // 转换为纹理
//    GdkTexture* texture = gdk_texture_new_for_surface(surface);
//    cairo_surface_destroy(surface);
//    return GDK_PAINTABLE(texture);
//}


static void on_drag_source_begin (GtkDragSource *source,
                                 GdkDrag       *drag,
                                 LingFixedViewItem      *item){
    g_print("Set the widget as the drag icon\n");
    //    GIcon * gicon = g_icon_new_for_string("firefox",NULL);
    //    GtkIconTheme *icon_theme = gtk_icon_theme_get_for_display(gdk_display_get_default());
    //    GdkPaintable *paintable =  GDK_PAINTABLE(gtk_icon_theme_lookup_by_gicon(icon_theme,gicon,64,1,
    //                                                                    GTK_TEXT_DIR_NONE,0));
    GdkPaintable *old = gtk_widget_paintable_new(GTK_WIDGET(item));
    //GdkPaintable *paintable = copy_paintable(old);
    GtkWidget * paintable_source = GTK_WIDGET(item);
    GdkPaintable *paintable = NULL;//gtk_widget_paintable_new(GTK_WIDGET(paintable_source));
    //    copy_paintable(paintable,gtk_widget_get_width(GTK_WIDGET(item)),gtk_widget_get_height(GTK_WIDGET(item)));
    //    if(item->content_copy){
    //        paintable_source = item->content_copy(item->content);
    //    }
    //paintable_source = gtk_image_new_from_icon_name("firefox");
    //gtk_image_set_pixel_size(GTK_IMAGE(paintable_source),64);
    //paintable_source = gtk_button_new();
    gtk_drag_source_set_icon (source, paintable, item->start_x, item->start_y);
    //g_print("x:%f y:%f\n",item->start_x,item->start_y);
    if(item->remove_on_drag)ling_fixed_view_remove_item(item->fixed_view,item);
    //gtk_widget_set_visible(GTK_WIDGET(item),FALSE);
    //g_print("ds_begin\n");
}

static void on_drag_source_end (GtkDragSource* source,GdkDrag* drag,
                               gboolean delete_data,LingFixedViewItem      *item){
    gtk_widget_set_visible(GTK_WIDGET(item),TRUE);
    if(!delete_data){
        g_print("delete_data\n");
        uint column,row;
        //gdk_drag_get_
        double x=item->x,y=item->y;
        if(ling_fixed_view_get_available_grid_pos(item->origin_fixed_view,x,y,item->width,item->height,&column,&row)){
            //ling_fixed_view_add_grid(item->origin_fixed_view,LING_FIXED_VIEW_ITEM(item),item->width,item->height,column,row);
            ling_fixed_view_add_item_free(item->origin_fixed_view,LING_FIXED_VIEW_ITEM(item),item->width,item->height,x,y);
            LING_FIXED_VIEW_ITEM(item)->column = column;
            LING_FIXED_VIEW_ITEM(item)->row = row;
            if(ling_operate_start_operating(item->origin_fixed_view->chain_move_op)){
                ling_operate_run_animation(item->origin_fixed_view->chain_move_op);
            }
        }
    }
    //gtk_drag_source_set_content(source,NULL);
    //g_print("ds_end\n");
}

//添加item函数
int ling_fixed_view_add_item_free(LingFixedView * fixed_view,LingFixedViewItem * item,uint width,uint height,gdouble x,gdouble y){
    if(fixed_view==NULL||item==NULL)return 0;
    if(x>fixed_view->width||x<0||y>fixed_view->height||y<0)return 0;
    item->fixed_view = fixed_view;
    item->width=width;
    item->height=height;
    item->x=x;
    item->y=y;
    gtk_fixed_put(GTK_FIXED(fixed_view->fixed),GTK_WIDGET(item),x,y);

    GtkGesture * drag = gtk_gesture_drag_new();
    g_signal_connect(drag, "drag-begin", G_CALLBACK(on_drag_begin), item);
    g_signal_connect(drag, "drag-update", G_CALLBACK(on_drag_update), item);
    g_signal_connect(drag, "drag-end", G_CALLBACK(on_drag_end), item);
    gtk_widget_add_controller(GTK_WIDGET(item), GTK_EVENT_CONTROLLER(drag));
    item->drag = drag;

    GtkDragSource * drag_source = gtk_drag_source_new ();
    gtk_drag_source_set_actions(drag_source, GDK_ACTION_MOVE);
    //g_signal_connect (drag_source, "prepare", G_CALLBACK (on_drag_source_prepare), item);
    g_signal_connect (drag_source, "drag-begin", G_CALLBACK (on_drag_source_begin), item);
    g_signal_connect (drag_source, "drag-end", G_CALLBACK (on_drag_source_end), item);
    gtk_widget_add_controller (GTK_WIDGET (item), GTK_EVENT_CONTROLLER (drag_source));
    item->drag_source = drag_source;

    return 1;
}

int ling_fixed_view_add_grid(LingFixedView * fixed_view,LingFixedViewItem * item,uint width,uint height,uint column,uint row){
    if(fixed_view==NULL||item==NULL)return 0;
    if(ling_fixed_view_get_item_by_grid_pos(fixed_view,column,row)!=NULL)return 0;

    item->fixed_view = fixed_view;
    item->width=width;
    item->height=height;
    gtk_fixed_put(GTK_FIXED(fixed_view->fixed),GTK_WIDGET(item),0,0);

    GtkGesture * drag = gtk_gesture_drag_new();
    g_signal_connect(drag, "drag-begin", G_CALLBACK(on_drag_begin), item);
    g_signal_connect(drag, "drag-update", G_CALLBACK(on_drag_update), item);
    g_signal_connect(drag, "drag-end", G_CALLBACK(on_drag_end), item);
    gtk_widget_add_controller(GTK_WIDGET(item), GTK_EVENT_CONTROLLER(drag));
    item->drag = drag;

    GtkDragSource * drag_source = gtk_drag_source_new ();
    gtk_drag_source_set_actions(drag_source, GDK_ACTION_MOVE);
    //g_signal_connect (drag_source, "prepare", G_CALLBACK (on_drag_source_prepare), item);
    g_signal_connect (drag_source, "drag-begin", G_CALLBACK (on_drag_source_begin), item);
    g_signal_connect (drag_source, "drag-end", G_CALLBACK (on_drag_source_end), item);
    gtk_widget_add_controller (GTK_WIDGET (item), GTK_EVENT_CONTROLLER (drag_source));
    item->drag_source = drag_source;

    return ling_fixed_view_item_set_grid_pos(item,column,row);
}

GtkWidget * ling_fixed_view_add_content_grid(LingFixedView * fixed_view,GtkWidget * content,
                                            uint width,uint height,uint column,uint row){
    GtkWidget * item = ling_fixed_view_item_new(content);
    if(ling_fixed_view_add_grid(fixed_view,LING_FIXED_VIEW_ITEM(item),width,height,column,row)<=0)return NULL;
    return item;
}

void ling_fixed_view_remove_item(LingFixedView * fv,LingFixedViewItem * item){
    g_object_ref(item);
    gtk_fixed_remove(GTK_FIXED(fv->fixed),GTK_WIDGET(item));
    item->x=0;
    item->y=0;
    item->column=0;
    item->row=0;
    item->origin_fixed_view = item->fixed_view;
    item->fixed_view=NULL;
    gtk_widget_remove_controller(GTK_WIDGET(item),GTK_EVENT_CONTROLLER(item->drag));
    gtk_widget_remove_controller(GTK_WIDGET(item),GTK_EVENT_CONTROLLER(item->drag_source));
    g_object_ref(item->drag);
    g_object_ref(item->drag_source);
    item->drag = NULL;
    item->drag_source = NULL;
}

void ling_fixed_view_update(LingFixedView * self){
    //更新布局信息
    self->width = gtk_widget_get_width(GTK_WIDGET(self));
    self->height = gtk_widget_get_height(GTK_WIDGET(self));
    self->item_width =(self->width-(self->column_num-1)*self->column_space-self->margin_start-self->margin_end)/self->column_num;
    self->item_height =(self->height-(self->row_num-1)*self->row_space-self->margin_top-self->margin_bottom)/self->row_num;

    //g_print("end:%d %d\n",self->item_width,self->item_height);

    //更新item布局
    for(GtkWidget * c=gtk_widget_get_first_child(GTK_WIDGET(self->fixed));c!=NULL;c=gtk_widget_get_next_sibling(c)){
        LingFixedViewItem * item = LING_FIXED_VIEW_ITEM(c);
        //if(item->width==2)g_print("w:%d,h:%d",self->item_width*item->width,self->item_height*item->height);
        int w=self->item_width*item->width+(item->width-1)*self->column_space;
        int h=self->item_height*item->height+(item->height-1)*self->row_space;
        if(w>0&&h>0)gtk_widget_set_size_request(c,w,h);
        if(self->arrange==LING_FIXED_VIEW_ARRANGE_GRID){
            //全局网格对齐
            //ling_fixed_view_item_adsorb_to_grid(item);
            if(item->column!=0&&item->row!=0)
                ling_fixed_view_item_set_grid_pos(item,item->column,item->row);
        }
    }
}

void ling_fixed_view_set_arrange_grid(LingFixedView * self){
    self->arrange = LING_FIXED_VIEW_ARRANGE_GRID;
    ling_fixed_view_update(self);   //触发全局网格对齐
}

void ling_fixed_view_set_arrange_free(LingFixedView * self){
    self->arrange=LING_FIXED_VIEW_ARRANGE_FREE;
}

int ling_fixed_view_set_grid_info(LingFixedView * self,int arrange,uint column_num,uint row_num,uint column_space,uint row_space){
    ///设置网格信息
    if(column_num<=0||row_num<=0)return 0;
    if(arrange>=0)self->arrange = arrange;//arrange小于0时保持原样
    self->column_num = column_num;
    self->row_num = row_num;
    self->column_space = column_space;
    self->row_space = row_space;

    ling_fixed_view_update(self);
    return 1;
}

int ling_fixed_view_get_arrange(LingFixedView * self){
    return self->arrange;
}

void ling_fixed_view_get_grid_pos(LingFixedView * fv,gdouble x,gdouble y,uint * column,uint * row){
    ////输入一个自由坐标,返回其所在的网格
    uint cc=0,rr=0;
    for(int c=1;c<=fv->column_num;c++){
        gdouble sx,ex;
        sx=(c-1)*(fv->column_space+fv->item_width);//+fv->margin_start;
        ex=(c)*(fv->column_space+fv->item_width);//+fv->margin_start;
        if(x>=sx&&x<ex){
            cc=c;
            break;
        }
    }

    for(int r=1;r<=fv->row_num;r++){
        gdouble sy,ey;
        sy=(r-1)*(fv->row_space+fv->item_height);//fv->margin_top;
        ey=(r)*(fv->row_space+fv->item_height);//+fv->margin_top;
        if(y>=sy&&y<ey){
            rr=r;
            break;
        }
    }

    if(cc<=0){
        if((fv->column_num)*(fv->column_space+fv->item_width)){
            cc=fv->column_num;
        }
        else cc=1;
    }
    if(rr<=0){
        if((fv->row_num)*(fv->row_space+fv->item_height)){
            rr=fv->row_num;
        }
        else rr=1;
    }
    *column=cc;
    *row=rr;
    //g_print("cc:%d rr:%d\n",cc,rr);
}

gboolean ling_fixed_view_get_rectangle_available(LingFixedView * fv,uint width,uint height,uint column,uint row){
    gboolean ret=1;
    for(int r=row;r<row+height;r++){
        for(int c=column;c<column+width;c++){
            if(c>fv->column_num||r>fv->row_num)return 0;
            if(ling_fixed_view_get_item_by_grid_pos(fv,c,r)!=NULL)ret=0;
        }
    }
    return ret;
}

int ling_fixed_view_get_available_grid_pos(LingFixedView * fv,gdouble x,gdouble y,uint width,uint height,uint * column,uint * row){
    ////输入一个自由坐标,返回其最近的可用的网格
    uint cc=0,rr=0;
    ling_fixed_view_get_grid_pos(fv,x,y,&cc,&rr);

    gboolean ret =ling_fixed_view_get_rectangle_available(fv,width,height,cc,rr);
    if(ret==1){
        *column=cc;
        *row=rr;
        return 1;
    }


    double distance=-1;
    int ava_c=0,ava_r=0;
    for(int r=1;r<=fv->row_num;r++){
        for(int c=1;c<=fv->column_num;c++){
            ret=ling_fixed_view_get_rectangle_available(fv,width,height,c,r);//ling_fixed_view_get_item_by_grid_pos(fv,c,r);
            if(ret==0)continue;
            double new_distance=abs(c-(int)cc)*abs(c-(int)cc)+abs(r-(int)rr)*abs(r-(int)rr);
            if(distance<0||new_distance<distance){
                ava_c=c;
                ava_r=r;
                distance = new_distance;
            }
        }
    }

    if(ava_c==0||ava_r==0)return 0;

    *column=ava_c;
    *row=ava_r;

    g_print("c:%d %d r:%d %d\n",ava_c,cc,ava_r,rr);
    return 1;
}

///--------------链式移动--------------///

int ling_fixed_view_chain_move_left(LingFixedView * fv,uint column,uint row){
    int free_r=0,free_c=0;  //空位的网格坐标
    int r,c=column;
    for(r=row;r<=fv->row_num;r++){
        for(;c<=fv->column_num;c++){
            GtkWidget * item = ling_fixed_view_get_item_by_grid_pos(fv,c,r);
            if(item==NULL){
                free_r=r;
                free_c=c;
                goto a;
            }
        }
        c=1;
    }

    return 0;   //链中没有空位
a:

    c=free_c;
    for(r=free_r;r>=row;r--){
        int c_end;
        if(r==row)c_end=column;
        else c_end=1;
        for(;c>=c_end;c--){
            uint item_c,item_r;
            if(c==1){
                item_c=fv->column_num;
                item_r=r-1;
            }else{
                item_c=c-1;
                item_r=r;
            }
            if(c==column&&r==row)continue;
            GtkWidget * item = ling_fixed_view_get_item_by_grid_pos(fv,item_c,item_r);
            if(item==NULL)continue;
            LING_FIXED_VIEW_ITEM(item)->column = c;
            LING_FIXED_VIEW_ITEM(item)->row = r;
            //ling_fixed_view_item_set_grid_pos(LING_FIXED_VIEW_ITEM(item),c,r);
        }
        c=fv->column_num;
    }
    return 1;
}

int ling_fixed_view_chain_move_right(LingFixedView * fv,uint column,uint row){
    int free_r=0,free_c=0;  //空位的网格坐标
    int r,c=column;
    for(r=row;r>=1;r--){
        for(;c>=1;c--){
            GtkWidget * item = ling_fixed_view_get_item_by_grid_pos(fv,c,r);
            if(item==NULL){
                free_r=r;
                free_c=c;
                goto a;
            }
        }
        c=fv->column_num;
    }

    return 0;   //链中没有空位
a:

    c=free_c;
    for(r=free_r;r<=row;r++){
        int c_end;
        if(r==row)c_end=column;
        else c_end=fv->column_num;
        for(;c<=c_end;c++){
            uint item_c,item_r;
            if(c==fv->column_num){
                item_c=1;
                item_r=r+1;
            }else{
                item_c=c+1;
                item_r=r;
            }
            if(c==column&&r==row)continue;
            GtkWidget * item = ling_fixed_view_get_item_by_grid_pos(fv,item_c,item_r);
            if(item==NULL)continue;
            LING_FIXED_VIEW_ITEM(item)->column = c;
            LING_FIXED_VIEW_ITEM(item)->row = r;
            //ling_fixed_view_item_set_grid_pos(LING_FIXED_VIEW_ITEM(item),c,r);
        }
        c=1;
    }
    return 1;
}

static gboolean chain_move_animate(gdouble velocity_x,gdouble velocity_y,gpointer user_data){
    LingFixedView * fv = user_data;
    int num=0;
    for(GtkWidget * w=gtk_widget_get_first_child(fv->fixed);w!=NULL;w=gtk_widget_get_next_sibling(w)){
        LingFixedViewItem * item = LING_FIXED_VIEW_ITEM(w);
        double x=(item->column-1)*fv->column_space+(item->column-1)*fv->item_width+fv->margin_start;
        double y=(item->row-1)*fv->row_space+(item->row-1)*fv->item_height+fv->margin_top;
        if(item->x!=x||item->y!=y){
            double new_x=item->x,new_y=item->y;
            int base_speed=1;
            if(fabs(item->x-x)<=base_speed)new_x=x;
            else{
                new_x+=(x-item->x)/10;
                if(fabs(new_x-x)>fabs(item->x-x))new_x=x;
            }
            if(fabs(item->y-y)<=base_speed)new_y=y;
            else{
                new_y+=(y-item->y)/10;
                if(fabs(new_y-y)>fabs(item->y-y))new_y=y;
            }
            ling_fixed_view_item_set_free_pos(item,new_x,new_y);
            num++;
            //g_print("newxy:%d %d\n",item->x,item->y);
        }
    }
    if(num==0){
        //fv->is_moving=0;
        return LING_OPERATE_ANIMATION_REMOVE;
    }
    fv->is_moving=1;
    return LING_OPERATE_ANIMATION_CONTINUE;
}

static void chain_move_end(gpointer user_data){
    if(user_data==NULL)return;
    LingFixedViewItem * item = user_data;
}


int ling_fixed_view_chain_move(LingFixedView * fv,gdouble x,gdouble y){
    ///停在边缘图标链式移动
    uint column=0,row=0;
    ling_fixed_view_get_grid_pos(fv,x,y,&column,&row);
    if(column==0||row==0)return 0;

    if(ling_fixed_view_get_item_by_grid_pos(fv,column,row)==NULL)return 2;

    LingOperate * op = fv->chain_move_op;

    //    chain_move_user_data * data=malloc(sizeof(chain_move_user_data));
    //    data->fv = fv;
    //    data->column=column;
    //    data->row=row;

    gdouble relative_x=x-(column-1)*(fv->item_width+fv->column_space);
    //gdouble relative_y=y-(row-1)*(fv->item_height+fv->row_space);
    if(relative_x<fv->item_width/3){
        //g_print("left c:%d r:%d \n",column,row);
        if(ling_operate_start_operating(op)){
            ling_fixed_view_chain_move_left(fv,column,row);
            // ling_operate_set_animation_cb(op,op->animation,fv);
            //            ling_operate_set_finish_cb(op,chain_move_end_left,data);
            ling_operate_run_animation(op);

        }

        return 1;
    }
    else if(relative_x>fv->item_width/3*2){
        if(ling_operate_start_operating(op)){
            ling_fixed_view_chain_move_right(fv,column,row);
            //ling_operate_set_finish_cb(op,chain_move_end_right,data);
            //ling_operate_set_animation_cb(op,op->animation,fv);
            ling_operate_run_animation(op);
            //ling_fixed_view_chain_move_right(fv,column,row);
        }
        ling_operate_run_animation(op);
        return 1;
    }
    return 0;
}

void ling_fixed_view_get_grid_size(LingFixedView * self,int * column,int *row){
    *column=self->column_num;
    *row=self->row_num;
}

GtkWidget * ling_fixed_view_new(uint arrange,uint column_num,uint row_num,uint column_space,uint row_space){
    //return g_object_new(LING_TYPE_FIXED_VIEW,NULL);
    LingFixedView * self = LING_FIXED_VIEW(g_object_new(LING_TYPE_FIXED_VIEW,NULL));
    ling_fixed_view_set_grid_info(self,arrange,column_num,row_num,column_space,row_space);

    self->chain_move_op = ling_operate_add(shell->controler,"chain_move",chain_move_animate,self,NULL,self,chain_move_end,self);
    return GTK_WIDGET(self);
}

void ling_fixed_view_set_item_drop_data(LingFixedView * self,gpointer data){
    self->item_drop_data = data;
}

static void ling_fixed_view_class_init(LingFixedViewClass * klass){

}

static gboolean chain_move_timeout(gpointer data){
    LingFixedView * fv=data;
    //g_print("chain_move:%f %f\n",fv->now_x,fv->now_y);
    fv->chain_move_timer_id=0;
    ling_fixed_view_chain_move(fv,fv->now_x,fv->now_y);
    return G_SOURCE_REMOVE;
}

static GdkDragAction motion (GtkDropTarget* self,
                            gdouble x,gdouble y,LingFixedView * fv){
    //g_print("x:%f y:%f\n",x,y);
    fv->now_x=x;
    fv->now_y=y;
    if(fv->chain_move_timer_id!=0){
        g_source_remove(fv->chain_move_timer_id);
        fv->chain_move_timer_id=0;
    }fv->chain_move_timer_id = g_timeout_add(300,chain_move_timeout,fv);
    return GDK_ACTION_MOVE;
}

static gboolean accept (GtkDropTarget* self,
                       GdkDrop* drop,LingFixedView * fv){
    //g_print("accept\n");
    return TRUE;
}

static gboolean drop (GtkDropTarget* self,const GValue* value,
                     gdouble x,gdouble y,LingFixedView * fv){
    g_print("drop:x:%f y:%f\n",x,y);
    //if(g_value_ !=LING_TYPE_FIXED_VIEW_ITEM)return FALSE;
    if(fv->chain_move_timer_id!=0){
        g_source_remove(fv->chain_move_timer_id);
        fv->chain_move_timer_id=0;
    }
    LingFixedViewItem * item = g_value_get_object(value);
    uint column,row;
    if(ling_fixed_view_get_available_grid_pos(fv,x,y,item->width,item->height,&column,&row)){
        //ling_fixed_view_add_grid(fv,LING_FIXED_VIEW_ITEM(item),item->width,item->height,column,row);
        if(item->drag_end!=NULL)item->drag_end(item,item->drag_data,fv->item_drop_data);
        ling_fixed_view_add_item_free(fv,LING_FIXED_VIEW_ITEM(item),item->width,item->height,x,y);
        LING_FIXED_VIEW_ITEM(item)->column = column;
        LING_FIXED_VIEW_ITEM(item)->row = row;
        if(ling_operate_start_operating(fv->chain_move_op)){
            ling_operate_run_animation(fv->chain_move_op);
            //ling_operate_set
        }
        //g_print("AAA:%d %d",column,row);
        return TRUE;
    }
    else return FALSE;
}

//static gboolean ddupdate(gpointer data){
//    LingFixedView * fv=data;
//    ling_fixed_view_update(fv);
//    return G_SOURCE_REMOVE;
//}

static void
state_flags_changed (
    GtkWidget* self,
    GtkStateFlags flags,
    gpointer data
    ){
    LingFixedView * fv=data;
    if(fv->is_moving==1)return;
    ling_fixed_view_update(fv);
    //g_print("state_change");
}

//static void
//show (
//  GtkWidget* self,
//  gpointer data
//        ){
//    LingFixedView * fv=data;
//    ling_fixed_view_update(fv);
//}

//static void
//realize (
//  GtkWidget* self,
//  gpointer data
//        ){
//    LingFixedView * fv=data;
//    ling_fixed_view_update(fv);
//}

//static void
//map (
//  GtkWidget* self,
//  gpointer data
//        ){
//    LingFixedView * fv=data;
//    ling_fixed_view_update(fv);
//}

static void ling_fixed_view_init(LingFixedView * self){
    self->fixed = gtk_fixed_new();
    gtk_box_append(GTK_BOX(self),self->fixed);

    GtkDropTarget * drop_target = gtk_drop_target_new(LING_TYPE_FIXED_VIEW_ITEM,GDK_ACTION_MOVE);
    g_signal_connect (drop_target, "motion", G_CALLBACK (motion), self);
    g_signal_connect (drop_target, "drop", G_CALLBACK (drop), self);
    g_signal_connect (drop_target, "accept", G_CALLBACK (accept), self);
    gtk_widget_add_controller (GTK_WIDGET (self), GTK_EVENT_CONTROLLER (drop_target));

    g_signal_connect(self,"state_flags_changed",G_CALLBACK(state_flags_changed),self);
    //g_signal_connect(self,"show",G_CALLBACK(show),self);
    //g_signal_connect(self,"realize",G_CALLBACK(realize),self);
}

void ling_fixed_view_set_margin(LingFixedView * self,int top,int bottom,int start,int end){
    if(top>=0)self->margin_top=top;
    if(bottom>=0)self->margin_bottom=bottom;
    if(start>=0)self->margin_start=start;
    if(end>=0)self->margin_end=end;
}
