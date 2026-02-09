// #include "lingdragface.h"

// struct _LingDragFace{
//     LingFixed parent;

//     GtkWidget * drag;   //被拽控件

//     int sx;
//     int sy;
//     int ex;
//     int ey;
// };

// G_DEFINE_FINAL_TYPE(LingDragFace,ling_drag_face,LING_TYPE_FIXED)

// void ling_drag_face_class_init(LingDragFaceClass * klass){
//     //gtk_layout_new()
// }

// void ling_drag_face_init(LingDragFace * self){

// }

// GtkWidget * ling_drag_face_new(){
//     return g_object_new(LING_TYPE_DRAG_FACE,NULL);
// }

// typedef struct {
//     LingDragFace * face;
//     ITEM_DRAG_START_CB start;
//     gpointer start_data;
//     ITEM_DRAG_ANI_CB ani;
//     gpointer ani_data;
//     ITEM_DRAG_END_CB end;
//     gpointer end_data;
// }drag_item_arg;

// static gdouble drag_progress(GtkWidget * widget,LingActionArgs args,gpointer user_data){
//     drag_item_arg * arg = (drag_item_arg *)user_data;
//     LingDragFace * face = arg->face;
//     if(face->drag==NULL){
//         GtkWidget * drag=widget;
//         if(arg->start!=NULL)drag = arg->start(widget,arg->start_data);
//         face->drag = drag;
//         face->ex = args.start_x;
//         face->ey = args.start_y;
//         gtk_widget_set_visible(GTK_WIDGET(arg->face),TRUE);
//     }
//     face->sx = args.offset_x + args.start_x;
//     face->sy = args.offset_y + args.start_y;
//     return 0;
// }

// static gboolean drag_release(GtkWidget * widget,LingActionArgs args,gpointer user_data){
//     drag_item_arg * arg = (drag_item_arg *)user_data;
//     LingDragFace * face = arg->face;
//     return LING_OPERATE_ANIMATION_DIR_FORWARD;
// }

// static void drag_ani(GtkWidget * widget,LingActionArgs args,gpointer user_data){
//     drag_item_arg * arg = (drag_item_arg *)user_data;
//     LingDragFace * face = arg->face;
//     gdouble nx = face->sx +(face->ex-face->sx)*args.progress;   //直线回去
//     gdouble ny = face->sy +(face->ey-face->sy)*args.progress;
//     ling_fixed_move(LING_FIXED(face),face->drag,nx,ny);
// }

// static void drag_finish_start(GtkWidget * widget,LingActionArgs args,gpointer user_data){
//     drag_item_arg * arg = (drag_item_arg *)user_data;
//     LingDragFace * face = arg->face;
//     face->drag = NULL;
// }

// static void drag_finish_end(GtkWidget * widget,LingActionArgs args,gpointer user_data){
//     drag_item_arg * arg = (drag_item_arg *)user_data;
//     LingDragFace * face = arg->face;
//     ling_fixed_remove(LING_FIXED(face),face->drag);
//     face->drag = NULL;
//     if(arg->end!=NULL)arg->end(widget,arg->end_data);
//     gtk_widget_set_visible(GTK_WIDGET(arg->face),FALSE);
// }

// LingOperate * ling_drag_item_add_operate(LingDragFace * face,GtkWidget * widget,LING_ACTION action,
//                                         ITEM_DRAG_START_CB start,gpointer start_data,
//                                         ITEM_DRAG_ANI_CB ani,gpointer ani_data,
//                                         ITEM_DRAG_END_CB end,gpointer end_data){
//     LingOperate * op = ling_operate_add(shell->controler,"drag_face_item_drag",widget);
//     drag_item_arg * arg =g_malloc0(sizeof(drag_item_arg));
//     arg->face = face;
//     arg->start = start;
//     arg->start_data = start_data;
//     arg->ani = ani;
//     arg->ani_data = ani_data;
//     arg->end = end;
//     arg->end_data = end_data;

//     ling_operate_add_action(op,action,
//                             drag_progress,arg,
//                             drag_ani,arg,
//                             drag_release,arg,
//                             drag_finish_start,drag_finish_end,arg);
//     return op;
// }

