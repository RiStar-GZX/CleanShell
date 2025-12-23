#pragma once

#include <gtk/gtk.h>
#include <lingoperate.h>
#include <lingshell.h>

G_BEGIN_DECLS

#define LING_TYPE_VIEW_PAGER (ling_view_pager_get_type())
    G_DECLARE_FINAL_TYPE(LingViewPager,ling_view_pager,LING,VIEW_PAGER,GtkBox)

GtkWidget * ling_view_pager_new();

void ling_view_pager_add_page(LingViewPager * self,GtkWidget * page);

void ling_view_pager_remove_page(LingViewPager * self,GtkWidget * page);

void ling_view_pager_set_dot_indicator(LingViewPager * self,gboolean able);

GtkWidget * ling_view_pager_get_page_by_pos(LingViewPager * self,uint pos);

GtkWidget * ling_view_pager_get_page_now(LingViewPager * self);

uint ling_view_pager_get_page_num(LingViewPager * self);

int ling_view_pager_get_pos(LingViewPager * self);

void ling_view_pager_show_page(LingViewPager * self,uint pos);

void ling_view_pager_set_page_cycle(LingViewPager * self,gboolean able);

gboolean ling_view_pager_next(LingViewPager * self,gboolean animate);

gboolean ling_view_pager_prev(LingViewPager * self,gboolean animate);



