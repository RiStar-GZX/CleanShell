#pragma once

#include <gtk/gtk.h>
#include <lingoperate.h>
#include <lingshell.h>

G_BEGIN_DECLS

#define LING_TYPE_TASK_SWITCHER (ling_task_switcher_get_type())
G_DECLARE_FINAL_TYPE(LingTaskSwitcher,ling_task_switcher,LING,TASK_SWITCHER,GtkBox)

GtkWidget * ling_task_switcher_new();

G_END_DECLS
