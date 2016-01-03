/*创建splash窗口splash.c*/
#include<gtk/gtk.h>

int main(int argc, char*argv[])
{
	GtkWidget *window;
	GtkWidget *vbox;
	GtkWidget *image;		
	GtkWidget *button;

	//初始化GTK
	gtk_init(&argc,&argv);
	//创建无边框窗口
	window = gtk_window_new(GTK_WINDOW_POPUP);
	//连接信号"delete_event",使得窗口关闭时发生
	g_signal_connect(G_OBJECT(window),"delete_event",G_CALLBACK(gtk_main_quit),NULL);
	//设置窗口标题	
	gtk_window_set_title(GTK_WINDOW(window),"splash窗口");
	//设定窗口的默认宽高
	gtk_window_set_default_size(GTK_WINDOW(window),300,200);
	//设定容器边框的宽度
	gtk_container_set_border_width(GTK_CONTAINER(window),20);
	//设定窗口的位置,让窗口总在最前端
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
	
	//创建盒子
	vbox = gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(window),vbox);
	//创建图像
	image = gtk_image_new_from_file("gnome-splash.png");
	gtk_box_pack_start(GTK_BOX(vbox),image,TRUE,TRUE,0);
	//创建按钮
	button = gtk_button_new_with_label("splash窗口");
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(gtk_main_quit),NULL);
	gtk_box_pack_start(GTK_BOX(vbox),button,FALSE,FALSE,0);

	gtk_widget_show_all(window);
	gtk_main();
	return FALSE;
}
