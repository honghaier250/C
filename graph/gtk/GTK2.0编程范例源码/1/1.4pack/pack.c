/*用盒装容器排列按钮pack.c*/
#include<gtk/gtk.h>

int main(int argc, char*argv[])
{
	GtkWidget *window;
	GtkWidget *box;
	GtkWidget *button1;	
	GtkWidget *button2;	
	GtkWidget *button3;	
	GtkWidget *button4;	
	gchar *title = "排列按钮";
	gtk_init(&argc,&argv);
	//创建最上层主窗口
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	//连接信号"delete_event",使得窗口关闭时发生
	g_signal_connect(G_OBJECT(window),"delete_event",G_CALLBACK(gtk_main_quit),NULL);
	//设置窗口标题	
	gtk_window_set_title(GTK_WINDOW(window),title);
	//设定窗口的位置
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
	//设定容器边框的宽度，否则按钮填充整个窗口
	gtk_container_set_border_width(GTK_CONTAINER(window),20);

	//创建盒子,FALSE表示不均匀排放，0表示无间隔
	//box = gtk_hbox_new(FALSE,0);
	box = gtk_hbox_new(TRUE,0);
	//把盒子放进窗口
	gtk_container_add(GTK_CONTAINER(window),box);	

	//创建一个带有标签的按钮
	button1 = gtk_button_new_with_label("按钮");
	//从前往后把按钮放入盒子中，参数三表示是否横向扩展，参数四表示是否填充于前一控件的间隔,参数五表示按钮间的像素
	gtk_box_pack_start(GTK_BOX(box),button1,TRUE,TRUE,3);
	
	button2 = gtk_button_new_with_label("按");
	gtk_box_pack_start(GTK_BOX(box),button2,FALSE,FALSE,3);
	
	button3  = gtk_button_new_with_label("钮");
	gtk_box_pack_start(GTK_BOX(box),button3,FALSE,FALSE,3);

	button4 = gtk_button_new_with_label("  ");
	gtk_box_pack_start(GTK_BOX(box),button4,FALSE,FALSE,3);
	
	//显示所有控件
	gtk_widget_show_all(window);
	gtk_main();
	return FALSE;

}
