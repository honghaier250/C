
/*带图像的按钮button.c*/
#include<gtk/gtk.h>

void on_button_clicked(GtkWidget *button,gpointer data)
{
	g_print("按钮 %s",(gchar *)data);
	//g_print("Button %s is pressed.\n",(gchar *)data);
	g_print("被按了一下\n");
}


//创建自己按钮的函数
GtkWidget* create_button1(void)
{
	GtkWidget *box;
	GtkWidget *image;
	GtkWidget *label;	
	GtkWidget *button;	
	//char *title = "红苹果";
	image = gtk_image_new_from_file("apple-red.png");
	label = gtk_label_new("红苹果");//(title);
	//不均匀排放，间隔为2个像素
	box = gtk_vbox_new(FALSE,2);
	//设定容器边框的宽度
	gtk_container_set_border_width(GTK_CONTAINER(box),5);	
	gtk_box_pack_start(GTK_BOX(box),image,FALSE,FALSE,3);
	gtk_box_pack_start(GTK_BOX(box),label,FALSE,FALSE,3);
	gtk_widget_show(image);
	gtk_widget_show(label);

	button = gtk_button_new();
	gtk_container_add(GTK_CONTAINER(button),box);
	gtk_widget_show(box);
	return button;
}

GtkWidget* create_button2(void)
{
	GtkWidget *box;
	GtkWidget *image;
	GtkWidget *label;	
	GtkWidget *button;	
	//char *title = "小蘑菇";
	image = gtk_image_new_from_file("gnome-gmush.png");
	label = gtk_label_new("小蘑菇");//(title);
	box = gtk_hbox_new(FALSE,2);
	//设定容器边框的宽度
	gtk_container_set_border_width(GTK_CONTAINER(box),5);	
	gtk_box_pack_start(GTK_BOX(box),image,FALSE,FALSE,3);
	gtk_box_pack_start(GTK_BOX(box),label,FALSE,FALSE,3);
	gtk_widget_show(image);
	gtk_widget_show(label);

	button = gtk_button_new();
	gtk_container_add(GTK_CONTAINER(button),box);
	gtk_widget_show(box);
	return button;
}

//主函数
int main(int argc, char*argv[])
{
	GtkWidget *window;
	GtkWidget *box;
	GtkWidget *button1;	
	GtkWidget *button2;	
	GtkWidget *button3;	
	GtkWidget *button4;	
	gchar *title = "带图像和快捷键的按钮";
	gchar *b1 = "Red apple";
	gchar *b2 = "Small mushroom";
	gchar *b3 = "Short key";	
	gchar *b4 = "OK";	
	//初始化GTK
	gtk_init(&argc,&argv);
	//创建最上层主窗口
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	//连接信号"delete_event",使得窗口关闭时发生
	g_signal_connect(G_OBJECT(window),"delete_event",G_CALLBACK(gtk_main_quit),NULL);
	//设置窗口标题	
	gtk_window_set_title(GTK_WINDOW(window),title);
	//设定窗口的默认宽高
	//gtk_window_set_default_size(GTK_WINDOW(window),200,300);
	//设定窗口的位置,让窗口总在最前端
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
	//设定容器边框的宽度
	gtk_container_set_border_width(GTK_CONTAINER(window),20);

	//创建盒子
	box = gtk_hbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(window),box);

	//创建按钮
	button1 = create_button1();
	g_signal_connect(G_OBJECT(button1),"clicked",G_CALLBACK(on_button_clicked),(gpointer)b1);
	//从前往后把按钮放入盒子中，参数三表示是否扩展，参数四表示是否填充于前一控件的间隔,参数五表示间隔的像素
	gtk_box_pack_start(GTK_BOX(box),button1,FALSE,FALSE,5);	

	button2 = create_button2();
	g_signal_connect(G_OBJECT(button2),"clicked",G_CALLBACK(on_button_clicked),(gpointer)b2);
	gtk_box_pack_start(GTK_BOX(box),button2,FALSE,FALSE,5);	
	
	//按钮3用到了创建快捷键的函数
	button3 = gtk_button_new_with_mnemonic("快捷键（_H）");
	g_signal_connect(G_OBJECT(button3),"clicked",G_CALLBACK(on_button_clicked),(gpointer)b3);
	gtk_box_pack_start(GTK_BOX(box),button3,FALSE,FALSE,5);	

	//按钮4用到的函数是从系统自带的资源项目中创建按钮，只有一个参数，这个参数在系统中使用了宏定义
	button4 = gtk_button_new_from_stock(GTK_STOCK_OK);
	g_signal_connect(G_OBJECT(button4),"clicked",G_CALLBACK(on_button_clicked),(gpointer)b4);
	gtk_box_pack_start(GTK_BOX(box),button4,FALSE,FALSE,5);	

	//显示所有控件
	gtk_widget_show_all(window);
	gtk_main();
	return FALSE;
}
