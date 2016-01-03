#include <gtk/gtk.h>

int main(int argc, char *argv[])
{
    GtkWidget *win;
    GtkWidget *image;
    GtkWidget *frame;
    GtkWidget *fixed;
    GtkWidget *combo;
    GList *items;

    gtk_init(&argc, &argv);

    win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_signal_connect(GTK_OBJECT(win), "destroy", G_CALLBACK(gtk_main_quit), NULL);

    combo = gtk_combo_new ();
    gtk_container_add(GTK_CONTAINER(win), combo);

    items = g_list_append (items, "First Item");
    items = g_list_append (items, "Second Item");
    items = g_list_append (items, "Third Item");
    items = g_list_append (items, "Fourth Item");
    items = g_list_append (items, "Fifth Item");
    gtk_combo_set_popdown_strings (GTK_COMBO (combo), items);

    gtk_widget_show_all(win);
    gtk_main();

    return 0;
}

