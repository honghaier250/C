/*选择日期cale.c*/
//头文件，函数声明，建立确定按钮on_ok_clicked的回调函数
#include <gtk/gtk.h>
void on_ok_clicked(GtkButton * button,gpointer data)
{
//声明变量年月日（此处月份从0开始，即0－11月）
	guint year;
	guint month;
	guint date;
//取得当前用户选择的日期（参数2－4分别代表年月日的整型指针）
	gtk_calendar_get_date(GTK_CALENDAR(data),&year,&month,&date);
//终端输出（你选择的日期是某年某月某日）
	g_print("你选择的日期是%d年%d月%d日\n",year,month+1,date);
}
//声明主函数,定义各控件(窗口，盒子，日历，按钮)
int main ( int argc, char *argv[])
{
	GtkWidget *window;
	GtkWidget *box;
	GtkWidget *calendar;
	GtkWidget *button;
//gtk命令行参数的初始化
	gtk_init(&argc,&argv);
//创建最上层主窗口（200x200大小,返回窗口的控件指针,指针值赋给变量window)
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
//为窗口连接信号"delete_event"要调用的回调函数gtk_main_quit,使得窗口关闭时,退出主循环
	g_signal_connect(G_OBJECT(window),"delete_event",G_CALLBACK(gtk_main_quit),NULL);
//设置窗口标题
	gtk_window_set_title(GTK_WINDOW(window),"选择日期");
//设定窗口的位置,让窗口总在最前端居中
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
//设定容器边框的宽度，默认＝0
	gtk_container_set_border_width(GTK_CONTAINER(window),20);
//创建纵向的盒子构件,FALSE表示不均匀排放，0表示无间隔
	box = gtk_vbox_new(FALSE,0);
//把盒子放进窗口中
	gtk_container_add(GTK_CONTAINER(window),box);
//创建日历控件
	calendar = gtk_calendar_new();
//从前往后把日历控件放入盒子中，参数三表示是否横向扩展，参数四表示是否填充于前一控件的间隔,参数五表示控件间的像素
	gtk_box_pack_start(GTK_BOX(box),calendar,FALSE,FALSE,5);
//创建确定按钮，调用系统自带函数GTK_STOCK_OK
	button = gtk_button_new_from_stock(GTK_STOCK_OK);
//从前往后把按钮放入盒子中，参数三表示是否横向扩展，参数四表示是否填充于前一控件的间隔,参数五表示按钮间的像素
	gtk_box_pack_start(GTK_BOX(box),button,FALSE,FALSE,5);  
//为button连接了一个“clicked”事件要调用的回调函数“on_ok_clicked”
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(on_ok_clicked),calendar);
//显示窗口中所有控件
	gtk_widget_show_all(window);
//gtk程序主循环
	gtk_main();
	return FALSE;
}
