//动态菜单操作addmenu.c
#include<gtk/gtk.h>

gint i = 2;
static GtkWidget *menu;
static GtkWidget *item;

//添加菜单函数
void add_menu (GtkButton *button,gpointer data)
{
	//定义一个名为newitem的字符型数组，长度为1024，存储菜单数目
	gchar newitem[1024];
	//定义整型变量len,用来记录菜单数目
	gint len;
	//添加菜单时，用g_list_length取得菜单项的数量，再根据数量创建新的菜单项，显示出后用函数gtk_menu_shell_append来向菜单添加就可以。
	//菜单控件的基础对象GTK_MENU_SHELL中有一个双向链表类型(GList)的指针children，他是用来容纳菜单的子控件指针的。
	//用相关函数来对他进行操作，就可以得出某一菜单的菜单项数或具体某一菜单项。
	len = g_list_length(GTK_MENU_SHELL(menu)->children);
	//把len＋1的值赋给newitem
	sprintf(newitem,"菜单项%d",len+1);
	//创建一个带标签的菜单项
	item = gtk_menu_item_new_with_label(newitem);
	//菜单项添加到菜单
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),item);
	//显示菜单项	
	gtk_widget_show(item);
	i++;
}

//删除菜单函数
void remove_menu(GtkButton *button,gpointer data)
{	//建立一个链表	
	GList *list=NULL;
	if(i==0)
		 return ;
	list = g_list_last(GTK_MENU_SHELL(menu)->children);
	GTK_MENU_SHELL(menu)->children = g_list_remove(GTK_MENU_SHELL(menu)->children,list->data);
	i--;
}

//主函数
int main (int argc, char *argv[])
{
	GtkWidget* window;
	GtkWidget* vbox;
	GtkWidget* hbox;
	GtkWidget* button;
	GtkWidget* rootmenu,*menubar;

	//初始化，在GTK＋2.0程序中，都必须用这一函数来初始化		
	gtk_init(&argc,&argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window),"动态添加菜单");
	gtk_window_set_default_size(GTK_WINDOW(window),400,300);
	gtk_container_set_border_width(GTK_CONTAINER(window),5);
	g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_main_quit),NULL);

	//创建一个纵向盒子
	vbox = gtk_vbox_new(FALSE,0);
	//把盒子放到窗体里
	gtk_container_add(GTK_CONTAINER(window),vbox);

	//创建一个菜单栏
	menubar = gtk_menu_bar_new();
	//把菜单栏放到盒子里
	gtk_box_pack_start(GTK_BOX(vbox),menubar,FALSE,FALSE,5);
	//创建一个新菜单
	menu = gtk_menu_new();
	//创建菜单项
	rootmenu = gtk_menu_item_new_with_label("根菜单");
	//将菜单绑定到根菜单项		
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(rootmenu),menu);
	//把根菜单放到菜单栏
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar),rootmenu);
	//创建根菜单里的子菜单项
	item = gtk_menu_item_new_with_label("菜单项 1");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),item);
	item = gtk_menu_item_new_with_label("菜单项 2");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),item);
	
	//创建一个横向盒子
	hbox = gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,5);

	//创建一个带标签的按钮		
	button = gtk_button_new_with_label("添加");
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(add_menu),NULL);
	gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,5);
		
	//创建一个带标签的按钮
	button = gtk_button_new_with_label("删除");
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(remove_menu),NULL);
	gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,5);
		
	//显示所有窗口		
	gtk_widget_show_all(window);
	//主事件循环	
	gtk_main();
	
	return FALSE;
}


