#include "linglistbox.h"

struct _LingListPart{
    GtkBox parent;
    GtkWidget * content;
};

G_DEFINE_FINAL_TYPE(LingListPart,ling_list_part,GTK_TYPE_BOX);

static void ling_list_part_class_init(LingListPartClass * klass){

}

static void ling_list_part_init(LingListPart * self){

}

GtkWidget * ling_list_part_new(){
    return g_object_new(LING_TYPE_LIST_PART,NULL);
}

int ling_list_part_set_content(LingListPart * part,GtkWidget * content){
    if(part==NULL||!LING_IS_LIST_PART(part)){
        return 0;
    }
    part->content = content;
    for(GtkWidget * w = gtk_widget_get_first_child(GTK_WIDGET(part));
         w!=NULL;w=gtk_widget_get_first_child(GTK_WIDGET(part))){
        gtk_box_remove(GTK_BOX(part),w);
    }
    gtk_box_append(GTK_BOX(part),content);
    gtk_widget_set_hexpand(content,TRUE);
    return 1;
}

LingListView * ling_list_part_set_as_list(LingListPart * part){
    GtkWidget * list_view = ling_list_view_new();
    if(!ling_list_part_set_content(part,list_view)){
        g_object_unref(list_view);
    }
    return LING_LIST_VIEW(list_view);
}

// LingListGrid * ling_list_part_set_as_grid(LingListPart * part){
//     GtkWidget * list_view = ling_list_view_new();
//     if(!ling_list_part_set_content(part,list_view)){
//         g_object_unref(list_view);
//     }
//     return LING_LIST_VIEW(list_view);
// }

struct _LingListView{
    GtkBox parent;
    GList * parts;
};

G_DEFINE_FINAL_TYPE(LingListView,ling_list_view,GTK_TYPE_BOX);

static void ling_list_view_class_init(LingListViewClass * klass){

}

static void ling_list_view_init(LingListView * self){
    gtk_orientable_set_orientation(GTK_ORIENTABLE(self),GTK_ORIENTATION_VERTICAL);
}

GtkWidget * ling_list_view_new(){
    return g_object_new(LING_TYPE_LIST_VIEW,NULL);
}

void ling_list_view_add_item(LingListView * list_view,GtkWidget * item){
    if(list_view==NULL||item==NULL||!LING_IS_LIST_VIEW(list_view)){
        return;
    }
    gtk_box_append(GTK_BOX(list_view),item);
}

LingListPart * ling_list_view_add_part(LingListView * list_view){
    if(list_view==NULL||!LING_IS_LIST_VIEW(list_view)){
        return NULL;
    }
    GtkWidget * part =ling_list_part_new();
    list_view->parts = g_list_append(list_view->parts,part);
    gtk_box_append(GTK_BOX(list_view),part);
    return LING_LIST_PART(part);
}

