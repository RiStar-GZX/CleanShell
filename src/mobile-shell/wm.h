#pragma once

#include <gtk/gtk.h>
#include <lingfixed.h>

G_BEGIN_DECLS

#define CL_TYPE_WM (cl_wm_get_type())
G_DECLARE_FINAL_TYPE(ClWm,cl_wm,CL,WM,LingFixed)


G_END_DECLS
