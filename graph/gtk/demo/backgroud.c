#include <gtk/gtk.h>

int main(int argc, char *argv[])
{
    GtkWidget *win;
    GtkWidget *image;
    GtkWidget *frame;
    GtkWidget *fixed;

    gtk_init(&argc, &argv);

    win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_signal_connect(GTK_OBJECT(win), "destroy", G_CALLBACK(gtk_main_quit), NULL);

    /* image = gtk_image_new_from_file("login.png"); */
    /* frame = gtk_frame_new(NULL); */
    /* gtk_container_add(GTK_CONTAINER(frame), image); */
    /* gtk_container_add(GTK_CONTAINER(win), frame); */

    fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(win), fixed);
    image = gtk_image_new_from_file("login.png");
    gtk_fixed_put(GTK_FIXED(fixed), image, 0, 0);

    gtk_widget_show_all(win);
    gtk_main();

    return 0;
}
