/*完善窗口的功能window.c*/
#include<gtk/gtk.h>
int main(int argc, char*argv[])
{
	GtkWidget *window;
	gtk_init(&argc,&argv);
	//创建最上层主窗口
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	//连接信号"delete_event",使得窗口关闭时发生
	g_signal_connect(G_OBJECT(window),"delete_event",G_CALLBACK(gtk_main_quit),NULL);
	//设置窗口标题	
	gtk_window_set_title(GTK_WINDOW(window),"一个功能完善的窗口");
	//设定窗口的默认宽高
	gtk_window_set_default_size(GTK_WINDOW(window),500,100);
	//设定窗口显示的位置
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
	//显示窗口	
	gtk_widget_show(window);
	gtk_main();
	return FALSE;

}
