/*浮动的工具条和菜单handle.c*/
#include<gtk/gtk.h>

//创建自己的按钮函数
GtkWidget * create_button(char *filename)
{	
	GtkWidget *image;
	GtkWidget *button;
	image = gtk_image_new_from_file(filename);
	gtk_widget_show(image);
	button = gtk_button_new();
	gtk_container_add(GTK_CONTAINER(button),image);
	return button;
}

//主函数
int main(int argc, char*argv[])
{
	GtkWidget *window;
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *box;
	GtkWidget *box1;
	GtkWidget *handle;
	GtkWidget *handle1;
	GtkWidget *menubar;	
	GtkWidget *button1;
	GtkWidget *button2;
	GtkWidget *button3;
	GtkWidget *button4;
	GtkWidget *menu;
	GtkWidget *menuitem;	
	GtkWidget *rootmenu;	
	GtkTooltips *button_tips;
	char *title = "浮动的工具条和菜单";

	//初始化GTK
	gtk_init(&argc,&argv);
	//创建最上层主窗口
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	//连接信号"delete_event",使得窗口关闭时发生
	g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_main_quit),NULL);
	//设置窗口标题	
	gtk_window_set_title(GTK_WINDOW(window),title);
	//设定容器边框的宽度
	gtk_container_set_border_width(GTK_CONTAINER(window),5);
	//设定窗口的默认宽高
	gtk_window_set_default_size(GTK_WINDOW(window),300,200);
	//设定窗口的位置,让窗口总在最前端
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);

	//创建盒子
	vbox = gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(window),vbox);

	hbox = gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,5);

	//创建浮动窗口，放在横向盒子里
	handle = gtk_handle_box_new();
	gtk_box_pack_start(GTK_BOX(hbox),handle,FALSE,FALSE,5);
	
	box = gtk_hbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(handle),box);

	//创建四个带图片的按钮，放在横向盒子里，此盒子放在浮动窗口中
	button1 = create_button("new.png");
	gtk_box_pack_start(GTK_BOX(box),button1,FALSE,FALSE,3);

	button2 = create_button("open.png");
	gtk_box_pack_start(GTK_BOX(box),button2,FALSE,FALSE,3);

	button3 = create_button("save.png");
	gtk_box_pack_start(GTK_BOX(box),button3,FALSE,FALSE,3);

	button4 = create_button("quit.png");
	gtk_box_pack_start(GTK_BOX(box),button4,FALSE,FALSE,3);
	
	//创建使用信息提示
	button_tips = gtk_tooltips_new();	
	//为控件设置提示信息，参数：1提示信息对象本身，2要加提示信息的控件指针
	//	3,4分别是控件可用时和不可用时的提示信息字符串。
	gtk_tooltips_set_tip(GTK_TOOLTIPS(button_tips),button1,"创建一个新文件","New");
	gtk_tooltips_set_tip(GTK_TOOLTIPS(button_tips),button2,"打开一个文件","Open");
	gtk_tooltips_set_tip(GTK_TOOLTIPS(button_tips),button3,"保存当前编辑的文件","Save");
	gtk_tooltips_set_tip(GTK_TOOLTIPS(button_tips),button4,"退出此程序","Quit");

	//创建浮动窗口，放在横向盒子里
	handle1 = gtk_handle_box_new();
	gtk_box_pack_start(GTK_BOX(hbox),handle1,FALSE,FALSE,5);
	
	box1 = gtk_hbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(handle1),box1);

	//创建菜单条
	menubar = gtk_menu_bar_new();
	gtk_box_pack_start(GTK_BOX(box1),menubar,FALSE,FALSE,5);
	//创建菜单
	menu = gtk_menu_new();
	menuitem = gtk_menu_item_new_with_label("菜单项---");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	menuitem = gtk_menu_item_new_with_label("菜单项-");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	rootmenu  = gtk_menu_item_new_with_label("文件");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(rootmenu),menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar),rootmenu);

	//显示所有控件
	gtk_widget_show_all(window);
	gtk_main();
	return FALSE;
}
