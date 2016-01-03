/*条件菜单 optionmenu.c*/
#include<gtk/gtk.h>
static GtkWidget*entry;

void on_menu_changed(GtkOptionMenu* option,gpointer data)
{
	gchar buf[20];
	gint i;
	//用来返回条件菜单当前被选择的菜单索引值
	i=gtk_option_menu_get_history(option);
	sprintf(buf,"菜单项 %d",i+1);
	gtk_entry_set_text(GTK_ENTRY(entry),buf);
}

int main(int argc, char* argv[])
{
	GtkWidget* window;
	GtkWidget* option;	
	GtkWidget* hbox;
	GtkWidget* menu;
	GtkWidget* item;
	gint i;
	gchar buf[20];
	gtk_init(&argc,&argv);
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window),"条件菜单");
	g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_main_quit),NULL);
	gtk_container_set_border_width(GTK_CONTAINER(window),20);

	//建立一个新的盒子
	hbox=gtk_hbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(window),hbox);

	//创建单行文本录用控件
	entry=gtk_entry_new();
	//设定单行文本录入控件的文本内容
	gtk_entry_set_text(GTK_ENTRY(entry),"菜单项 1");
	//盒子里面从前到后依次排列控件
	gtk_box_pack_start(GTK_BOX(hbox),entry,FALSE,FALSE,5); 

	//建立一个新的菜单
	menu=gtk_menu_new();
	for(i=1;i<9;i++)
	{	
	       //显示菜单项
		sprintf(buf,"菜单项 %d",i);
		//创建一个带标签的菜单项
		item=gtk_menu_item_new_with_label(buf);
		//把菜单项添加到菜单中
		gtk_menu_shell_append(GTK_MENU_SHELL(menu),item);
	}

	//创建一个空的条件菜单
	option=gtk_option_menu_new();
	//设定菜单项的子菜单项
	gtk_option_menu_set_menu(GTK_OPTION_MENU(option),menu);
	//调用回调函数
	g_signal_connect(G_OBJECT(option),"changed",G_CALLBACK(on_menu_changed),NULL);
	//按顺序从前到后排列横向盒子的依此排列控件
	gtk_box_pack_start(GTK_BOX(hbox),option,FALSE,FALSE,10);
	//显示所有控件
	gtk_widget_show_all(window);
	//主事件循环	
	gtk_main();
	return FALSE;
}

