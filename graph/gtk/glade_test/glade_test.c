 #include <gtk/gtk.h>

 int main(int argc,char *argv[])
 {
	//1.gtk初始化
	gtk_init(&argc,&argv);

	//2.创建GtkBuilder对象，GtkBuilder在<gtk/gtk.h>声明
	GtkBuilder *builder = gtk_builder_new();

	//3.读取test.glade文件的信息，保存在builder中
	if ( !gtk_builder_add_from_file(builder,"test.glade", NULL)) {
		printf("connot load file!");
	}

	//4.获取窗口指针，注意"window1"要和glade里面的标签名词匹配
	GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder,"window1"));

	GtkButton *button = GTK_BUTTON(gtk_builder_get_object(builder, "button1"));


	const char *text = gtk_button_get_label(button);
	printf("text=%s\n", text);


	gtk_widget_show_all(window);

	gtk_main();

	return 0;
 }
