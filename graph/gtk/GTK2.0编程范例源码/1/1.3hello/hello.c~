/*hello程序hello.c*/
#include<gtk/gtk.h>
gint count = 1;
void on_button_clicked(GtkWidget * button, gpointer userdata)
{
	//g_print("你好，这是测试程序。");	
	g_print("Hello, this is a test!");
	//g_print("%d\n",(gint)userdata);
	g_print("%d\n",count);
	count = count +1;
}

int main(int argc, char*argv[])
{
	GtkWidget *window;
	GtkWidget *button;	
	gtk_init(&argc,&argv);
	//创建最上层主窗口
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	//连接信号"delete_event",使得窗口关闭时发生
	g_signal_connect(G_OBJECT(window),"delete_event",G_CALLBACK(gtk_main_quit),NULL);
	//设置窗口标题	
	gtk_window_set_title(GTK_WINDOW(window),"Hello 功能实现");
	//设定窗口的默认宽高
	gtk_window_set_default_size(GTK_WINDOW(window),300,200);
	//设定窗口的位置
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
	//设定容器边框的宽度，否则按钮填充整个窗口
	gtk_container_set_border_width(GTK_CONTAINER(window),60);

	//创建一个带有标签的按钮
	button = gtk_button_new_with_label("按下此按钮会在终端上显示一行信息");
	//连接信号"clicked",使得窗口关闭时发生
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(on_button_clicked),(gpointer)count);
	//把按钮装入窗口
	gtk_container_add(GTK_CONTAINER(window),button);	
	
	//显示窗口和按钮
	gtk_widget_show(button);
	gtk_widget_show(window);
	gtk_main();
	return FALSE;

}
