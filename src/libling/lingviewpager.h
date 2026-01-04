#pragma once

#include <gtk/gtk.h>
#include <lingoperate.h>
#include <mobileshell.h>
#include <lingfixed.h>

G_BEGIN_DECLS

#define LING_TYPE_VIEW_PAGER (ling_view_pager_get_type())
    G_DECLARE_FINAL_TYPE(LingViewPager,ling_view_pager,LING,VIEW_PAGER,GtkBox)

void ling_view_pager_fixed_set_page_pos(LingViewPager * self,uint pos,gdouble x,gdouble y,int level);

GtkWidget * ling_view_pager_new_with_op(gboolean size_adapt);

GtkWidget * ling_view_pager_new(gboolean size_adapt);

void ling_view_pager_add_page(LingViewPager * self,GtkWidget * page);

void ling_view_pager_remove_page(LingViewPager * self,GtkWidget * page);

void ling_view_pager_set_dot_indicator(LingViewPager * self,gboolean able);

void ling_view_pager_set_pos(LingViewPager * self,uint pos);

GtkWidget * ling_view_pager_get_page_by_pos(LingViewPager * self,uint pos);

void ling_view_pager_fixed_get_page_pos(LingViewPager * self,uint pos,gdouble * x,gdouble * y);

uint ling_view_pager_get_page_pos(LingViewPager * self,GtkWidget * page);

GtkWidget * ling_view_pager_get_page_now(LingViewPager * self);

uint ling_view_pager_get_page_now_pos(LingViewPager * self);

uint ling_view_pager_get_page_num(LingViewPager * self);

gboolean ling_view_pager_get_page_cycle(LingViewPager * self);

int ling_view_pager_get_pos(LingViewPager * self);

void ling_view_pager_show_page(LingViewPager * self,uint pos);

void ling_view_pager_set_page_cycle(LingViewPager * self,gboolean able);

gboolean ling_view_pager_next(LingViewPager * self,gboolean animate);

gboolean ling_view_pager_prev(LingViewPager * self,gboolean animate);




void ling_view_pager_finish_left(GtkWidget * widget,LingActionArgs args,gpointer data);

void ling_view_pager_finish_right(GtkWidget * widget,LingActionArgs args,gpointer data);

void ling_view_pager_finish_center(GtkWidget * widget,LingActionArgs args,gpointer data);

gdouble ling_view_pager_progress(GtkWidget * widget,LingActionArgs args,gpointer user_data);

gboolean ling_view_pager_release(GtkWidget * widget,LingActionArgs args,gpointer data);

void ling_view_pager_ani(GtkWidget * widget,LingActionArgs args,gpointer user_data);

G_END_DECLS
