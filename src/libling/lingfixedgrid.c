// #include "lingfixedgrid.h"

// struct _LingFixedGridLayout{
//     LingFixedLayout parent;
//     uint cloumn,row;
// };

// G_DEFINE_FINAL_TYPE(LingFixedGridLayout,ling_fixed_grid_layout,LING_TYPE_FIXED_LAYOUT)

// static void ling_fixed_grid_layout_class_init(LingFixedGridLayoutClass * klass){

// }

// static void ling_fixed_grid_layout_init(LingFixedGridLayout * self){

// }


// struct _LingFixedGrid{
//     LingFixed parent;
//     uint column,row;
//     uint column_space;
//     uint row_space;
// };

// G_DEFINE_FINAL_TYPE(LingFixedGrid,ling_fixed_grid,GTK_TYPE_FIXED)

// static void ling_fixed_grid_class_init(LingFixedGridClass * klass){

// }

// static void ling_fixed_grid_init(LingFixedGrid * self){

// }

// GtkWidget * ling_fixed_grid_new(uint column,uint row){
//     LingFixedGrid * self = LING_FIXED_GRID(g_object_new(LING_TYPE_FIXED_GRID,NULL));
//     self->column = column;
//     self->row = row;
//     return GTK_WIDGET(self);
// }

// GtkAllocation grid_adjust(GtkLayoutManager *manager,GtkWidget *widget,
//                                    int width,int height,int baseline,graphene_rect_t rect){

// }

// void ling_fixed_grid_put(LingFixedGrid *fg,GtkWidget * widget,uint c,uint r){
//     //ling_fixed_put(GTK_FIXED(fg),widget,grid_adjust,);
// }
