#include "lingwallpaper.h"

struct _LingWallpaper{
    GtkBox parent;
    GtkWidget * picture;
};

G_DEFINE_FINAL_TYPE(LingWallpaper,ling_wallpaper,GTK_TYPE_BOX)

void ling_wallpaper_class_init(LingWallpaperClass * klass){

}

void ling_wallpaper_init(LingWallpaper * self){

}

GtkWidget * ling_wallpaper_new(const char * path){
    LingWallpaper * self = LING_WALLPAPER(g_object_new(LING_TYPE_WALLPAPER,NULL));
    self->picture = gtk_picture_new_for_filename(path);
    gtk_box_append(GTK_BOX(self),self->picture);
    gtk_picture_set_can_shrink(GTK_PICTURE(self->picture),1);
    gtk_widget_set_vexpand(self->picture,1);
    gtk_widget_set_hexpand(self->picture,1);
    gtk_widget_add_css_class(GTK_WIDGET(self),"wallpaper");
    return GTK_WIDGET(self);
}
