/*在输入栏输入一个命令，单击执行按钮可在终端执行该命令*/
#include<gtk/gtk.h>
static GtkWidget *entry; 	//文本输入构件
void on_button_clicked(GtkButton *button,gpointer data)
{
	const gchar *command;
	command=gtk_entry_get_text(GTK_ENTRY(entry));		//获取文本输入构件的内容
	system(command);		//执行command内容
}
int main (int argc,char *argv[])
{
	GtkWidget *window;
	GtkWidget *hbox;
	GtkWidget *label;
	GtkWidget *button;  	//窗口，盒子，标签，按钮
		gtk_init(&argc,&argv);		//初始化
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);		//创建最上层主窗口
	g_signal_connect(G_OBJECT(window),"delete_event",G_CALLBACK(gtk_main_quit),NULL);	//退出键
	gtk_window_set_title(GTK_WINDOW(window),"命令窗口");	//设置窗口标题
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);		//设定窗口的位置
	hbox=gtk_hbox_new(FALSE,0); 	//创建盒子
	gtk_container_add(GTK_CONTAINER(window),hbox);	//把盒子放进窗口
	label=gtk_label_new("输入命令：");		//创建一个标签
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,5);	//把标签放入盒子中
	entry=gtk_entry_new();	//创建文本输入构件
	gtk_box_pack_start(GTK_BOX(hbox),entry,TRUE,TRUE,5);		//把文本输入构件放入盒子
		button=gtk_button_new_with_label("执行");		//创建一个带有标签的按钮
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(on_button_clicked),NULL);	//回调函数
	gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,FALSE,5);	//把按钮放入盒子中
	gtk_widget_show_all(window); 	//显示所有控件
	gtk_main();
	return FALSE;
}
	
	
