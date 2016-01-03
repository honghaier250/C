/*添加工具条toolbar.c*/
#include<gtk/gtk.h>

void on_button_clicked(GtkButton *button, gpointer data)
{
	g_print("你点击的按钮是 %s.\n",(gchar *)data);
	//g_print("Button %s is pressed.\n",(gchar *)data);
}
//主函数
int main(int argc, char*argv[])
{
	GtkWidget *window;
	GtkWidget *box;
	GtkWidget *toolbar;	
	GtkWidget *entry;	
	GtkWidget *label;	

	//初始化GTK
	gtk_init(&argc,&argv);
	//创建最上层主窗口
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	//连接信号"delete_event",使得窗口关闭时发生
	g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_main_quit),NULL);
	//设置窗口标题	
	gtk_window_set_title(GTK_WINDOW(window),"添加工具条");
	//设定窗口的默认宽高
	gtk_window_set_default_size(GTK_WINDOW(window),300,200);
	//设定窗口的位置,让窗口总在最前端
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);

	//创建盒子
	box = gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(window),box);

	//创建工具条
	toolbar = gtk_toolbar_new( );
	gtk_box_pack_start(GTK_BOX(box),toolbar,FALSE,FALSE,0);	

	//向工具条中插入按钮：可 添加带文字或图像的，可向前添加，可插入，下面的函数为向工具条中插入系统固有资源项目；
	//第1个参数指出要添加的工具条，第2个参数指出系统固有资源项目，第3、4个参数指出按钮可用和不可用时的提示信息；
	//第5个参数指出回调函数，用宏进行了转换，第6个参数指出传回给回调函数的参数；
	//最后一个参数指出插入按钮的位置，－1表示最后，NULL表示依次；
	gtk_toolbar_insert_stock(GTK_TOOLBAR(toolbar),GTK_STOCK_NEW,"新建一个文件","新建",GTK_SIGNAL_FUNC(on_button_clicked),(gpointer)("新建"),-1);
	gtk_toolbar_insert_stock(GTK_TOOLBAR(toolbar),GTK_STOCK_OPEN,"打开一个文件","打开",GTK_SIGNAL_FUNC(on_button_clicked),(gpointer)("打开"),-1);
	gtk_toolbar_insert_stock(GTK_TOOLBAR(toolbar),GTK_STOCK_SAVE,"保存当前文件","保存",GTK_SIGNAL_FUNC(on_button_clicked),(gpointer)("保存"),-1);
	
	//向工具条中添加其他控件：这里添加了文字标签和一个单行文本录入控件
	label = gtk_label_new("文件名：");
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar),label,"这是一个标签","标签");
	entry = gtk_entry_new();
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar),entry,"这是一个录入条","录入");
	
	//向工具条中添加竖线间隔
	gtk_toolbar_append_space(GTK_TOOLBAR(toolbar));

	gtk_toolbar_insert_stock(GTK_TOOLBAR(toolbar),GTK_STOCK_CUT,"剪切","剪切",GTK_SIGNAL_FUNC(on_button_clicked),(gpointer)("剪切"),-1);
	gtk_toolbar_insert_stock(GTK_TOOLBAR(toolbar),GTK_STOCK_COPY,"复制","复制",GTK_SIGNAL_FUNC(on_button_clicked),(gpointer)("复制"),-1);
	gtk_toolbar_insert_stock(GTK_TOOLBAR(toolbar),GTK_STOCK_PASTE,"粘贴","粘贴",GTK_SIGNAL_FUNC(on_button_clicked),(gpointer)("粘贴"),-1);
	gtk_toolbar_insert_stock(GTK_TOOLBAR(toolbar),GTK_STOCK_QUIT,"退出","退出",GTK_SIGNAL_FUNC(on_button_clicked),(gpointer)("退出"),-1);

	//设定工具条的外观,GTK_TOOLBAR_ICONS表示只显示图标
	gtk_toolbar_set_style(GTK_TOOLBAR(toolbar),GTK_TOOLBAR_ICONS);
	//设定工具条中图标大小，GTK_ICON_SIZE_SMALL_TOOLBAR表示工具条中使用小图标；
	gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar),GTK_ICON_SIZE_SMALL_TOOLBAR);;

	//显示所有控件
	gtk_widget_show_all(window);
	gtk_main();
	return FALSE;
}
