//弹出式菜单
#include <gtk/gtk.h>

//弹出菜单回调函数
static gint my_popup_handler (GtkWidget *widget, GdkEvent *event)
{
	GtkMenu *menu;
	//要弹出菜单的对象
	GdkEventButton *event_button;
	
	g_return_val_if_fail (widget != NULL, FALSE);
	g_return_val_if_fail (GTK_IS_MENU (widget), FALSE);
	g_return_val_if_fail (event != NULL, FALSE);
	
	//转换为菜单
	menu = GTK_MENU (widget);
	//判断是否是此事件
	if (event->type == GDK_BUTTON_PRESS)
	{
		event_button = (GdkEventButton *) event; 
		//判断是否为左右键
		if (event_button->button == 3)
		{      //右键执行操作，第2～5个参数为NULL时表示在鼠标当前位置弹出菜单，
			//第6个参数表示被按下的按钮，最后一个参数是鼠标按下的时间。
			gtk_menu_popup (menu, NULL, NULL, NULL, NULL,event_button->button, event_button->time);
			return TRUE;
		}
	}
	return FALSE;
}

int main (int argc, char* argv[])
{
	GtkWidget *window;
	GtkWidget *box;
	GtkWidget *button;
	GtkWidget *menubar;
	GtkWidget *filemenu;
	GtkWidget *editmenu;
	GtkWidget *helpmenu;
	GtkWidget *rootmenu;
	GtkWidget *menuitem;
	GtkAccelGroup *accel_group;

	gtk_init(&argc, &argv);//初始化
	window = gtk_window_new ( GTK_WINDOW_TOPLEVEL);//创建窗体构件
	gtk_window_set_title ( GTK_WINDOW(window),"弹出式菜单");//设置窗体标题
	gtk_window_set_default_size(GTK_WINDOW(window),300,200);
	g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_main_quit), NULL);

	accel_group = gtk_accel_group_new();
	gtk_window_add_accel_group(GTK_WINDOW(window),accel_group);

	box = gtk_vbox_new ( FALSE, 0);
	gtk_container_add( GTK_CONTAINER( window ), box);

	menubar = gtk_menu_bar_new();
	gtk_box_pack_start(GTK_BOX(box), menubar, FALSE, FALSE, 5);
	
	//文件菜单
	rootmenu = gtk_menu_item_new_with_label("文件");
	filemenu = gtk_menu_new();
	menuitem = gtk_image_menu_item_new_from_stock(GTK_STOCK_NEW, accel_group);
	gtk_menu_shell_append( GTK_MENU_SHELL(filemenu), menuitem);
	menuitem = gtk_image_menu_item_new_from_stock(GTK_STOCK_OPEN,accel_group);
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), menuitem);
	menuitem = gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE, accel_group);
	gtk_menu_shell_append( GTK_MENU_SHELL(filemenu), menuitem);
	menuitem = gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE_AS, accel_group);
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), menuitem);
	//gtk_widget_show(menuitem);
	menuitem = gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), menuitem);
	menuitem = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, accel_group);
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), menuitem);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(rootmenu),filemenu);	
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar),rootmenu);
	
	//编辑菜单
	rootmenu = gtk_menu_item_new_with_label("编辑");
	editmenu = gtk_menu_new();
	menuitem = gtk_image_menu_item_new_from_stock(GTK_STOCK_CUT, accel_group);
	gtk_menu_shell_append(GTK_MENU_SHELL(editmenu),menuitem);
	menuitem = gtk_image_menu_item_new_from_stock(GTK_STOCK_COPY,accel_group);
	gtk_menu_shell_append(GTK_MENU_SHELL(editmenu),menuitem);
	menuitem = gtk_image_menu_item_new_from_stock(GTK_STOCK_PASTE, accel_group);
	gtk_menu_shell_append(GTK_MENU_SHELL(editmenu), menuitem);
	menuitem = gtk_image_menu_item_new_from_stock(GTK_STOCK_FIND,accel_group);
	gtk_menu_shell_append(GTK_MENU_SHELL(editmenu), menuitem);
	//gtk_widget_show(rootmenu);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(rootmenu), editmenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar),rootmenu);
	
	//帮助菜单
	rootmenu =gtk_menu_item_new_with_label("帮助");
	helpmenu = gtk_menu_new();
	menuitem = gtk_image_menu_item_new_from_stock(GTK_STOCK_HELP,accel_group);
	gtk_menu_shell_append(GTK_MENU_SHELL(helpmenu),menuitem);
	menuitem = gtk_menu_item_new_with_label("关于..."); 
	gtk_menu_shell_append(GTK_MENU_SHELL(helpmenu), menuitem);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(rootmenu),helpmenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar),rootmenu);

	//关键－－－弹出式菜单设计
	button = gtk_button_new_with_label("单击右键弹出菜单");
	//弹出的菜单为“文件”菜单的菜单项，主要取决于GTK_OBJECT()函数中的参数，
	//GTK_OBJECT(menu)中的menu 代表的是用gtk_menu_new()函数创建的菜单，文件菜单就是用此函数创建的。
	g_signal_connect_swapped(GTK_OBJECT(button),"button_press_event",G_CALLBACK(my_popup_handler), GTK_OBJECT(filemenu));
	gtk_box_pack_start(GTK_BOX(box),button,FALSE, FALSE, 5);

	gtk_widget_show_all(window);
	gtk_main();
	return FALSE;
}
