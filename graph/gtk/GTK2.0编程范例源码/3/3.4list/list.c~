//列表控件(GtkList)在编程中经常用到，一般表示在多个选项中选择一项或多项//本示例的功能是依次向列表框中加列表项，还可以清除这些列表项

#include <gtk/gtk.h>
static  int i=0;
GtkWidget *list;

void list_add(GtkWidget *widget,gpointer data)
{
	gchar buf[100];
	GtkContainer *container;
	GtkWidget *item;

	sprintf(buf,"列表项%d",i);
	//
	container=GTK_CONTAINER(list);
	//创建一个列表项
	item=gtk_list_item_new_with_label(buf);
	gtk_container_add(container,item);
	gtk_widget_show(item);
	i++;
}

void  list_clear(GtkButton *button,gpointer data)
{	//
	gtk_list_clear_items(GTK_LIST(list),0,-1);
}

int main(int argc,char*argv[])
{
	//控件定义	
	GtkWidget *window;	
	GtkWidget *button;	
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *view;

	gtk_init(&argc,&argv);	
	
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);	
	gtk_window_set_default_size(GTK_WINDOW(window),200,200);
	g_signal_connect(G_OBJECT(window),"delete_event",G_CALLBACK(gtk_main_quit),NULL);
	gtk_window_set_title(GTK_WINDOW(window),"列表控件");	
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
	gtk_container_set_border_width(GTK_CONTAINER(window),10);

	//创建纵向盒子
	vbox=gtk_vbox_new(FALSE,0);	
	gtk_container_add(GTK_CONTAINER(window),vbox);
        
	//创建观察框
	//视区控件GtkViewPort,它也是一个容器(只能容纳一个控件)，在功能上和框架大致相同,主要是美化控件外观的作用
	//两个参数用来描述可调整的边界的值，默认时用NULL代替
	view = gtk_viewport_new(NULL,NULL);	
	gtk_box_pack_start(GTK_BOX(vbox),view,TRUE,TRUE,5);

	//创建列表
	list = gtk_list_new();
	gtk_container_add(GTK_CONTAINER(view),list);
	//设置列表框的选择模式(单选或多选)
	gtk_list_set_selection_mode(GTK_LIST(list),GTK_SELECTION_SINGLE);

	//创建横向的盒子
	hbox = gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,5);

	//创建标签
	button = gtk_button_new_with_label( "插入");
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(list_add),NULL);
	gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,FALSE,5);


	button = gtk_button_new_with_label("清除");
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(list_clear),NULL);
	gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,FALSE,5);

	gtk_widget_show_all(window);
	gtk_main();

	return FALSE;
}
