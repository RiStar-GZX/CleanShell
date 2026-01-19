#include "ling.h"

void ling_widget_scale(GtkWidget * widget,gdouble scale){
    GtkWidget * parent = gtk_widget_get_parent(GTK_WIDGET(widget));
    // int w = gtk_widget_get_width(parent);
    // int h = gtk_widget_get_height(parent);
    // gtk_widget_set_margin_top(GTK_WIDGET(self),progress/100.00f*h);
    // gtk_widget_set_margin_bottom(GTK_WIDGET(self),progress/100.00f*h);
    // gtk_widget_set_margin_start(GTK_WIDGET(self),progress/100.00f*w);
    // gtk_widget_set_margin_end(GTK_WIDGET(self),progress/100.00f*w);
    // gtk_widget_set_margin_top(widget,progress/100.00f*30);
    // gtk_widget_set_margin_bottom(widget,progress/100.00f*30);
    // gtk_widget_set_margin_start(widget,progress/100.00f*30);
    // gtk_widget_set_margin_end(widget,progress/100.00f*30);

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
