 #include "lingdygrid.h"

struct _LingDyGridLayout{
    GtkLayoutManager manager;
};

G_DEFINE_FINAL_TYPE(LingDyGridLayout,ling_dy_grid_layout,GTK_TYPE_LAYOUT_MANAGER);

struct _LingDyGrid{
    GtkWidget parent;
    GtkLayoutManager * manager;
    int item_w,item_h;
    int space_min;//,space_max;    //动态方向的间隔
    int space;  //静态方向的间隔
};

G_DEFINE_FINAL_TYPE(LingDyGrid,ling_dy_grid,GTK_TYPE_WIDGET);

static void ling_dy_side_layout_allocate(GtkLayoutManager *manager,GtkWidget *widget,
                                        int width,int height,int baseline){
    GtkWidget * parent = gtk_widget_get_parent(widget);
    if(parent==NULL)return;

    if(width<=0){
        width = gtk_widget_get_width(gtk_widget_get_parent(widget));
    }
    if(height<=0){
        height = gtk_widget_get_height(gtk_widget_get_parent(widget));
    }

    graphene_rect_t rect;
    if(!gtk_widget_compute_bounds(parent,widget,&rect))return;
    int start = gtk_widget_get_margin_start(widget);
    int end = gtk_widget_get_margin_end(widget);
    int top = gtk_widget_get_margin_top(widget);
    int bottom = gtk_widget_get_margin_bottom(widget);
    rect.size.height-=top+bottom;
    rect.size.width-=start+end;

    LingDyGrid * grid = LING_DY_GRID(widget);

    GtkAllocation alloc;
    alloc.width = grid->item_w;
    alloc.height = grid->item_h;
    int num;
    int space;
    for(num=1;;num++){
        space = width - num*grid->item_w;
        space/=(num+1);
        if(space<grid->space_min){
            num--;
            space = width - num*grid->item_w;
            space/=(num+1);
            break;
        }
    }
    if(num==0)return;

    g_print("num:%d space:%d\n",num,space);

    int column=1,row=1;
    for(GtkWidget * w = gtk_widget_get_first_child(widget);w!=NULL;w=gtk_widget_get_next_sibling(w)){
        if(column<=num){
            alloc.x = space*column+grid->item_w*(column-1);
            column++;
        }
        else{
            row++;
            column=1;
            alloc.x = space*column+grid->item_w*(column-1);
        }

        alloc.y = grid->space*row+grid->item_h*(row-1);
        gtk_widget_size_allocate(w, &alloc, baseline);
    }
}

static void ling_dy_grid_layout_class_init(LingDyGridLayoutClass * klass){
    GtkLayoutManagerClass * manager_class = GTK_LAYOUT_MANAGER_CLASS(klass);
    manager_class->allocate = ling_dy_side_layout_allocate;
}

static void ling_dy_grid_layout_init(LingDyGridLayout * self){

}

GtkLayoutManager * ling_dy_grid_layout_new(){
    return g_object_new(LING_TYPE_DY_GRID_LAYOUT,NULL);
}

static void ling_dy_grid_class_init(LingDyGridClass * klass){

}

static void ling_dy_grid_init(LingDyGrid * self){
    self->manager = ling_dy_grid_layout_new();
    gtk_widget_set_layout_manager(GTK_WIDGET(self),self->manager);
}

GtkWidget * ling_dy_grid_new(int item_w,int item_h,int space_min,int space){
    LingDyGrid * grid = LING_DY_GRID(g_object_new(LING_TYPE_DY_GRID,NULL));
    grid->item_w = item_w;
    grid->item_h = item_h;
    grid->space_min = space_min;
    grid->space = space;
    return GTK_WIDGET(grid);
}

static void ling_dy_grid_allocate(LingDyGrid * grid){

}

void ling_dy_grid_add_item(LingDyGrid * grid,GtkWidget * item){
    if(grid==NULL||item==NULL||
        !LING_IS_DY_GRID(grid)){
        return;
    }
    gtk_widget_set_parent(item,GTK_WIDGET(grid));
    ling_dy_grid_allocate(grid);
}

void ling_dy_grid_remove_item(LingDyGrid * grid,GtkWidget * item){
    if(grid==NULL||item==NULL||
        !LING_IS_DY_GRID(grid)){
        return;
    }
    GtkWidget * widget = ling_get_child_in_widget(GTK_WIDGET(grid),item);
    if(widget)gtk_widget_unparent(widget);
    ling_dy_grid_allocate(grid);
}
