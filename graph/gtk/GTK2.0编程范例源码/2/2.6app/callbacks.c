#include<gtk/gtk.h>
gboolean on_window_delete_event(GtkWidget*widget,GdkEvent *event,gpointer data)
{
	gtk_main_quit();
	return FALSE;
}
void on_file_new_activate (GtkMenuItem *menuitem,gpointer data)
{
	g_print("the file_new is clicked .");
}

void on_file_open_activate (GtkMenuItem *menuitem,gpointer data)
{
}

void on_file_save_activate(GtkMenuItem *menuitem,gpointer data)
{
}

void on_file_saveas_activate(GtkMenuItem *menuitem,gpointer data)
{
}

void on_file_exit_activate(GtkMenuItem *menuitem,gpointer data)
{
}

void on_edit_cut_activate(GtkMenuItem *menuitem,gpointer data)
{
}
void on_edit_copy_activate(GtkMenuItem *menuitem,gpointer data)
{
}

void on_edit_paste_activate(GtkMenuItem *menuitem,gpointer data)
{
}

void on_edit_selectall_activate(GtkMenuItem *menuitem,gpointer data)
{
}

void on_edit_find_activate(GtkMenuItem *menuitem,gpointer data)
{
}

void on_help_help_activate(GtkMenuItem *menuitem,gpointer data)
{
}

void on_help_about_activate(GtkMenuItem *menuitem,gpointer data)
{
}

