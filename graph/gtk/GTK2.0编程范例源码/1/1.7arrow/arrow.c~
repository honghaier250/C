/*方向按钮arrow.c*/
#include<gtk/gtk.h>

//创建自己按钮的函数
GtkWidget * create_arrow_button(GtkArrowType arrowtype, GtkShadowType shadowtype )
{	
	GtkWidget *button;	
	GtkWidget *arrow;
	button = gtk_button_new();
	//创建箭头，参数一：箭头方向；参数二：箭头阴影类型
	arrow  = gtk_arrow_new(arrowtype,shadowtype);
	gtk_container_add(GTK_CONTAINER(button),arrow);
	gtk_widget_show(arrow);
	return button;
}

//主函数
int main(int argc, char*argv[])
{
	GtkWidget *window;
	GtkWidget *box;
	GtkWidget *arrow1;	
	GtkWidget *arrow2;	
	GtkWidget *arrow3;	
	GtkWidget *arrow4;	
	gchar *title = "方向按钮";
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
	gtk_widget_show(box);

	//创建带箭头的按钮，调用自定义的函数
	arrow1 = create_arrow_button(GTK_ARROW_LEFT,GTK_SHADOW_IN);
	gtk_box_pack_start(GTK_BOX(box),arrow1,FALSE,FALSE,13);
	gtk_widget_show(arrow1);

	arrow2 = create_arrow_button(GTK_ARROW_UP,GTK_SHADOW_IN);
	gtk_box_pack_start(GTK_BOX(box),arrow2,FALSE,FALSE,13);
	gtk_widget_show(arrow2);

	arrow3 = create_arrow_button(GTK_ARROW_DOWN,GTK_SHADOW_IN);
	gtk_box_pack_start(GTK_BOX(box),arrow3,FALSE,FALSE,13);
	gtk_widget_show(arrow3);

	arrow4 = create_arrow_button(GTK_ARROW_RIGHT,GTK_SHADOW_IN);
	gtk_box_pack_start(GTK_BOX(box),arrow4,FALSE,FALSE,13);
	gtk_widget_show(arrow4);

	gtk_widget_show(window);
	gtk_main();
	return FALSE;
}
