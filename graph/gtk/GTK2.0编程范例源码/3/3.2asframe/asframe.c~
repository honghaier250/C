//实现规范的框架，使用规范框架控件，设定框架控件的外观、标题等属性
#include<gtk/gtk.h>

int main(int argc, char*argv[])
{
	//定义各控件
	GtkWidget *window;
	GtkWidget *frame;
	GtkWidget *asframe;
	GtkWidget *vbox;
	GtkWidget *label;
	GtkWidget *image;

	//初始化、创建窗口，设置窗口属性
	gtk_init(&argc,&argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(G_OBJECT(window),"delete_event",G_CALLBACK(gtk_main_quit),NULL);
	gtk_window_set_title(GTK_WINDOW(window),"不同样式的框架");
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
	gtk_container_set_border_width(GTK_CONTAINER(window),20);
	
	//创建盒子构件，把盒子放到窗口中
	vbox = gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(window),vbox);
	
	//创建框架构件
	frame = gtk_frame_new("框架一");
	gtk_box_pack_start(GTK_BOX(vbox),frame,FALSE,FALSE,5);
	//设定框架控件的外观轮廓，第二个参数为GtkShadowType类型，类型共有5个值：GTK_SHADOW_OUT（外向型的外框线）GTK_SHADOW_IN（内向型的外框线），
	//GTK_SHADOW_NONE（无外框线），GTK_SHADOW_ETCHED_OUT（内凹），GTK_SHADOW_ETCHED_IN（外凸）
	gtk_frame_set_shadow_type(GTK_FRAME(frame),GTK_SHADOW_OUT);
	label = gtk_label_new("阴影类型：GTK_SHADOW_OUT 标题位置：居左");
	gtk_container_add(GTK_CONTAINER(frame),label);

	frame = gtk_frame_new("框架二");
	gtk_box_pack_start(GTK_BOX(vbox),frame,FALSE,FALSE,5);
	gtk_frame_set_shadow_type(GTK_FRAME(frame),GTK_SHADOW_OUT);
	//设置框架的标签位置，
	gtk_frame_set_label_align(GTK_FRAME(frame), 1.0, 0);
	label = gtk_label_new("阴影类型：GTK_SHADOW_ETCHED_OUT 标题位置：居右");
	gtk_container_add(GTK_CONTAINER(frame),label);
	
	frame = gtk_frame_new(NULL);
	gtk_box_pack_start(GTK_BOX(vbox),frame,FALSE,FALSE,5);
	label = gtk_label_new("阴影类型：默认 标题位置：自定义的图像");
	gtk_container_add(GTK_CONTAINER(frame),label);
	image = gtk_image_new_from_file("title.bmp");
	gtk_frame_set_label_widget(GTK_FRAME(frame), image);
	
	//创建纵横比框架,在比例上是固定的，拖动窗口，不随之改动，第二、三个参数表示横向、纵向的位置，
	//第四个参数表示纵横比系数，最后一个参数如果设置为TRUE，子构件的长度比例会和他所请求的理想长宽比例相匹配。
	asframe = gtk_aspect_frame_new("框架四", 0, 0.8, 0.8, TRUE);
	gtk_box_pack_start(GTK_BOX(vbox),asframe,FALSE,FALSE,5);
	label = gtk_label_new("阴影类型：默认 标题位置：居左\n使用了aspectframe控件，不能改变大小");
	gtk_container_add(GTK_CONTAINER(asframe),label);
	
	gtk_widget_show_all(window);
	gtk_main();
	return FALSE;
}

	
