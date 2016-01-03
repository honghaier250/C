#include<gtk/gtk.h>
#include"prop-editor.h"
void on_button_clicked(GtkButton*  button, GtkWidget*  data)
{
	GtkWidget*  dialog;
	dialog = create_prop_editor(data,NULL);
	gtk_widget_show(dialog);
}

int main(int argc,char* argv[])
{
	GtkWidget* window,*vbox;
	GtkWidget* label,*button;
	gtk_init(&argc,&argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window),"设置控件的属性");
	g_signal_connect(G_OBJECT(window),"delete_event",G_CALLBACK(gtk_main_quit),NULL);
	gtk_contaier_set_border_width(GTK_CONTAINER(window),10);
	vbox = gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(window),vbox);
	label = gtk_label_new("此功能用来设置控件的属性\n这用到了GTK+2.0源代码中tests目录下的\nprop-editor.h和prop-editor.c两个文件,\n它们提供了create_prop_editor函数,\n这使我们设定控件的属性变得非常容易.");
	gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,5);
	button = gtk_button_new_with_label("点击此按钮来设置此按钮控件的属性");
	gtk_box_pack_start(GTK_BOX(vbox),button,FALSE,FALSE,5);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(on_button_clicked),button);
	gtk_widget_show_all(window);
	gtk_main();
return FALSE;
}
