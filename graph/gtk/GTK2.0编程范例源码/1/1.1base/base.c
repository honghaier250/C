/*最简单的窗口base.c*/
#include<gtk/gtk.h>
int main(int argc, char*argv[])
{	
	//控件定义
	GtkWidget *window;
	//gtk初始化
	gtk_init(&argc,&argv);
	//创建最上层主窗口
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	//显示窗口		
	gtk_widget_show(window);
	//交由管理
	gtk_main();
	return FALSE;

}
