#include <gtk/gtk.h>
#include <desktop.h>
#include <lingviewpager.h>
#include <lingdatasaver.h>
#include <lockscreen.h>

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
    GtkWidget * appview = clm_shell_start();
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
    //g_print("%d.%d.%d\n",GTK_MAJOR_VERSION,GTK_MINOR_VERSION,GTK_MICRO_VERSION);
    app = gtk_application_new ("org.ling.lib", G_APPLICATION_HANDLES_OPEN);
    g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
    g_signal_connect (app, "open", G_CALLBACK (app_open), NULL);
    stat =g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);
    return stat;
}

// #include <gtk/gtk.h>

// struct _MyFixedLayout
// {
//     GtkLayoutManager parent;
// };

// #define MY_TYPE_FIXED_LAYOUT (my_fixed_layout_get_type())
// G_DECLARE_FINAL_TYPE(MyFixedLayout,my_fixed_layout,MY,FIXD_LAYOUT, GtkLayoutManager)

// G_DEFINE_FINAL_TYPE(MyFixedLayout,my_fixed_layout,GTK_TYPE_LAYOUT_MANAGER)

// int i=0;
// void my_allocate(GtkLayoutManager *manager,
//                  GtkWidget        *widget,
//                  int               width,
//                  int               height,
//                  int               baseline){
//     graphene_rect_t rect;


//     for(GtkWidget * child = gtk_widget_get_first_child(widget);child!=NULL;child=gtk_widget_get_next_sibling(child)) {
//         gtk_widget_compute_bounds(widget,child,&rect);
//         GtkAllocation child_alloc = {
//             .x = 0+i,
//             .y = 0+i,
//             .width = rect.size.width/2,
//             .height = rect.size.height/2
//         };
//         i+=rect.size.width/2;
//         // 分配空间并确保可见
//         gtk_widget_size_allocate(child, &child_alloc,0);
//         // if (!gtk_widget_get_visible(widget))
//         // gtk_widget_show(widget);
//     }
//     // GtkWidget * parent = gtk_widget_get_parent(widget);
//     // if(parent==NULL)return;
//     // GtkAllocation child_allocation;
//     // GtkAllocation alloc;
//     // gtk_widget_get_allocation(parent, &alloc);
//     // graphene_rect_t rect;
//     // gtk_widget_compute_bounds(parent,widget,&rect);
//     // g_print("b:%d %d\n",width,height);
//     // child_allocation.height=rect.size.height;
//     // child_allocation.width=rect.size.width/2;
//     // child_allocation.x=0;
//     // child_allocation.y=0;
//     // gtk_widget_size_allocate(widget, &child_allocation, baseline);
// }

// void  my_measure(GtkLayoutManager *manager,
//                 GtkWidget        *widget,
//                 GtkOrientation    orientation,
//                 int               for_size,
//                 int              *minimum,
//                 int              *natural,
//                 int              *minimum_baseline,
//                 int              *natural_baseline){
//     // 获取容器分配尺寸
//     GtkAllocation allocation;
//     gtk_widget_get_allocation(gtk_widget_get_parent(widget), &allocation);
//     // 根据方向返回尺寸
//     if (orientation == GTK_ORIENTATION_HORIZONTAL) {
//         *minimum = *natural = allocation.width;
//         *minimum_baseline = *natural_baseline = -1;
//     } else {
//         *minimum = *natural = allocation.height;
//         *minimum_baseline = *natural_baseline = -1;
//     }
//     g_print("a:%d %d\n",allocation.width,allocation.height);
// }

// void my_fixed_layout_class_init(MyFixedLayoutClass * klass){
//     GtkLayoutManagerClass * manager_class = GTK_LAYOUT_MANAGER_CLASS(klass);
//     manager_class->allocate = my_allocate;
//     //manager_class->measure = my_measure;
// }

// void my_fixed_layout_init(MyFixedLayout * self){

// }

// GtkLayoutManager * my_fixed_layout(){
//     return g_object_new(MY_TYPE_FIXED_LAYOUT,NULL);
// }

// // 示例：在小部件中使用自定义布局
// static void activate(GtkApplication *app, gpointer user_data){
//     GtkWidget *window = gtk_application_window_new(app);
//     gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

//     GtkWidget * fixed = gtk_fixed_new();//box_new(GTK_ORIENTATION_HORIZONTAL,0);
//     gtk_widget_set_hexpand(fixed,TRUE);
//     gtk_widget_set_vexpand(fixed,TRUE);
//     gtk_window_set_child(GTK_WINDOW(window), fixed);
//     gtk_widget_set_layout_manager(GTK_WIDGET(fixed), my_fixed_layout());

//     // 创建容器并设置自定义布局
//     GtkWidget *b1 = /*gtk_image_new_from_icon_name("spotify");//*/gtk_button_new_with_label("111");//fixed_new()
//     //gtk_box_append(GTK_BOX(fixed),b1);
//     // gtk_widget_set_layout_manager(b1,my_fixed_layout());
//     gtk_widget_set_parent(b1,fixed);

//     GtkWidget *b2 = /*gtk_image_new_from_icon_name("firefox");//*/gtk_button_new_with_label("111");//fixed_new();
//     //gtk_box_append(GTK_BOX(fixed),b2);
//     //gtk_widget_set_layout_manager(b2,my_fixed_layout());
//     //gtk_widget_set_parent(b2,fixed);
//     gtk_fixed_put(GTK_FIXED(fixed),b2,0,0);
//     // 添加子部件（自动铺满）
//     //GtkWidget *button = gtk_button_new_with_label("Stretchy Button");
//     //GtkLayoutChild * layout_child = create_layout_child(fixed_layout,fixed,button);
//     //gtk_widget_set_parent(button,fixed);
//     //gtk_box_append(GTK_BOX(box), button);
//     gtk_window_present(GTK_WINDOW(window));
// }

// int main(int argc, char **argv){
//     GtkApplication *app = gtk_application_new("org.example.stretchy", G_APPLICATION_DEFAULT_FLAGS);
//     g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
//     return g_application_run(G_APPLICATION(app), argc, argv);
// }
