/* 按钮盒    buttonbox.c  */
#include <gtk/gtk.h>

//创建按钮函数
GtkWidget * create_button (gint i)
{
	GtkWidget *image;
	GtkWidget *button;
	GtkWidget *vbox;
	GtkWidget *label;
	switch(i)
	{
	case 1:
		//传值1，按钮内容为“是”
		image = gtk_image_new_from_stock(GTK_STOCK_YES,GTK_ICON_SIZE_MENU);
		label = gtk_label_new(" 是");
		break;
	case 2:
		//传值2，按钮内容为“否”
		image = gtk_image_new_from_stock(GTK_STOCK_NO,GTK_ICON_SIZE_MENU);
		label= gtk_label_new("否");
		break;
	case 3:
		//传值3，按钮内容为“帮助”
		image = gtk_image_new_from_stock(GTK_STOCK_HELP,GTK_ICON_SIZE_MENU);
		label=gtk_label_new("帮助");
		break;
	}

	button=gtk_button_new();
	vbox = gtk_vbox_new(FALSE,0);
	//向盒状容器添加控件
	gtk_container_add(GTK_CONTAINER(button),vbox);
	//从前到后依次排列控件
	gtk_box_pack_start(GTK_BOX(vbox),image,FALSE,FALSE,0);
	//image和label是被容纳控件的指针
	gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);
	return button;
}
//创建按钮盒GTK_BUTTONBOX_EDGE,
 GtkWidget * create_button_box(gboolean ish, GtkButtonBoxStyle style, const gchar *title)
{
	GtkWidget *frame;//定义框架
	GtkWidget *button;// 定义按钮
	GtkWidget *bbox;	//定义按钮盒
	frame = gtk_frame_new(title);
	//判断， ish值为真，按钮盒是横向的，否则，按钮盒是纵向的
	if(ish == TRUE)
		bbox = gtk_hbutton_box_new();
	else
		bbox=gtk_vbutton_box_new();
	//设定盒状容器的的子控件间隔
	gtk_box_set_spacing(GTK_BOX(bbox),5);
	//设置按钮盒的布局容器
	gtk_button_box_set_layout(GTK_BUTTON_BOX(bbox),style);
	//设置容器边框的宽度
	gtk_container_set_border_width(GTK_CONTAINER(bbox),5);
	//将另一控件添加到容器中
	gtk_container_add(GTK_CONTAINER(frame),bbox);
	
	//根据上面形参接收的值，创建相应按钮1，2，3
	button=create_button(1); 
	//排列按钮
	gtk_box_pack_start(GTK_BOX(bbox),button,FALSE,FALSE,2);
	
	button=create_button(2);
	gtk_box_pack_start(GTK_BOX(bbox),button,FALSE,FALSE,2);
	
	button=create_button(3);
	gtk_box_pack_start(GTK_BOX(bbox),button,FALSE,FALSE,2);
	return frame;
}

int main(int argc,char *argv[])
{
	GtkWidget *window;//定义窗口
	GtkWidget *vbox;//定义纵向盒子
	GtkWidget *hbox;//定义横向盒子
	GtkWidget *frame;//定义框架
	
	gtk_init(&argc,&argv);
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(G_OBJECT(window),"delete_event",G_CALLBACK(gtk_main_quit),NULL);
	gtk_window_set_title(GTK_WINDOW(window),"按钮盒");
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);

	//分别为横向按钮盒和纵向按钮盒的按钮进行位置设置
	hbox=gtk_hbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(window),hbox);
	vbox=gtk_vbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(hbox),vbox,FALSE,FALSE,2);
	//按钮盒的属性有以下几个值：GTK_BUTTONBOX_EDGE居中，GK_BUTTONBOX_SPREAD紧密，
	//GTK_BUTTONBOX_END尾对齐，_BUTTONBOX_START首对齐，GK_BUTTONBOX_STYLE默认。
	frame=create_button_box(TRUE,GTK_BUTTONBOX_EDGE,"居中");
	gtk_box_pack_start(GTK_BOX(vbox),frame,FALSE,FALSE,2);
	frame=create_button_box(TRUE,GTK_BUTTONBOX_SPREAD,"紧密");
	gtk_box_pack_start(GTK_BOX(vbox),frame,FALSE,FALSE,2);
	frame=create_button_box(TRUE,GTK_BUTTONBOX_END,"尾对齐");
	gtk_box_pack_start(GTK_BOX(vbox),frame,FALSE,FALSE,2);
	frame=create_button_box(TRUE,GTK_BUTTONBOX_START,"首对齐");
	gtk_box_pack_start(GTK_BOX(vbox),frame,FALSE,FALSE,2);
	frame=create_button_box(FALSE,GTK_BUTTONBOX_EDGE,"居中");
	gtk_box_pack_start(GTK_BOX(hbox),frame,FALSE,FALSE,2);
	frame=create_button_box(FALSE,GTK_BUTTONBOX_SPREAD,"紧密");
	gtk_box_pack_start(GTK_BOX(hbox),frame,FALSE,FALSE,2);
	frame=create_button_box(FALSE,GTK_BUTTONBOX_END,"尾对齐");
	gtk_box_pack_start(GTK_BOX(hbox),frame,FALSE,FALSE,2);
	frame=create_button_box(FALSE,GTK_BUTTONBOX_START,"首对齐");
	gtk_box_pack_start(GTK_BOX(hbox),frame,FALSE,FALSE,2);
	
	//显示窗口
	gtk_widget_show_all(window);
	gtk_main();
	return FALSE;
}

















	
