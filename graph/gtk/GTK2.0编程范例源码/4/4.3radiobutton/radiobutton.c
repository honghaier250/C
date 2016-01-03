#include<gtk/gtk.h>
static gchar*red="红色";
static gchar*green="绿色";
static gchar*yellow="黄色";
static gchar*blue="蓝色";
void on_radio_clicked(GtkWidget*radio,gint data)
{
	g_print("你选择的颜色是:");
	switch((int)data)
	{
	case 1:
		g_print("%s",red);break;
	case 2:
		g_print("%s",green);break;
	case 3:
		g_print("%s",yellow);break;
	case 4:
		g_print("%s",blue);break;
	}
	g_print("\n");
}
//主函数
int main(int argc,char *argv[])
{
	GtkWidget*	window;
	GtkWidget*	frame;
	GtkWidget*	box;
	GtkWidget*	box1;
	GtkWidget*	button1;
	GtkWidget*	radio1;
	GtkWidget*	radio2;
	GtkWidget*	radio3;
	GtkWidget*	radio4;
      	GSList*  group;
	gchar*  title="单向选择窗口";
	//初始化函数	
	gtk_init(&argc,&argv);
	//创建一个窗口	
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);	
	//设置窗口标题	
	gtk_window_set_title(GTK_WINDOW(window),title);
	//设置窗口的属性
	gtk_container_set_border_width(GTK_CONTAINER(window),10);
	//窗口信号
	g_signal_connect(G_OBJECT(window),"destory",G_CALLBACK(gtk_main_quit),NULL);
	//创建纵一个盒子
	box=gtk_vbox_new(FALSE,0);
	//把盒子放进窗口里	
	gtk_container_add(GTK_CONTAINER(window),box);
	//创建一个框架
	frame=gtk_frame_new("请选择一种颜色");
	gtk_frame_set_shadow_type(GTK_FRAME(frame),GTK_SHADOW_ETCHED_OUT);
	//把盒子放进框架里面	
	gtk_box_pack_start(GTK_BOX(box),frame,FALSE,FALSE,5);
	//创建盒子1
	box1=gtk_hbox_new(FALSE,10);
	//设置盒子的属性
	gtk_container_set_border_width(GTK_CONTAINER(box1),10);
	//把盒子1放进框架里
	gtk_container_add(GTK_CONTAINER(frame),box1);
	//建立下拉列表选项
	radio1=gtk_radio_button_new_with_label(NULL,red);
	//设置列表选项的信号
	 g_signal_connect(G_OBJECT(radio1),"released",G_CALLBACK(on_radio_clicked),(gpointer)1);
	gtk_box_pack_start(GTK_BOX(box1),radio1,FALSE,FALSE,5);
	// 创建多选按钮组
	group=gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio1));
	//建立下拉列表选项
	radio2=gtk_radio_button_new_with_label(group,green);
	//设置列表选项的信号
	 g_signal_connect(G_OBJECT(radio2),"released",G_CALLBACK(on_radio_clicked),(gpointer)2);
	gtk_box_pack_start(GTK_BOX(box1),radio2,FALSE,FALSE,5);
	// 创建多选按钮组
	group=gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio2));
	//建立下拉列表选项
	radio3=gtk_radio_button_new_with_label(group,yellow);
	//设置列表选项的信号
	 g_signal_connect(G_OBJECT(radio3),"released",G_CALLBACK(on_radio_clicked),(gpointer)3);
	gtk_box_pack_start(GTK_BOX(box1),radio3,FALSE,FALSE,5);
	// 创建多选按钮组
	group=gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio3));
		//建立下拉列表选项
	radio4=gtk_radio_button_new_with_label(group,blue);
	//设置列表选项的信号
	 g_signal_connect(G_OBJECT(radio4),"released",G_CALLBACK(on_radio_clicked),(gpointer)4);
	gtk_box_pack_start(GTK_BOX(box1),radio4,FALSE,FALSE,5);
	// 创建多选按钮组
	group=gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio4));
	
	button1=gtk_button_new_from_stock(GTK_STOCK_OK);
	//设置按钮信号
	g_signal_connect(G_OBJECT(button1),"clicked",G_CALLBACK(gtk_main_quit),NULL);
	//把按钮放进盒子里
	gtk_box_pack_start(GTK_BOX(box),button1,FALSE,FALSE,5);
	gtk_widget_show_all(window);
	gtk_main();
	return FALSE;
}
	
	
	

























































	
