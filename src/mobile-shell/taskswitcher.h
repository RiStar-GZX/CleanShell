#pragma once

#include <gtk/gtk.h>
#include <lingoperate.h>
#include <mobileshell.h>

G_BEGIN_DECLS

#define CLM_TYPE_TASK_PAGE (clm_task_page_get_type())
G_DECLARE_FINAL_TYPE(ClmTaskPage,clm_task_page,CLM,TASK_PAGE,GtkBox)


#define CLM_TYPE_TASK_SWITCHER (clm_task_switcher_get_type())
G_DECLARE_FINAL_TYPE(ClmTaskSwitcher,clm_task_switcher,CLM,TASK_SWITCHER,GtkBox)

GtkWidget * clm_task_switcher_new();
GtkWidget * clm_task_switcher_get_bar(ClmTaskSwitcher * self);

void clm_task_switcher_set_opacity(ClmTaskSwitcher * self,gdouble opacity);

G_END_DECLS
