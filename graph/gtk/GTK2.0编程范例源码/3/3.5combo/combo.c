//创建和使用下拉列表框控件
#include <gtk/gtk.h>

GtkWidget *create_item(gint i)
{
    GtkWidget *item;
    GtkWidget *hbox;
    GtkWidget *image;
    GtkWidget *label;

    hbox = gtk_hbox_new(FALSE, 0);
    //根据i的值创建不同的图像和标签
    switch(i)
    {
    case 1:
        image = gtk_image_new_from_stock(GTK_STOCK_YES, GTK_ICON_SIZE_MENU);
        label = gtk_label_new("列表项一");
        break;
    case 2:
        image = gtk_image_new_from_stock(GTK_STOCK_NO, GTK_ICON_SIZE_MENU);
        label = gtk_label_new("列表项二");
        break;
    case 3:
        image = gtk_image_new_from_stock(GTK_STOCK_HELP, GTK_ICON_SIZE_MENU);
        label = gtk_label_new("列表项三");
        break;
    case 4:
        image = gtk_image_new_from_stock(GTK_STOCK_OK, GTK_ICON_SIZE_MENU);
        label = gtk_label_new("列表项四");
        break;
    case 5:
        image = gtk_image_new_from_stock(GTK_STOCK_CANCEL, GTK_ICON_SIZE_MENU);
        label = gtk_label_new("列表项五");
        break;
    }
    //将标签添加到盒子里
    gtk_box_pack_start(GTK_BOX(hbox), image, FALSE, FALSE, 2);
    gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 2);
    //创建一个数据项
    item = gtk_list_item_new();
    //盒子放进数据项中
    gtk_container_add(GTK_CONTAINER(item), hbox);
    gtk_widget_show_all(item);
    return item;
}

int main(int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *combo;
    GtkWidget *vbox;
    GtkWidget *label;
    GList *items = NULL;
    GtkWidget *item;

    //向列表里添加数据项
    items = g_list_append(items, "列表项A");
    items = g_list_append(items, "列表项B");
    items = g_list_append(items, "列表项C");
    items = g_list_append(items, "列表项D");
    items = g_list_append(items, "列表项E");

    gtk_init(&argc, &argv);

    //创建窗口以及设定窗口初始属性
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(gtk_main_quit), NULL);
    gtk_window_set_title(GTK_WINDOW(window), "下拉列表框");
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(window), 20);

    //创建盒子
    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    //第一种向下拉列表框添加数据项的方法
    label = gtk_label_new("请选择下列中的一项：");
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 5);
    //创建一个下拉列表框
    combo = gtk_combo_new();
    gtk_box_pack_start(GTK_BOX(vbox), combo, FALSE, FALSE, 5);
    //向下拉列表框添加数据项所在的列表，并在文字录入控件中显示数据项名称
    gtk_combo_set_popdown_strings(GTK_COMBO(combo), items);

    //第二种向下拉列表框添加数据项的方法
    label = gtk_label_new("另一种选择方式：");
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 5);
    combo = gtk_combo_new();
    gtk_box_pack_start(GTK_BOX(vbox), combo, FALSE, FALSE, 5);

    //调用函数获取数据项
    item = create_item(1);
    //在文字录入控件中对指定的数据项显示指定的字符串
    gtk_combo_set_item_string(GTK_COMBO(combo), GTK_ITEM(item), "项目一");
    //将得到的数据项添加到下拉列表框的列表中
    gtk_container_add(GTK_CONTAINER(GTK_COMBO(combo)->list), item);

    item = create_item(2);
    gtk_combo_set_item_string(GTK_COMBO(combo), GTK_ITEM(item), "项目二");
    gtk_container_add(GTK_CONTAINER(GTK_COMBO(combo)->list), item);

    item = create_item(3);
    gtk_combo_set_item_string(GTK_COMBO(combo), GTK_ITEM(item), "项目三");
    gtk_container_add(GTK_CONTAINER(GTK_COMBO(combo)->list), item);

    item = create_item(4);
    gtk_combo_set_item_string(GTK_COMBO(combo), GTK_ITEM(item), "项目四");
    gtk_container_add(GTK_CONTAINER(GTK_COMBO(combo)->list), item);

    item = create_item(5);
    gtk_combo_set_item_string(GTK_COMBO(combo), GTK_ITEM(item), "项目五");
    gtk_container_add(GTK_CONTAINER(GTK_COMBO(combo)->list), item);

    //显示窗口
    gtk_widget_show_all(window);
    gtk_main();
    return FALSE;
}
