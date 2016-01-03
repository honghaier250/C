#include <gtk/gtk.h>
int main( int argc , char *argv[])
{
	//	声明窗口控件的指针
	GtkWidget* window;
	//声明分割面板容器控件
	GtkWidget* paned;
	GtkWidget* paned1;
	//声明按钮控件指针
	GtkWidget* button1;
	GtkWidget* button2;
	GtkWidget* button3;

	//初始化
	gtk_init(&argc,&argv);

	//创建新的窗口
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window),"分割面板测试程序");
	g_signal_connect(G_OBJECT(window),"destory",G_CALLBACK(gtk_main_quit),NULL);

	//横向分割板
	paned = gtk_hpaned_new();
	gtk_container_add(GTK_CONTAINER(window),paned);
	gtk_widget_show(paned);
	button1 = gtk_button_new_with_label("按钮一");
	//add1 表示向第一个容器添加 即向横向左侧 或纵向上部
	gtk_paned_add1(GTK_PANED(paned),button1);
	gtk_widget_show(button1);
	
	///纵向分割板
	paned1 = gtk_vpaned_new();
	gtk_paned_add2(GTK_PANED(paned),paned1);
	gtk_widget_show(paned1);

	button2 = gtk_button_new_with_label("按钮二");
	gtk_paned_add1(GTK_PANED(paned1),button2);	
	gtk_widget_show(button2);

	button3 = gtk_button_new_with_label("按钮三");
	//add2  向第二个容器中添加    即纵向或下部
	gtk_paned_add2(GTK_PANED(paned1),button3);
	gtk_widget_show(button3);

	gtk_widget_show(window);
	gtk_main();
	return FALSE;
}














