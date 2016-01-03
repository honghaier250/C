#include<gtk/gtk.h>
void on_file_select_ok(GtkWidget * button,GtkFileSelection *fs)
{
    GtkWidget * dialog;	//对话框
    gchar message[1024];	//路径缓存
    const gchar * filename;//路径或文件名
    
    //从参数 fs 中获取路径
    filename = gtk_file_selection_get_filename(fs);
    
    //判断是否为目录
    if(g_file_test(filename,G_FILE_TEST_IS_DIR))		
		sprintf(message,"你选择的目录是:%s",filename);	//字符串连接
    else
		sprintf(message,"你选择的文件是:%s",filename);

    dialog = gtk_message_dialog_new(NULL,
	    GTK_DIALOG_DESTROY_WITH_PARENT,
	    GTK_MESSAGE_INFO,
	    GTK_BUTTONS_OK,
	    message,
	    NULL);
    
    gtk_widget_destroy(GTK_WIDGET(fs));	//销毁文件选则对话框 fs
    gtk_dialog_run(GTK_DIALOG(dialog));	//显示dialog对话框并等待按钮,在有按钮按下之后继续
    gtk_widget_destroy(dialog);			//销毁dialog对话框
}

void on_button_clicked(GtkWidget * button,gpointer userdata)
{
    GtkWidget * dialog;
    
    //创建文件选择对话框
    dialog = gtk_file_selection_new("请选择一个文件活目录：");
    gtk_window_set_position(GTK_WINDOW(dialog),GTK_WIN_POS_CENTER);
    gtk_signal_connect(GTK_OBJECT(dialog),"destroy",
	    GTK_SIGNAL_FUNC(gtk_widget_destroy),&dialog);
	
	// 给确定按钮添加回掉函数
    g_signal_connect(
			GTK_OBJECT(GTK_FILE_SELECTION(dialog)->ok_button),
			 "clicked",
		    GTK_SIGNAL_FUNC(on_file_select_ok),
		    GTK_FILE_SELECTION(dialog));
//			G_CALLBACK(on_file_select_ok),
//			GTK_FILE_SELECTION(dialog));

	//给取消按钮添加回掉函数
	gtk_signal_connect_object(
			GTK_OBJECT(GTK_FILE_SELECTION(dialog)->cancel_button),
			"clicked",
			GTK_SIGNAL_FUNC(gtk_widget_destroy),
			GTK_OBJECT(dialog));

	gtk_widget_show(dialog);
}

int main(int argc,char * argv[])
{
	GtkWidget * window;
	GtkWidget * button;

	gtk_init(&argc,&argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(G_OBJECT(window),"delete_event",
			G_CALLBACK(gtk_main_quit),NULL);
	gtk_window_set_title(GTK_WINDOW(window),"选择文件和目录");
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
	gtk_container_set_border_width(GTK_CONTAINER(window),20);

	button = gtk_button_new_with_label("按下此按钮选择文件");
	g_signal_connect(G_OBJECT(button),"clicked",
			G_CALLBACK(on_button_clicked),NULL);
	gtk_container_add(GTK_CONTAINER(window),button);
	
	gtk_widget_show(button);
	gtk_widget_show(window);

	gtk_main();
	return 0;
}
