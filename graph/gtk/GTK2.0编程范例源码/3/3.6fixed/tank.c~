//创建使用自由布局控件，接收和控制键盘的输入，动态更改图像控件的显示内容
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

//定义枚举类型：四个元素分别代表左,上,右,下
enum _FORWARD { LEFT, UP, RIGHT, DOWN };  
//定义方向类型 Forward为枚举类型变量
typedef enum _FORWARD Forward;

static gchar *tank_file[4] = {"0.png","2.png","4.png","6.png"}; //指针数组 放图片
static GtkWidget *fixed;
static GtkWidget *tank_image;
gint i = 75;//显示的横坐标
gint j = 75;//显示的纵坐标
Forward forward = UP; //定义方向

//函数声明 
void move (Forward fw);    
void key_press (GtkWidget *widget,GdkEventKey *event,gpointer data); 

//按键后所用的函数 显示按键后的位置
void move (Forward fw)
{
	switch(fw)
	{
	//按钮"上"的功能
	case UP :
		j = j - 5 ; //纵坐标减少5
		if ( j < 0 ) j = 175 ; //如果纵坐标小于0 则图片在顶端显示
		gtk_image_set_from_file(GTK_IMAGE(tank_image),tank_file[0]);//放入图片
		gtk_fixed_move(GTK_FIXED(fixed),tank_image,i,j);//移动后图片显示及位置
		break;
	//按钮"下"的功能
	case DOWN :
		j = j + 5 ;//纵坐标加5
		if ( j > 200 ) j = 0;//如果纵坐标大于200 则图片在底端显示
		gtk_image_set_from_file(GTK_IMAGE(tank_image),tank_file[2]);
		gtk_fixed_move(GTK_FIXED(fixed),tank_image,i,j);
		break;
	//按钮"左"的功能
	case LEFT :
		i = i - 5 ;//横坐标减少5
		if ( i < 0 ) i = 175 ;//如果横坐标小于0 则图片在最右端显示
		gtk_image_set_from_file(GTK_IMAGE(tank_image),tank_file[3]);
		gtk_fixed_move(GTK_FIXED(fixed),tank_image,i,j);
		break;
	//按钮"右"的功能
	case RIGHT :
		i = i + 5 ;//横坐标加5
		if ( i > 200 )  i = 0 ;//如果横坐标大于200 则图片在最左端显示
		gtk_image_set_from_file(GTK_IMAGE(tank_image),tank_file[1]);
		gtk_fixed_move(GTK_FIXED(fixed),tank_image,i,j);
		break;
	}
}

//按键函数：按键发生后，调用函数
void key_press (GtkWidget *widget,GdkEventKey *event,gpointer data)
{
	switch(event->keyval)
	{
	//按键"上"被按下
	case GDK_Up :
		forward = UP; //令枚举类型变量为UP
		move(forward);//调用函数实现功能
		break;
	//按键"下"被按下
	case GDK_Down :
		forward = DOWN;//令枚举类型变量为DOWN
		move(forward);//调用函数实现功能
		break;
	//按键"左"被按下
	case GDK_Left :
		forward = LEFT;//令枚举类型变量为LEFT
		move(forward);//调用函数实现功能
		break;
	//按键"右"被按下
	case GDK_Right :
		forward = RIGHT;//令枚举类型变量为RIGHT
		move(forward);//调用函数实现功能
		break;
	}
}

//主函数
int main (int argc, char *argv[])
{
	GtkWidget *window; 
	GtkWidget *vbox;
	GtkWidget *bbox;
	GtkWidget *sep;
	GtkWidget *frame;
	GtkWidget *button;
	
	gtk_init(&argc,&argv);
	//设定主窗口
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window)," 坦克游戏");
	gtk_container_set_border_width(GTK_CONTAINER(window),10);
	g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_main_quit),NULL);

	//键盘按键事件	
	g_signal_connect(G_OBJECT(window),"key_press_event",G_CALLBACK(key_press),NULL);
	
	//创建一个盒子后放入窗口中	
	vbox = gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(window),vbox);
	//创建一个无标签框架	
	frame = gtk_frame_new(NULL);
	gtk_frame_set_shadow_type(GTK_FRAME(frame),GTK_SHADOW_ETCHED_OUT);
	
	//创建一个固定窗口并设置窗口大小
	fixed = gtk_fixed_new();
	//
	gtk_fixed_set_has_window(GTK_FIXED(fixed),TRUE);
	//设置固定窗口大小
	gtk_widget_set_size_request(fixed,200,200);
	//设置图片及显示位置	
	tank_image = gtk_image_new_from_file(tank_file[0]);
	gtk_fixed_put(GTK_FIXED(fixed),tank_image,i,j);
	//加入框架中
	gtk_container_add(GTK_CONTAINER(frame),fixed);
	//框架加入盒子中
	gtk_box_pack_start(GTK_BOX(vbox),frame,TRUE,TRUE,5);
	
	//创建一个横向分割器放入盒子中
	sep = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox),sep,FALSE,FALSE,5);
	
	//创建一个横向按钮盒并设置外观放入vbox盒子中 	
	bbox = gtk_hbutton_box_new();
	gtk_button_box_set_layout(GTK_BUTTON_BOX(bbox),GTK_BUTTONBOX_END);
	gtk_box_pack_start(GTK_BOX(vbox),bbox,FALSE,FALSE,5);
	
	//设置退出按键	
	button = gtk_button_new_from_stock(GTK_STOCK_QUIT);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(gtk_main_quit),NULL);
	gtk_box_pack_end(GTK_BOX(vbox),button,FALSE,FALSE,5);
	
	gtk_widget_show_all(window);
	gtk_main();
	return FALSE;
}
	
