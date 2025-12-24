#include <gtk/gtk.h>
#include <lingdesktop.h>
#include <lingviewpager.h>
#include <lingdatasaver.h>
#include <linglockscreen.h>

static void app_activate (GApplication *app) {
    // 应用CSS样式
    GtkCssProvider * provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider,"../../ui/style.css");
    gtk_style_context_add_provider_for_display(gdk_display_get_default(),
                                               GTK_STYLE_PROVIDER(provider),
                                               GTK_STYLE_PROVIDER_PRIORITY_USER);

    GtkWidget * window=gtk_window_new();
    //GtkWidget * appview = ling_desktop_new();
    //GtkWidget * appview = ling_lock_screen_new();
    GtkWidget * appview = ling_shell_start();
    gtk_window_set_child(GTK_WINDOW(window),appview);
    gtk_window_set_application(GTK_WINDOW(window),GTK_APPLICATION(app));
    gtk_window_set_default_size(GTK_WINDOW(window),500,1000);
    gtk_window_set_resizable(GTK_WINDOW(window),0);
    gtk_window_present(GTK_WINDOW(window));

}

static void app_open (GApplication *app, GFile ** files, gint n_files, gchar *hint) {

}

int main (int argc, char **argv) {
    GtkApplication *app;
    int stat;
    app = gtk_application_new ("org.ling.lib", G_APPLICATION_HANDLES_OPEN);
    g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
    g_signal_connect (app, "open", G_CALLBACK (app_open), NULL);
    stat =g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);
    return stat;
}
