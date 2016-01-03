/*登录窗口pass.c*/
#include<gtk/gtk.h>

	//定义2个输入控件指针变量
	static GtkWidget* entry1;
	static GtkWidget* entry2;

//点击“确认”按钮的回调函数
void on_button_clicked(GtkWidget* button,gpointer data)
{
	//获取2个输入框的信息
	 const gchar *username = gtk_entry_get_text(GTK_ENTRY(entry1));
	 const gchar *password = gtk_entry_get_text(GTK_ENTRY(entry2));
	//输出2个输入框的信息
	g_print("用户名是:%s",username);
	g_print("\n");
	g_print("密码是:%s",password);
	g_print("\n");
}

//主函数
int main(int argc,char* argv[])
{
	//定义多个窗口控件指针变量
	GtkWidget* window;
	GtkWidget* box;
	GtkWidget* box1;
	GtkWidget* box2;
	GtkWidget* label1;
	GtkWidget* label2;
	GtkWidget* button;
	GtkWidget* sep;

	//初始化
	gtk_init(&argc,&argv);

	//创建一个新窗口
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	//退出程序
	g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_main_quit),NULL);
	//窗口标题
	gtk_window_set_title(GTK_WINDOW(window),"登录窗口");
	//窗口位置
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
	//窗口边框大小
	gtk_container_set_border_width(GTK_CONTAINER(window),20);

	//创建一个纵向盒子，并添加到窗口中
	box = gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(window),box);
	//创建2个横向盒
	box1 = gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(box),box1,FALSE,FALSE,5);
	box2 = gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(box),box2,FALSE,FALSE,5);

	//创建一个横向分隔线，并添加到纵向的盒子中
	sep = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(box),sep,FALSE,FALSE,5);

	//创建一个“用户名”标签
	label1 = gtk_label_new("用户名:");
	//创建一个输入框
	entry1 = gtk_entry_new();
	//按从前到后顺序在一个横向盒子中排列这个标签和输入框
	gtk_box_pack_start(GTK_BOX(box1),label1,FALSE,FALSE,5);
	gtk_box_pack_start(GTK_BOX(box1),entry1,FALSE,FALSE,5);
	//创建一个“密码”标签
	label2= gtk_label_new("密码:");
	entry2 = gtk_entry_new();
	//单行输入控件的显示属性，FALSE表示显示的内容全部是＊
	gtk_entry_set_visibility(GTK_ENTRY(entry2),FALSE);
	gtk_box_pack_start(GTK_BOX(box1),label2,FALSE,FALSE,5);
	gtk_box_pack_start(GTK_BOX(box1),entry2,FALSE,FALSE,5);

	//创建一个“确认”按钮
	button = gtk_button_new_with_label("确认");
	//调用回调函数，显示信息
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(on_button_clicked),NULL);
	//点击"确认"按钮，调用回调函数，退出程序
	g_signal_connect_swapped(G_OBJECT(button),"clicked",G_CALLBACK(gtk_widget_destroy),window);
	//将按钮放在盒子中
	gtk_box_pack_start(GTK_BOX(box),button,FALSE,FALSE,5);

	//显示所用控件
	gtk_widget_show_all(window);
	gtk_main();
	return FALSE;

}
