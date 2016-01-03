//头文件
#include <gtk/gtk.h>
//函数声明
int
main (int argc, char* argv[])
{
	//声明窗口控制的指针
	GtkWidget* window;
	//声明盒子控制的指针	
	GtkWidget* vbox;
	//声明一个框架
	GtkWidget* frame;
	//声明一个旋转
	GtkWidget* spin;
	//声明一个标签
	GtkWidget* label;
	//声明一个盒子
	GtkWidget* vvbox;
	//声明4个盒子
	GtkWidget* hbox, * vbox1, * vbox2, * vbox3;
	//定义一个调节的
	GtkAdjustment* adj1;

	//初始化
	gtk_init(&argc, &argv);

	//创建窗口
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	//设置窗口的标题
	gtk_window_set_title(GTK_WINDOW(window), "滚动按钮");
	//为信号连接退出函数
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	//为容器设置边缘宽度
	gtk_container_set_border_width(GTK_CONTAINER(window),10);
	//创建一个vbox盒子
	vbox = gtk_vbox_new(FALSE,0);
	//把盒子添加入窗口
	gtk_container_add(GTK_CONTAINER(window),vbox);

	//创建一个框架
	frame = gtk_frame_new("类似一个日历");
	//把框架放到vbox中
	gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 5);
	
	//创建一个横向的不规则排列的盒子
	hbox = gtk_hbox_new(FALSE, 0);
	//把横向的盒子放到框架里
	gtk_container_add(GTK_CONTAINER(frame), hbox);
	//为盒子设置边缘宽度
	gtk_container_set_border_width(GTK_CONTAINER(hbox),10);
	
	//创建一个纵向的规则排列的盒子
	vbox1 = gtk_vbox_new(TRUE,0);
	//把vbox1放入hbox里
	gtk_box_pack_start(GTK_BOX(hbox), vbox1, TRUE, TRUE, 5);

	//创建一个标签
	label = gtk_label_new("年: ");
	//把标签放入盒子中
	gtk_box_pack_start(GTK_BOX(vbox1), label, FALSE, FALSE, 2);

	//创建一个滚轮的按钮
	spin = gtk_spin_button_new_with_range(1900, 2100, 1);
	//把旋转按钮放入盒子中
	gtk_box_pack_start(GTK_BOX(vbox1), spin, FALSE, FALSE, 2);

	//创建一个盒子
	vbox2 = gtk_vbox_new(TRUE, 0);
	//把vbox2放入hbox中
	gtk_box_pack_start(GTK_BOX(hbox), vbox2, TRUE, TRUE, 5);

	//创建一个标签
	label = gtk_label_new("月: ");
	//把标签放入盒子中
	gtk_box_pack_start(GTK_BOX(vbox2), label, FALSE, FALSE, 2);

	//创建一个旋转按钮
	spin = gtk_spin_button_new_with_range(1, 12, 1);
	//把旋转按钮加入盒子中
	gtk_box_pack_start(GTK_BOX(vbox2), spin, FALSE, FALSE, 2);

	//创建一个盒子	
	vbox3 = gtk_vbox_new(TRUE, 0);
	//把vbox3放入hbox里
	gtk_box_pack_start(GTK_BOX(hbox), vbox3, TRUE, TRUE, 5);

	//创建一个标签
	label = gtk_label_new("日: ");
	//把标签放入vbox3中
	gtk_box_pack_start(GTK_BOX(vbox3), label, FALSE, FALSE, 2);

	//创建一个旋转按钮
	spin = gtk_spin_button_new_with_range(1, 31, 1);
	//把旋转按钮加入盒子中
	gtk_box_pack_start(GTK_BOX(vbox3), spin, FALSE, FALSE, 2);

	//创建一个框架
	frame = gtk_frame_new(NULL);
	//把框架放到盒子里
	gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 5);

	//创建一个盒子
	vvbox = gtk_vbox_new(FALSE, 0);
	//把盒子添加到框架中
	gtk_container_add(GTK_CONTAINER(frame), vvbox);
	//设置窗口边缘宽度
	gtk_container_set_border_width(GTK_CONTAINER(vvbox), 10);

	//创建一个标签
	label = gtk_label_new("第一个滚动按钮,\n显示整数, 范围: 0 - 100");
	//创建一个调节
	adj1 = (GtkAdjustment*) gtk_adjustment_new(50.0, 0.0, 100.0, 1.0, 5.0, 5.0);
	//创建一个滚动按钮
	spin = gtk_spin_button_new(adj1, 5, 1);
	//把标签加入盒子
	gtk_box_pack_start(GTK_BOX(vvbox), label, FALSE, FALSE, 3);
	//把滚动按钮加入盒子
	gtk_box_pack_start(GTK_BOX(vvbox), spin, FALSE, FALSE, 3);
	
	//创建一个标签
	label = gtk_label_new("第二个滚动按钮,\n显示浮点数, 范围: 0.1 - 9.9");
	//创建一个旋转按钮
	spin = gtk_spin_button_new_with_range(0, 9.9, 0.1);
	//把标签加入盒子
	gtk_box_pack_start(GTK_BOX(vvbox), label, FALSE, FALSE, 3);
	//把滚轮按钮加入盒子
	gtk_box_pack_start(GTK_BOX(vvbox), spin, FALSE, FALSE, 3);

	//显示全部窗口	
	gtk_widget_show_all(window);
	//主事件循环
	gtk_main();
	//代码返回逻辑值FALSE
	return FALSE;
}
	
