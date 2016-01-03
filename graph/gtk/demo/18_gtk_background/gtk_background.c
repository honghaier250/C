#include <gtk/gtk.h>

void change_background(GtkWidget * widget, int w, int h, const gchar * path)
{
    //1.允许窗口可以绘图
    gtk_widget_set_app_paintable(widget, TRUE);
    gtk_widget_realize(widget);

    /* 更改背景图时，图片会重叠
     * 这时要手动调用下面的函数，让窗口绘图区域失效，产生窗口重绘制事件（即 expose 事件）。
     */
    gtk_widget_queue_draw(widget);
    GdkPixbuf *src = gdk_pixbuf_new_from_file(path, NULL);
    GdkPixbuf *dst = gdk_pixbuf_scale_simple(src, w, h, GDK_INTERP_BILINEAR);

    /* 创建pixmap图像;
     * NULL：不需要蒙版;
     * 123： 0~255，透明到不透明
     */
    GdkPixmap *pixmap = NULL;
    gdk_pixbuf_render_pixmap_and_mask(dst, &pixmap, NULL, 128);

    // 通过pixmap给widget设置一张背景图，最后一个参数必须为: FASL
    gdk_window_set_back_pixmap(widget->window, pixmap, FALSE);

    g_object_unref(src);
    g_object_unref(dst);
    g_object_unref(pixmap);

    return;
}

int main(int argc, char *argv[])
{
    GtkWidget *vbox;
    GtkWidget *hbox;
    GtkWidget *label;
    GtkWidget *user;
    //1.gtk环境的初始化
    gtk_init(&argc, &argv);

    //2.创建一个window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    //3.设置窗口标题
    gtk_window_set_title(GTK_WINDOW(window), "设置窗口背景图");
    gtk_widget_set_size_request(window, 500, 450);

    //4.设置窗口图片
    change_background(window, 500, 400, "./img/1.jpg");

    //5.显示所有窗口
    gtk_widget_show_all(window);

    //6.主事件循环
    gtk_main();

    return 0;
}
