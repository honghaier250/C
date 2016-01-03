#include <gtk/gtk.h>

GtkWidget *create_window(); //创建窗体和控件!!
gboolean button_press(GtkWidget *window,GdkEventButton *event,gint data);

int main(int argc,char *argv[])
{
    GtkWidget *window;
    gtk_init(&argc, &argv); //初始化
    window = create_window(); //创建窗体和控件!!

    gtk_widget_show_all(window); //显示窗体和控件
    gtk_main(); //事件循环处理
    return 0;
}

GtkWidget *create_button(const gchar *button_str)
{
    GtkWidget *button = gtk_button_new_with_label(button_str);
    return button;
}

GtkWidget *create_image(const gchar *path)
{
    GtkWidget *image = gtk_image_new_from_file(path);
    return image;
}

//退出按钮(button2)单击事件函数
void button2_click()
{
    gtk_main_quit();
}

void button1_click()
{
    GtkWidget *dialog; //提示框
    gchar *msg = "科创职业学院计算机网络及应用\n三国软件开发小组 菜鸟\n邮箱:qiu_hai_long@sina.com\nqq:356752238\n";
    GtkMessageType type = GTK_MESSAGE_INFO; //信息消息
    //GTK_DIALOG_MODAL 对话框模式   GTK_BUTTONS_OK 只有一个'OK'按钮
    dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, type, GTK_BUTTONS_OK, msg);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog); //销毁dialog
}

//新建任意布局控件和布局上的控件内容
GtkWidget *create_fixed()
{
    GdkPixbuf *quit_icon=gdk_pixbuf_new_from_file("./finish.png", NULL);
    GtkWidget *quit    =  gtk_image_new_from_pixbuf(quit_icon);
    GtkWidget *fixed   =  gtk_fixed_new(); //新建任意布局
    GtkWidget *button1 =  create_button("               安全登录             ");
    GtkWidget *button2 =  gtk_button_new();
    GtkWidget *text1   =  gtk_entry_new();
    GtkWidget *text2   =  gtk_entry_new();
    GtkWidget *image   =  create_image("login.png");
    GtkWidget *hbox    =  gtk_hbox_new(FALSE, 0);
    GList *login_methods = NULL;
    GList *login_accounts = NULL;
    GtkWidget *combo_methods = NULL;
    GtkWidget *combo_accounts = NULL;
    GtkWidget *check = NULL;

    //向列表里添加数据项
    login_methods = g_list_append(login_methods, "安全登录");
    login_methods = g_list_append(login_methods, "账户登录");
    login_accounts = g_list_append(login_accounts, "贺红杰");

    //创建一个下拉列表框
    combo_methods = gtk_combo_new();
    combo_accounts = gtk_combo_new();

    //设置按钮背景
    gtk_button_set_image(GTK_BUTTON(button2), quit);
    gtk_button_set_image_position(GTK_BUTTON(button2), GTK_POS_TOP);

    //设置按钮样式
    gtk_button_set_relief(GTK_BUTTON(button2), GTK_RELIEF_NONE);
    //设置text2为密码输入 gtk_entry_set_visibility
    //gtk_entry_set_visibility(GTK_ENTRY(text2),FALSE);

    //创建单选框
    //check = gtk_check_button_new_with_label ("自动登录");
    check = gtk_check_button_new_with_label ("");

    //设置控件的 宽和高
    gtk_widget_set_size_request(combo_methods, 140, 26);
    gtk_widget_set_size_request(combo_accounts, 140, 26);
    gtk_widget_set_size_request(text2, 122, 22);
    gtk_widget_set_size_request(button1, 210, 33);

    //添加窗体背景图片
    gtk_fixed_put(GTK_FIXED(fixed), image, 0, 0);
    gtk_fixed_put(GTK_FIXED(fixed), button1, 10, 312);
    gtk_fixed_put(GTK_FIXED(fixed), button2, 200, 3);
    gtk_fixed_put(GTK_FIXED(fixed), combo_methods, 70,208);
    gtk_fixed_put(GTK_FIXED(fixed), combo_accounts, 70,238);
    gtk_fixed_put(GTK_FIXED(fixed), check, 8,290);

    //向下拉列表框添加数据项所在的列表，并在文字录入控件中显示数据项名称
    gtk_combo_set_popdown_strings(GTK_COMBO(combo_methods), login_methods);
    gtk_combo_set_popdown_strings(GTK_COMBO(combo_accounts), login_accounts);

    //退出按钮(button2)单击事件
    g_signal_connect(G_OBJECT(button2),"clicked",G_CALLBACK(button2_click),NULL);
    //确定按钮(button1)单击事件
    g_signal_connect(G_OBJECT(button1),"clicked",G_CALLBACK(button1_click),NULL);
    return fixed;
}

GtkWidget *create_window() //创建窗体和控件!!
{
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *fixed;

    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 224, 160); //窗体大小
    gtk_window_set_title(GTK_WINDOW(window),"精美窗体测试"); //标题栏
    gtk_window_set_decorated(GTK_WINDOW(window),FALSE); //去掉窗口标题栏
    //添加窗体事件掩码 GDK_BUTTON...MASK按下任何按键的时候,接收鼠标运动的事件!!
    gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);


    fixed  = create_fixed(); //新建任意布局控件和布局上的控件内容
    //将fixed任意布局控件添加到最顶层窗体window!!
    gtk_container_add(GTK_CONTAINER(window), fixed);

    //为一个特定的对象(wnidow)链接返回函数(button_press)!!
    g_signal_connect(G_OBJECT(window), "button-press-event", G_CALLBACK(button_press), NULL);
    //为一个特定的对象(window)链接返回函数(gtk_main_quit)!!
    g_signal_connect_swapped(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), G_OBJECT(window));

    return window;
}

//button-pree-event响应函数
gboolean button_press(GtkWidget *widget,GdkEventButton *event,gint data)
{
    if(event->type == GDK_BUTTON_PRESS) //判断鼠标是否被按下
    {
        if(event->button == 1)// 1代表鼠标左键!! 2 鼠标中间那个东东!! 3 就是剩下的那个!!
        {   //gtk_widget_get_toplevel 返回顶层窗口 就是window!!
            gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)), event->button,
                    event->x_root, event->y_root,event->time);
        }
    }
}
/*  gtk_window_begin_move_drag
    移动窗体!!第一个参数: GtkWindow *window
           第二个参数: gint button
          第三个参数: gint root_x
         第四个参数: gint root_y
        第五.....: guint32 timestamp 参考 GdkEventButton 结构体

   GdkEventButton 结构体
   typedef struct {
        GdkEventType type;
        GdkWindow *window;
        gint8 send_event;
        guint32 time;
        gdouble x;
        gdouble y;
        gdouble *axes;
        guint state;
        guint button;
        GdkDevice *device;
        gdouble x_root, y_root;
   } GdkEventButton;
*/
