#include<gtk/gtk.h>
#include"callbacks.h"
#include"interface.h"
int main(int argc,gchar *argv[])
{
	//声明了窗口控件
	GtkWidget *window;
	//初始化命令行参数
	gtk_init(&argc,&argv);
	window=create_window();
	//信号连接
	g_signal_connect(G_OBJECT(window),"delete_event",G_CALLBACK(on_window_delete_event),NULL);
	//显示窗口
	gtk_widget_show(window);
	gtk_main();
	//主事件循环
	return FALSE;
}

