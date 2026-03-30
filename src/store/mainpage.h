#pragma once

#include <gtk/gtk.h>
#include <ling.h>

G_BEGIN_DECLS

#define OMAG_TYPE_MAIN_PAGE (omag_main_page_get_type())
G_DECLARE_FINAL_TYPE(OmagMainPage,omag_main_page,OMAG,MAIN_PAGE,GtkBox)

GtkWidget * omag_main_page_new();

G_END_DECLS

