#include <gtk/gtk.h>


int main(int argc, char *argv[])
{
    GtkWidget *window, *event_box, *label;
    GdkColor color;
    GdkPixbuf *quit_icon;
    GtkWidget *quit;
    GtkWidget *button2;
    GtkWidget *hbox;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

    quit_icon=gdk_pixbuf_new_from_file("./finish.png", NULL);
    quit    =  gtk_image_new_from_pixbuf(quit_icon);

    hbox    =  gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), quit, FALSE, FALSE, 0);

    button2 =  gtk_button_new();
    gtk_container_add(GTK_CONTAINER(button2), hbox);
    gtk_container_add(GTK_CONTAINER(window), button2);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}


