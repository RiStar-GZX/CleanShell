#include <gtk/gtk.h>
#include <desktop.h>
#include <lingviewpager.h>
#include <lingdatasaver.h>
#include <lockscreen.h>

#include <gtk/gtk.h>

GdkFrameClock * my_clock;

gboolean fps_timeout(gpointer user_data){
    ClmShell * shell = CLM_SHELL(user_data);

    gdouble frame = gdk_frame_clock_get_fps(my_clock);
    char str[100];
    sprintf(str,"%3.0f",frame);
    gtk_label_set_text(GTK_LABEL(shell->fps_label),str);

    return G_SOURCE_CONTINUE;
}

static void app_activate (GApplication *app) {
    // 应用CSS样式
    GtkCssProvider * provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider,"../../ui/style.css");
    gtk_style_context_add_provider_for_display(gdk_display_get_default(),
                                               GTK_STYLE_PROVIDER(provider),
                                               GTK_STYLE_PROVIDER_PRIORITY_USER);
    //gtksetting
    GtkSettings *settings = gtk_settings_get_default();
    // 设置拖放阈值为0像素（为了桌面图标流畅的拖拽）
    g_object_set(settings, "gtk-dnd-drag-threshold", 1, NULL);

    GtkWidget * window=gtk_window_new();
    //GtkWidget * appview = ling_desktop_new();
    //GtkWidget * appview = ling_lock_screen_new();
    GtkWidget * appview = clm_shell_start();
    gtk_window_set_child(GTK_WINDOW(window),appview);
    gtk_window_set_application(GTK_WINDOW(window),GTK_APPLICATION(app));
    gtk_window_set_default_size(GTK_WINDOW(window),500,1000);
    gtk_window_set_resizable(GTK_WINDOW(window),0);
    gtk_window_present(GTK_WINDOW(window));

    my_clock = gtk_widget_get_frame_clock(GTK_WIDGET(window));
    g_timeout_add(500,fps_timeout,appview);
}

static void app_open (GApplication *app, GFile ** files, gint n_files, gchar *hint) {

}

int main (int argc, char **argv) {
    GtkApplication *app;
    int stat;
    //g_print("%d.%d.%d\n",GTK_MAJOR_VERSION,GTK_MINOR_VERSION,GTK_MICRO_VERSION);
    app = gtk_application_new ("org.ling.lib", G_APPLICATION_HANDLES_OPEN);
    g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
    g_signal_connect (app, "open", G_CALLBACK (app_open), NULL);
    stat =g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);
    return stat;
}
