/*创建状态栏控件及使用状态栏控件 status.c */
#include<gtk/gtk.h>

//状态栏控件statusbar的指针在主函数外部定义，并且定义为静态变量，
//恰他函数可以非常容易地调用和控制此控件指针，这种方式在以后的单文件编程中经常用到。
static GtkWidget *statusbar;

gint i = 0;

//按钮“堆入”的回调函数
void on_push_clicked(GtkButton* button, gpointer data)
{
	char message[1024];
	i++;
	sprintf(message, "这是输入的第 %d 条消息。", i);
	gtk_statusbar_push(GTK_STATUSBAR(statusbar), i, message);
} 

//按钮“弹出”的回调函数
void on_pop_clicked(GtkButton* button, gpointer data)
{
	if(i < 0)
                return ;
	gtk_statusbar_pop(GTK_STATUSBAR(statusbar), i);
	i--;
}

//状态栏回调函数
void on_popped(GtkStatusbar* statusbar, guint id, const gchar* text)
{
	if(!statusbar->messages)
		i = 0;
}

int main(int argc, char* argv[])
{	
	//定义主窗口对象
	GtkWidget *window;
	//将要放入主窗口的“box”
	GtkWidget *vbox, *hbox;
	//将要放入“box”的按钮对象
	GtkWidget *button;

	//初始化
	gtk_init(&argc, &argv);
	
	//创建窗口
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	//窗口连接信号
	g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(gtk_main_quit), NULL);
	//设置窗口标题
	gtk_window_set_title(GTK_WINDOW(window), "状态栏");
	//设定窗口的默认宽高
	gtk_window_set_default_size(GTK_WINDOW(window),300,200);
	//设置窗口边框宽度
	gtk_container_set_border_width(GTK_CONTAINER(window), 20);

	//创建垂直“ box”
	vbox  = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(window), vbox);

	//创建状态栏
	statusbar = gtk_statusbar_new();
	g_signal_connect(G_OBJECT(statusbar), "text-popped", G_CALLBACK(on_popped), NULL);
	//将状态栏放入到"vbox"中
	gtk_box_pack_start(GTK_BOX(vbox), statusbar, FALSE, FALSE, 5);

	//创建水平“ box”
	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);

	//创建“ 堆入”按钮
	button = gtk_button_new_with_label("堆入");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(on_push_clicked), NULL);
	gtk_box_pack_start(GTK_BOX(hbox), button, TRUE, TRUE, 5);

	//创建“ 弹出”按钮
	button = gtk_button_new_with_label("弹出");
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(on_pop_clicked), NULL);
	gtk_box_pack_start(GTK_BOX(hbox), button, TRUE, TRUE, 5);
	
	//显示窗口
	gtk_widget_show_all(window);
	//GTK程序主循环
	gtk_main();

	return FALSE;
}
