#pragma once

#include <gtk/gtk.h>
#include <ling.h>

G_BEGIN_DECLS

#define LING_TYPE_LIST_PART (ling_list_part_get_type())
G_DECLARE_FINAL_TYPE(LingListPart,ling_list_part,LING,LIST_PART,GtkBox)

#define LING_TYPE_LIST_VIEW (ling_list_view_get_type())
G_DECLARE_FINAL_TYPE(LingListView,ling_list_view,LING,LIST_VIEW,GtkBox)

/*list_part*/

GtkWidget * ling_list_part_new();

int ling_list_part_set_content(LingListPart * part,GtkWidget * content);

LingListView * ling_list_part_set_as_list(LingListPart * part);

/*list_view*/

GtkWidget * ling_list_view_new();

void ling_list_view_add_item(LingListView * list_view,GtkWidget * item);

LingListPart * ling_list_view_add_part(LingListView * list_view);

G_END_DECLS

