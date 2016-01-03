/*添加菜单menu.c*/
//向窗口中添加菜单的一般步骤：
//a.创建菜单条menubar，将菜单条加入到窗口；
//b.创建菜单menu，加入到菜单条中；
//c.创建菜单项menuitem，加入到菜单中。
//前提是先创建一个快捷键集accel_group。
#include<gtk/gtk.h>

void on_menu_activate(GtkMenuItem *item, gpointer data)
{
	g_print("菜单项 %s被激活\n",(gchar *)data);
	//g_print("Menuitem is  %s  pressed.\n",(gchar *)data);
}

//主函数
int main(int argc, char*argv[])
{
	GtkWidget *window;
	GtkWidget *box;
	GtkWidget *menubar;	
	GtkWidget *filemenu;	
	GtkWidget *editmenu;	
	GtkWidget *helpmenu;
	GtkWidget *rootmenu;
	GtkWidget *menuitem;	
	GtkAccelGroup* accel_group;	

	//初始化GTK
	gtk_init(&argc,&argv);
	//创建最上层主窗口
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	//连接信号"delete_event",使得窗口关闭时发生
	g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_main_quit),NULL);
	//设置窗口标题	
	gtk_window_set_title(GTK_WINDOW(window),"菜单测试程序");
	//设定窗口的默认宽高
	gtk_window_set_default_size(GTK_WINDOW(window),300,200);
	//设定窗口的位置,让窗口总在最前端
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
	//设定容器边框的宽度
	//gtk_container_set_border_width(GTK_CONTAINER(window),20);

	//创建快捷键集，不可视
	accel_group = gtk_accel_group_new( );
	gtk_window_add_accel_group(GTK_WINDOW(window),accel_group);

	//创建盒子
	box = gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(window),box);
	
	//创建菜单条
	menubar = gtk_menu_bar_new();
	gtk_box_pack_start(GTK_BOX(box),menubar,FALSE,FALSE,5);	

	//创建菜单
	//文件菜单
	//创建只带文字的菜单项
	rootmenu = gtk_menu_item_new_with_label("文件");
	//把菜单项加到菜单条
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar),rootmenu);
	//创建文件子菜单	
	filemenu = gtk_menu_new();
	//把菜单加到菜单条，设定菜单项的子菜单 
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(rootmenu),filemenu);
		
	//从系统资源中创建带图像的菜单项
	menuitem = gtk_image_menu_item_new_from_stock(GTK_STOCK_NEW, accel_group);	
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu),menuitem);
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(on_menu_activate),(gpointer)("新建"));

	menuitem = gtk_image_menu_item_new_from_stock(GTK_STOCK_OPEN, accel_group);
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu),menuitem);
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(on_menu_activate),(gpointer)("打开"));

	menuitem = gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE, accel_group);
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu),menuitem);
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(on_menu_activate),(gpointer)("保存"));

	menuitem = gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE_AS, accel_group);
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu),menuitem);
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(on_menu_activate),(gpointer)("另存为"));
	//创建菜单中的横线
	menuitem = gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu),menuitem);

	menuitem = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, accel_group);
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu),menuitem);
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(on_menu_activate),(gpointer)("退出"));

	
	//编辑菜单
	rootmenu = gtk_menu_item_new_with_label("编辑");
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar),rootmenu);
	editmenu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(rootmenu),editmenu);
	
	menuitem = gtk_image_menu_item_new_from_stock(GTK_STOCK_CUT, accel_group);
	gtk_menu_shell_append(GTK_MENU_SHELL(editmenu),menuitem);
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(on_menu_activate),(gpointer)("剪切"));

	menuitem = gtk_image_menu_item_new_from_stock(GTK_STOCK_COPY, accel_group);
	gtk_menu_shell_append(GTK_MENU_SHELL(editmenu),menuitem);
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(on_menu_activate),(gpointer)("复制"));

	menuitem = gtk_image_menu_item_new_from_stock(GTK_STOCK_PASTE, accel_group);
	gtk_menu_shell_append(GTK_MENU_SHELL(editmenu),menuitem);
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(on_menu_activate),(gpointer)("粘贴"));

	menuitem = gtk_image_menu_item_new_from_stock(GTK_STOCK_FIND, accel_group);
	gtk_menu_shell_append(GTK_MENU_SHELL(editmenu),menuitem);
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(on_menu_activate),(gpointer)("查找"));

	//帮助菜单
	rootmenu = gtk_menu_item_new_with_label("帮助");
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar),rootmenu);
	helpmenu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(rootmenu),helpmenu);

	menuitem = gtk_image_menu_item_new_from_stock(GTK_STOCK_HELP, accel_group);
	gtk_menu_shell_append(GTK_MENU_SHELL(helpmenu),menuitem);
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(on_menu_activate),(gpointer)("帮助"));

	menuitem = gtk_menu_item_new_with_label("关于。。。");
	gtk_menu_shell_append(GTK_MENU_SHELL(helpmenu),menuitem);
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(on_menu_activate),(gpointer)("关于"));

	//显示所有控件
	gtk_widget_show_all(window);
	gtk_main();
	return FALSE;
}
