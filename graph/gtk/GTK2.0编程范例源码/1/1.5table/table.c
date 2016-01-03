/*格状容器table.c*/
#include<gtk/gtk.h>

int main(int argc, char*argv[])
{
	GtkWidget *window;
	GtkWidget *table;
	GtkWidget *button1;	
	GtkWidget *button2;	
	GtkWidget *button3;	
	GtkWidget *button4;	
	GtkWidget *button5;	
	GtkWidget *button6;	
	GtkWidget *frame;	
	
	//初始化GTK
	gtk_init(&argc,&argv);
	//创建最上层主窗口
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	//连接信号"delete_event",使得窗口关闭时发生
	g_signal_connect(G_OBJECT(window),"delete_event",G_CALLBACK(gtk_main_quit),NULL);
	//设置窗口标题	
	gtk_window_set_title(GTK_WINDOW(window),"格状容器");
	//设定窗口的默认宽高
	gtk_window_set_default_size(GTK_WINDOW(window),200,300);
	//设定窗口的位置,让窗口总在最前端
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
	//设定容器边框的宽度
	gtk_container_set_border_width(GTK_CONTAINER(window),20);

	//创建框架
	frame = gtk_frame_new("请注意下列按钮的排列");
	//把框架放进窗口
	gtk_container_add(GTK_CONTAINER(window),frame);	

	//创建格状容器，行数、列数、是否均匀分配
	table= gtk_table_new(4,4,FALSE);
        //设定容器边框的宽度
	gtk_container_set_border_width(GTK_CONTAINER(table),10);
	//为所有的行和列之间设置相同的空白，均为5个像素
	gtk_table_set_row_spacings(GTK_TABLE(table),5);
	gtk_table_set_col_spacings(GTK_TABLE(table),5);
	//把格状容器放进框架
	gtk_container_add(GTK_CONTAINER(frame),table);	

	//创建一个带有标签的按钮
	button1 = gtk_button_new_with_label("0,1,0,1");
	//把按钮放入格状容器，“0，1，0，1”表示四个坐标，从左往右，从上到下，GTK_FILL：可填充，“0，0”间隔距离
	gtk_table_attach(GTK_TABLE(table),button1,0,1,0,1,GTK_FILL,GTK_FILL,0,0);
	
	button2 = gtk_button_new_with_label("1,3,1,3");
	gtk_table_attach(GTK_TABLE(table),button2,1,3,1,3,GTK_FILL,GTK_FILL,0,0);

	button3 = gtk_button_new_with_label("0,1,1,3");
	//简写，默认间隔为0
	gtk_table_attach_defaults(GTK_TABLE(table),button3,0,1,1,3);

	button4 = gtk_button_new_with_label("1,3,0,1");
	gtk_table_attach_defaults(GTK_TABLE(table),button4,1,3,0,1);

	button5= gtk_button_new_with_label("0,4,3,4");
	gtk_table_attach_defaults(GTK_TABLE(table),button5,0,4,3,4);

	button6= gtk_button_new_with_label("3,4,0,3");
	gtk_table_attach_defaults(GTK_TABLE(table),button6,3,4,0,3);

	//显示所有控件
	gtk_widget_show_all(window);
	gtk_main();
	return FALSE;

}
