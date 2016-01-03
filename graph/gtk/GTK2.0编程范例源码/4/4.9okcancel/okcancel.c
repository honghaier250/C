#include <gtk/gtk.h>
/* 点击"确认"按钮的回调函数 */
void on_ok_clicked(GtkButton *button,gpointer data)
{
	g_print("你选择的是确认.\n");
}

/* 点击"取消"按钮的回调函数 */
void on_cancel_clicked(GtkButton *button,gpointer data)
{
	g_print("你选择的是取消.\n");
}

int main(int argc,char *argv[])
{
	GtkWidget *window;
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *bbox;
	GtkWidget *button;
	GtkWidget *label;
	GtkWidget *image;
	GtkWidget *sep;
	/* 初始化命令行参数 */
	gtk_init(&argc,&argv);
	/* 创建一个最上层的主窗口 */
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(G_OBJECT(window),"destory",G_CALLBACK(gtk_main_quit),NULL);
	/* 设置窗口标题 */
	gtk_window_set_title(GTK_WINDOW(window),"确定/取消对话框");
	/* 设置窗口位置在屏幕中央 */                        
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
	/* 设置窗口边框的宽度 */
	gtk_container_set_border_width(GTK_CONTAINER(window),10);
	vbox = gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(window),vbox);
	hbox = gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,5);
	/* 警告图标 */
	image = gtk_image_new_from_stock(GTK_STOCK_DIALOG_WARNING,GTK_ICON_SIZE_DIALOG);
	gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,5);
	label = gtk_label_new("这是一个确定/取消对话框.\n根据选择做出相应的操作.");
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,5);
	/* 分割线 */
	sep = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox),sep,FALSE,FALSE,5);
	/* 按钮盒 */
	bbox = gtk_hbutton_box_new();
	/* 为按钮盒设置布局 */
	gtk_button_box_set_layout(GTK_BUTTON_BOX(bbox),GTK_BUTTONBOX_SPREAD);
	gtk_box_pack_start(GTK_BOX(vbox),bbox,FALSE,FALSE,5);
	/* 创建一个"确定"按钮 */
	button = gtk_button_new_from_stock(GTK_STOCK_OK);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(on_ok_clicked),NULL);
	/* 关闭对话框 */
	g_signal_connect_swapped(G_OBJECT(button),"clicked",G_CALLBACK(gtk_widget_destroy),window);
	gtk_box_pack_start(GTK_BOX(bbox),button,FALSE,FALSE,0);
	
	/* 创建一个"取消"按钮 */
	button = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(on_cancel_clicked),NULL);
	/* 关闭对话框 */
	g_signal_connect_swapped(G_OBJECT(button),"clicked",G_CALLBACK(gtk_widget_destroy),window);	
	gtk_box_pack_start(GTK_BOX(bbox),button,FALSE,FALSE,0);
	/* 显示所有窗体 */
	gtk_widget_show_all(window);
	/* 主事件循环 */
	gtk_main();
	return FALSE;
}
