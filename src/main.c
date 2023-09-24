#include <cairo.h>
#include <gtk/gtk.h>
#include <z80.h>

#include "ef9345.h"
#include "emulator.h"

GtkWidget *about_window;
GtkWidget *debug_window;
GtkWidget *keyboard_window;
GtkWidget *debug_address;
GtkWidget *screen_area;

GThread *emulator_thread;
cairo_surface_t *screen;

int main(int argc, char *argv[])
{
	GtkWidget *main_window;
	GtkBuilder *builder;

	gtk_init(&argc, &argv);

	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "vgtk_ui.glade", NULL);

	main_window		= GTK_WIDGET(gtk_builder_get_object(builder, "MainWindow"));
	about_window	= GTK_WIDGET(gtk_builder_get_object(builder, "AboutDialog"));
	debug_window	= GTK_WIDGET(gtk_builder_get_object(builder, "DebugWindow"));
	keyboard_window	= GTK_WIDGET(gtk_builder_get_object(builder, "KeyboardWindow"));
	debug_address	= GTK_WIDGET(gtk_builder_get_object(builder, "DebuggerAddressDialog"));
	screen_area		= GTK_WIDGET(gtk_builder_get_object(builder, "OutputArea"));

    screen = cairo_image_surface_create(CAIRO_FORMAT_RGB24, 492, 312);

    gtk_builder_connect_signals(builder, NULL);
	g_object_unref(builder);

	emulator_init(argc, argv);
	emulator_thread = g_thread_new("Emulator", emulator_run, NULL);

	gtk_widget_show(main_window);
    gtk_main();

    return 0;
}

void on_graphics_kb_click()
{
	gtk_widget_show(keyboard_window);
}

void on_about_click()
{
	gtk_widget_show(about_window);
}

void on_screen_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
    extern cairo_surface_t* screen;

    cairo_set_source_surface(cr, screen, 0, 0);
    cairo_paint(cr); // Crash
}

void on_main_window_exit()
{
    gtk_main_quit();
}

void emulator_refresh_screen(struct ef9345 *ef_ctx)
{
    uint32_t *screen_data;
    int stride;
    int x, y;

    ef9345_rasterize(ef_ctx);

    cairo_surface_flush(screen);

    screen_data = (uint32_t*) cairo_image_surface_get_data(screen);
    stride = cairo_image_surface_get_stride(screen);

    for(y = 0; y < 312; y ++)
        for(x = 0; x < 492; x ++)
            screen_data[x + stride * y / 4] = ef_ctx->raster[0][0];

    cairo_surface_mark_dirty(screen);
    gtk_widget_queue_draw_area(screen_area, 0, 0, 492, 312);
}
