//编译有警告，数据类型不一致，

/*创建菜单的快捷方法itemfact.c*/
#include<gtk/gtk.h>
void on_menu_activate(GtkMenuItem *item, gpointer data);

//定义菜单集
//各参数含义：1，显示在菜单上文字以及文字所在的位置，2，快捷键，3，信号回调函数，
//4，传递给回调函数的参数，5，此项属性，系统固有资源，6，属性的值 
static GtkItemFactoryEntry menu_items[ ] = 
{	{"/文件(_F)",NULL,NULL,0,"<Branch>"},
	{"/文件(_F)/新建",NULL,on_menu_activate,( gpointer)("新建"),"<StockItem>",GTK_STOCK_NEW},
	{"/文件(_F)/打开",NULL,on_menu_activate,( gpointer)("打开"),"<StockItem>",GTK_STOCK_OPEN},
	{"/文件(_F)/保存",NULL,on_menu_activate,( gpointer)("保存"),"<StockItem>",GTK_STOCK_SAVE},
	{"/文件(_F)/另存为",NULL,on_menu_activate,( gpointer)("另存为"),"<StockItem>",GTK_STOCK_SAVE_AS},
	{"/文件(_F)/-",NULL,NULL,0,"<Separator>"},
	{"/文件(_F)/退出",NULL,on_menu_activate,( gpointer)("退出"),"<StockItem>",GTK_STOCK_QUIT},
	{"/编辑(_E)",NULL,NULL,0,"<Branch>"},
	{"/编辑(_E)/剪切",NULL,on_menu_activate,( gpointer)("剪切"),"<StockItem>",GTK_STOCK_CUT},	
	{"/编辑(_E)/复制",NULL,on_menu_activate,( gpointer)("复制"),"<StockItem>",GTK_STOCK_COPY},
	{"/编辑(_E)/粘贴",NULL,on_menu_activate,( gpointer)("粘贴"),"<StockItem>",GTK_STOCK_PASTE},
	{"/编辑(_E)/查找",NULL,on_menu_activate,( gpointer)("查找"),"<StockItem>",GTK_STOCK_FIND},
	{"/帮助(_H)",NULL,NULL,0,"<LastBranch>"},
	{"/帮助(_H)/帮助",NULL,on_menu_activate,( gpointer)("帮助"),"<StockItem>",GTK_STOCK_HELP},
	{"/帮助(_H)/关于。。。",NULL,on_menu_activate,( gpointer)("关于"),NULL},
};

void on_menu_activate(GtkMenuItem *item, gpointer data)
{	
	g_print("菜单项 %s被激活\n",(gchar *) data);
	//g_print("Menuitem is  %s  pressed.\n",(gchar *)data);
}

//主函数
int main(int argc, char*argv[])
{
	GtkWidget *window;
	GtkWidget *box;
	GtkWidget *menubar;	
	GtkAccelGroup* accel_group;	
	GtkItemFactory* item_factory;
	gint n = 15; //数组元素个数

	//初始化GTK
	gtk_init(&argc,&argv);
	//创建最上层主窗口
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	//连接信号"delete_event",使得窗口关闭时发生
	g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_main_quit),NULL);
	//设置窗口标题	
	gtk_window_set_title(GTK_WINDOW(window),"添加菜单的快捷方式");
	//设定窗口的默认宽高
	gtk_window_set_default_size(GTK_WINDOW(window),300,200);
	//设定窗口的位置,让窗口总在最前端
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
	//设定容器边框的宽度
	//gtk_container_set_border_width(GTK_CONTAINER(window),20);

	//创建快捷键集，不可视
	accel_group = gtk_accel_group_new( );

	//创建盒子
	box = gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(window),box);
	gtk_widget_show(box);

	//创建菜单
	item_factory = gtk_item_factory_new(GTK_TYPE_MENU_BAR,"<main>",accel_group);
	//创建菜单项       
	 gtk_item_factory_create_items(item_factory,n,menu_items,NULL);
	//向窗口添加快捷键集
	gtk_window_add_accel_group(GTK_WINDOW(window),accel_group);
	//创建菜单条，取得菜单条的控件指针
	menubar = gtk_item_factory_get_widget(item_factory,"<main>");
	gtk_box_pack_start(GTK_BOX(box),menubar,FALSE,FALSE,0);	
	gtk_widget_show(menubar);
	gtk_widget_show(window);
	
	//显示所有控件
	//gtk_widget_show_all(window);
	gtk_main();
	return FALSE;
}
