#include "ling.h"

void ling_widget_scale(GtkWidget * widget,gdouble scale){
    GtkWidget * parent = gtk_widget_get_parent(GTK_WIDGET(widget));

    GString * str=g_string_new("");
    g_string_printf(str,"box{transform:scale(%f);}",scale);
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
                                    str->str, -1);
    gtk_style_context_add_provider(gtk_widget_get_style_context(widget),
                                   GTK_STYLE_PROVIDER(provider),GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_free(str);
    g_object_unref(provider);
}

void ling_widget_border_radis(GtkWidget * widget,gdouble px){
    GtkWidget * parent = gtk_widget_get_parent(GTK_WIDGET(widget));

    GString * str=g_string_new("");
    g_string_printf(str,"box{border-radius:%fpx;}",px);
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
                                    str->str, -1);
    gtk_style_context_add_provider(gtk_widget_get_style_context(widget),
                                   GTK_STYLE_PROVIDER(provider),GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_free(str);
    g_object_unref(provider);
}
