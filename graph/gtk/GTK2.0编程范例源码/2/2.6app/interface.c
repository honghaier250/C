#include<gtk/gtk.h>
#include"callbacks.h"

//对创建菜单，创建工具条函数进行声明
//这两个函数被前一个函数引用，外部没有用到，所以在interface.h中只声明了一个窗口创建函数
GtkWidget *create_menu(GtkAccelGroup *accel_group,GtkWidget *window);
GtkWidget *create_toolbar(GtkWidget *window);

//创建窗口函数
GtkWidget *create_window(void)
{
	//声明窗口控件指针
	GtkWidget *window;
	//声明文本指针
	GtkWidget *text;
	//声明滚动条指针
	GtkWidget *scrolledwin;
	//声明盒子指针
	GtkWidget *box;
	//声明状态指针
	GtkWidget *statusbar;
	//声明菜单条指针
	GtkWidget *menubar;
	//声明工具条指针
	GtkWidget *toolbar;
	//声明快捷键组指针
	GtkAccelGroup *accel_group;

	//创建窗口，窗口的类型为最上层的主窗口
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window),"完整的应用程序窗口");
	gtk_window_set_default_size(GTK_WINDOW(window),400,300);

	//创建快捷键组
	accel_group=gtk_accel_group_new();
	gtk_window_add_accel_group(GTK_WINDOW(window),accel_group);
	
	//创建一个纵向的盒子。参数1：表示是否均匀排放，参数2：容器中控件有无间隔
	box=gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(window),box);
	
	//创建菜单
	menubar=create_menu(accel_group,window);
	//按照从前到后的顺序依次排放菜单条控件。参数1：要容纳控件的容器对象。参数2：被容纳控件的指针。参数3：是否扩展。参数4：是否添充
	gtk_box_pack_start(GTK_BOX(box),menubar,0,0,0);
	//创建工具条
	toolbar=create_toolbar(window);
	//按照从前到后的顺序排列工具条控件
	gtk_box_pack_start(GTK_BOX(box),toolbar,0,1,0);
	//创建状态栏
	statusbar=gtk_statusbar_new();
	//从前到后的顺序排放状态栏控件
	gtk_box_pack_start(GTK_BOX(box),statusbar,FALSE,FALSE,0);

	//创建滚动条窗口
	scrolledwin=gtk_scrolled_window_new(NULL,NULL);	//按照从前到后的顺序排放滚动窗口控件
	gtk_box_pack_start(GTK_BOX(box),scrolledwin,TRUE,TRUE,0);
	//创建文本窗口
	text=gtk_text_view_new();
	//将文本窗口添加到滚动条窗口中
	gtk_container_add(GTK_CONTAINER(scrolledwin),text);
	//设置文本窗口是可编辑的
	gtk_text_view_set_editable(GTK_TEXT_VIEW(text),TRUE);

	//显示所有的窗口
	gtk_widget_show_all(window);
	//返回窗口
	return window;
}

