? ??
#include<gtk/gtk.h>
#include<gdk/gdk.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
static GdkPixmap *pixmap = NULL;
int radius;
void DrawTickAt(GdkDrawable * pixmap, GdkGC * gc, int nHour, int cx, int cy)
{
    double dRadians = nHour * 3.14 / 6.0;
    gdk_draw_line(pixmap, gc, cx + (int) ((0.95 * radius * sin(dRadians))), cy - (int) ((0.95 * radius * cos(dRadians))), cx + (int) ((1.0 * radius * sin(dRadians))), cy - (int) ((1.0 * radius * cos(dRadians))));
}

gint Repaint(gpointer data)
{
    GtkWidget *drawing_area = (GtkWidget *) data;
    GdkRectangle update_rect;
    int midx, midy;
    int nHour;
    float dRadians;
    time_t now;
    struct tm *now_tm;
    gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, 0, 0, drawing_area->allocation.width, drawing_area->allocation.height);
    midx = drawing_area->allocation.width / 2;
    midy = drawing_area->allocation.height / 2;
    radius = MIN(midx, midy);
    gdk_draw_arc(pixmap, drawing_area->style->black_gc, 0, 0, 0, midx + midx, midy + midy, 0, 360 * 64);
    for (nHour = 1; nHour <= 12; nHour++)

    {
        DrawTickAt(pixmap, drawing_area->style->black_gc, nHour, midx, midy);
    }
    time(&now);
    now_tm = localtime(&now);
    dRadians = now_tm->tm_sec * 3.14 / 30.0;
    gdk_draw_line(pixmap, drawing_area->style->black_gc, midx, midy, midx + (0.9 * radius * sin(dRadians)), midy - (0.9 * radius * cos(dRadians)));
    dRadians = (now_tm->tm_min * 3.14 / 30.0) + (3.14 * now_tm->tm_sec / 1800.0);
    gdk_draw_line(pixmap, drawing_area->style->black_gc, midx, midy, midx + (int) (0.7 * radius * sin(dRadians)), midy - (int) (0.7 * radius * cos(dRadians)));
    dRadians = (now_tm->tm_hour % 12) * 3.14 / 6.0 + (3.14 * now_tm->tm_min / 360.0);
    gdk_draw_line(pixmap, drawing_area->style->black_gc, midx, midy, midx + (int) (radius * 0.5 * sin(dRadians)), midy - (int) (radius * 0.5 * cos(dRadians)));
    update_rect.x = 0;
    update_rect.y = 0;
    update_rect.width = drawing_area->allocation.width;
    update_rect.height = drawing_area->allocation.height;
    gtk_widget_draw(drawing_area, &update_rect);
}

static gint configure_event(GtkWidget * widget, GdkEventConfigure * event)
{
    if (pixmap)

    {
        gdk_pixmap_unref(pixmap);
    }
    pixmap = gdk_pixmap_new(widget->window, widget->allocation.width, widget->allocation.height, -1);
    return TRUE;
}

gint expose_event(GtkWidget * widget, GdkEventExpose * event)
{
    gdk_draw_pixmap(widget->window, widget->style->fg_gc[GTK_WIDGET_STATE(widget)], pixmap, event->area.x, event->area.y, event->area.x, event->area.y, event->area.width, event->area.height);
    return FALSE;
}

void quit()
{
    gtk_exit(0);
}

int main(int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *drawing_area;
    GtkWidget *vbox;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_signal_connect(GTK_OBJECT(window), "destroy", GTK_SIGNAL_FUNC(quit), NULL);

    vbox = gtk_hbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    gtk_widget_show(vbox);

    drawing_area = gtk_drawing_area_new();
    gtk_drawing_area_size(GTK_DRAWING_AREA(drawing_area), 200, 200);
    gtk_signal_connect(GTK_OBJECT(drawing_area), "expose_event", (GtkSignalFunc) expose_event, NULL);
    gtk_signal_connect(GTK_OBJECT(drawing_area), "configure_event", (GtkSignalFunc) configure_event, NULL);

    gtk_box_pack_start(GTK_BOX(vbox), drawing_area, TRUE, TRUE, 0);
    gtk_widget_show(drawing_area);

    gtk_widget_show(window);
    gtk_timeout_add(1000, Repaint, drawing_area);
    gtk_main();

    return 0;
}
