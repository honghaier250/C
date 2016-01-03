#include<gtk/gtk.h>

//定义全局变量

gboolean isbold = FALSE;
gboolean isitli = FALSE;
gboolean isuline = FALSE;
gboolean iscolor = FALSE;

//关于check的一个回调函数

void on_check_clicked(GtkWidget *check,gpointer data)
{
	switch((int)data)
	{
	case 1:
		isbold = !isbold;
		break;
	case 2:
                isitli = !isitli;
                break;
	case 3:
                isuline = !isuline;
                break;
	case 4:
                iscolor = !iscolor;
                break;
	}
}

//关于按钮的一个回调函数

void on_button_clicked(GtkWidget *button,gpointer data)
{
	g_print("字体配置为：");
	if(isbold)
		g_print("粗体");
        if(isitli)
                g_print("斜体");
        if(isuline)
                g_print("下划线");
        if(iscolor)
                g_print("彩色");
        if(!isbold && !iscolor && !isuline && !isitli)
                g_print("正常，无任何选项");
	g_print("\n");
}

//主函数

int main(int argc,char *argv[])
{

//定义在窗口中要出现的控件
//定义窗口
	GtkWidget *window;
//定义框架控件
	GtkWidget *frame;
//定义盒装控件
	GtkWidget *box;
//定义按钮控件
	GtkWidget *button;
//定义盒子控件
	GtkWidget *box1;
//定义多选按钮控件
	GtkWidget *check1;
	GtkWidget *check2;
	GtkWidget *check3;
	GtkWidget *check4;
//定义一段文字
	char *title = "多项选择窗口";
//初始化
	gtk_init(&argc,&argv);
//创建窗口
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
//定义窗口的标题
	gtk_window_set_title(GTK_WINDOW(window),title);
//为窗口设置边缘宽度
	gtk_container_set_border_width(GTK_CONTAINER(window),20);
//为信号连接退出函数
	g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_main_quit),NULL);
	gtk_widget_set_size_request(window,200,250);
//创建一个纵向的盒子
	box = gtk_vbox_new(FALSE,0);
//将box添加到window中
	gtk_container_add(GTK_CONTAINER(window),box);
//创建一个框架
	frame = gtk_frame_new("字体选项：");
//将框架放入box中
	gtk_box_pack_start(GTK_BOX(box),frame,FALSE,FALSE,5);
//创建一个纵向的盒子	
	box1 = gtk_vbox_new(FALSE,0);
//将盒子添加到框架中
	gtk_container_add(GTK_CONTAINER(frame),box1);
//设置盒子边缘宽度为10
	gtk_container_set_border_width(GTK_CONTAINER(box1),10);
//显示盒子
	gtk_widget_show(box);
//创建一个多选项按钮
	check1 = gtk_check_button_new_with_label("粗体");
//多选项按钮连接on_check_clicked函数
	g_signal_connect(G_OBJECT(check1),"released",G_CALLBACK(on_check_clicked),(gpointer)1);
//将多选项按钮放入盒子BOX1中
	gtk_box_pack_start(GTK_BOX(box1),check1,FALSE,FALSE,3);
//创建一个多选项按钮
        check2 = gtk_check_button_new_with_label("斜体");
//多选项按钮连接on_check_clicked函数
        g_signal_connect(G_OBJECT(check2),"released",G_CALLBACK(on_check_clicked),(gpointer)2);
//将多选项按钮放入盒子BOX1中
        gtk_box_pack_start(GTK_BOX(box1),check2,FALSE,FALSE,3);
//创建一个多选项按钮
        check3 = gtk_check_button_new_with_label("下划线");
//多选项按钮连接on_check_clicked函数
        g_signal_connect(G_OBJECT(check3),"released",G_CALLBACK(on_check_clicked),(gpointer)3);
//将多选项按钮放入盒子BOX1中
        gtk_box_pack_start(GTK_BOX(box1),check3,FALSE,FALSE,3);
//创建一个多选项按钮
        check4 = gtk_check_button_new_with_label("彩色");
//多选项按钮连接on_check_clicked函数
        g_signal_connect(G_OBJECT(check4),"released",G_CALLBACK(on_check_clicked),(gpointer)4);
//将多选项按钮放入盒子BOX1中
        gtk_box_pack_start(GTK_BOX(box1),check4,FALSE,FALSE,3);
//创建一个按钮
	button = gtk_button_new_from_stock(GTK_STOCK_OK);
//按钮连接on_check_clicked函数
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(on_button_clicked),NULL);
	g_signal_connect_swapped(G_OBJECT(button),"clicked",G_CALLBACK(gtk_widget_destroy),window);
//将按钮放入盒子BOX中
	gtk_box_pack_start(GTK_BOX(box),button,FALSE,FALSE,5);
//显示所有控件
	gtk_widget_show_all(window);
//主事件循环
	gtk_main();
//代码返回逻辑值FALSE
	return FALSE;
}