//创建菜单函数
GtkWidget *create_menu(GtkAccelGroup *accel_group,GtkWidget *window)
{	//声明菜单条指针
	GtkWidget *menubar;
	//声明菜单指针
	GtkWidget *menu;
	//声明编辑菜单指针
	GtkWidget *editmenu;
	//声明帮助菜单指针
	GtkWidget *helpmenu;
	//声明菜单项目指针
	GtkWidget *menuitem;
	//声明根菜单指针
	GtkWidget *rootmenu;

	//创建菜单
	menu=gtk_menu_new();
	//从系统资源中创建带图像的菜单项
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_NEW,accel_group);
	//向菜单条中添加菜单项
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	//信号连接
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(on_file_new_activate),NULL);
	
	//从系统资源中创建带图像的菜单项
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_OPEN,accel_group);
	//向菜单中添加菜单项
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	//信号连接
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(on_file_open_activate),NULL);
	//从系统资源中创建带有图像的菜单项
        menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE,accel_group);
	//向菜单中添加菜单项
        gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	//从系统资源中创建带有图像的菜单项
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE_AS,accel_group);
	//向菜单中添加菜单项
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	//创建分隔菜单项
	menuitem=gtk_separator_menu_item_new();
	//向菜单中添加菜单项
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	//从系统资源中创建带有图像的菜单项
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT,accel_group);
	//向菜单中添加菜单项
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	//信号连接
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(on_window_delete_event),NULL);
	//创建带文字“文件”的菜单项
	rootmenu=gtk_menu_item_new_with_label("文件");
	//显示根菜单
	gtk_widget_show(rootmenu);
	//设定菜单项的子菜单
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(rootmenu),menu);
	//创建菜单条
	menubar=gtk_menu_bar_new();
	//向菜单中添加根菜单
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar),rootmenu);
	//创建带有文字“”编辑“的菜单项
	rootmenu=gtk_menu_item_new_with_label("编辑");
	//创建编辑菜单
	editmenu=gtk_menu_new();
	//从系统资源中创建带有图像的菜单项
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_CUT,accel_group);
	//向菜单中添加菜单项
	gtk_menu_shell_append(GTK_MENU_SHELL(editmenu),menuitem);
	//信号连接
        g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(on_edit_cut_activate),NULL);
	//创建带有图像的菜单项
        menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_COPY,accel_group);
	//向菜单中添加菜单项
        gtk_menu_shell_append(GTK_MENU_SHELL(editmenu),menuitem);
	//信号连接
        g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(on_edit_copy_activate),NULL);
	//从系统资源中创建带有图像的菜单项
        menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_PASTE,accel_group);
	//向编辑菜单中添加菜单项
        gtk_menu_shell_append(GTK_MENU_SHELL(editmenu),menuitem);
	//信号连接
        g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(on_edit_paste_activate),NULL);
	//从系统资源中创建带有图像的菜单项
        menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_FIND,accel_group);
	//向菜单中添加菜单项
        gtk_menu_shell_append(GTK_MENU_SHELL(editmenu),menuitem);
	//信号连接
  	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(on_edit_find_activate),NULL);
	//显示根菜单
	gtk_widget_show(rootmenu);
	//设定根菜单中的子菜单
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(rootmenu),editmenu);
	//向菜单条中添加根菜单
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar),rootmenu);
	//创建带有文字“帮助”的菜单项
	rootmenu=gtk_menu_item_new_with_label("帮助");
	//创建帮助菜单
	helpmenu=gtk_menu_new();
	//从系统资源中添加带有图像的菜单项
	menuitem=gtk_image_menu_item_new_from_stock(GTK_STOCK_HELP,accel_group);
	//向帮助菜单中添加菜单项
	gtk_menu_shell_append(GTK_MENU_SHELL(helpmenu),menuitem);
	//信号连接
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(on_help_help_activate),NULL);
	//创建带有文字“关于”的菜单项
	menuitem=gtk_menu_item_new_with_label("关于");
	//向帮助菜单中添加菜单项
	gtk_menu_shell_append(GTK_MENU_SHELL(helpmenu),menuitem);
	//信号连接
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(on_help_about_activate),NULL);
	//设定帮助菜单的子菜单项
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(rootmenu),helpmenu);
	//向菜单中添加根菜单
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar),rootmenu);
	//返回菜单条
	return menubar;
}

//创建工具条函数
GtkWidget *create_toolbar(GtkWidget *window)
{
//声明工具条指针
	GtkWidget *toolbar;
//创建工具条
	toolbar=gtk_toolbar_new();
//向工具条中插入固有资源项目。参数1：添加控件的工具条。参数2：系统固有的资源项目。参数3，4：指出按钮可用和不可用时提示的信息。参数5：指出回调函数。参数6：插入的位置
	gtk_toolbar_insert_stock(GTK_TOOLBAR(toolbar),GTK_STOCK_NEW,"创建一个新文件","新建",GTK_SIGNAL_FUNC(on_file_new_activate),window,-1);
	gtk_toolbar_insert_stock(GTK_TOOLBAR(toolbar),GTK_STOCK_OPEN,"打开一个文件","打开",GTK_SIGNAL_FUNC(on_file_open_activate),toolbar,-1);
	gtk_toolbar_insert_stock(GTK_TOOLBAR(toolbar),GTK_STOCK_SAVE,"保存当前文件","保存",GTK_SIGNAL_FUNC(on_file_save_activate),toolbar,-1);
//返回工具条
	return toolbar;
}

