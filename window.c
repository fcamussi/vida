
#include <gtk/gtk.h>
#include "window.h"
#include "bars.h"
#include "grid.h"
#include "functions.h"



/* Funciones privadas */
static gboolean Delete_Event(GtkWidget *, GdkEvent *, gpointer);




/* Crea la ventana principal con el area para la grilla */
void Create_Main_Window(MainWindow *mainwindow)
{
	GtkWidget *menubar;
	GtkWidget *toolbar;
	GtkWidget *gridarea;

	/* Se crea la ventana principal */
	mainwindow->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(mainwindow->window), "Vida");
	gtk_window_set_default_size(GTK_WINDOW(mainwindow->window),
								MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT);

	mainwindow->vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(mainwindow->window),
					  mainwindow->vbox);
	
	g_signal_connect(G_OBJECT(mainwindow->window), "delete_event",
	G_CALLBACK(Delete_Event), &mainwindow->lifegrid);


	/* Se crea la barra de menú */
	menubar = Menu_Create(&mainwindow->menubar, &mainwindow->lifegrid);

	gtk_window_add_accel_group(GTK_WINDOW(mainwindow->window),
							   Menu_Get_Accel_Group(&mainwindow->menubar));

	gtk_box_pack_start(GTK_BOX(mainwindow->vbox), menubar, FALSE, FALSE, 0);


	/* Se crea la barra de herramientas */
	toolbar = Toolbar_Create(&mainwindow->toolbar, &mainwindow->lifegrid);
	
	gtk_box_pack_start(GTK_BOX(mainwindow->vbox), toolbar, FALSE, FALSE, 1);


	/* Se muestra todo */
	gtk_widget_show_all(mainwindow->window);


	/* Se crea el area donde se dibuja la grilla */
	gridarea = Grid_Create_Area(&mainwindow->lifegrid);

	gtk_box_pack_start(GTK_BOX(mainwindow->vbox), gridarea, TRUE, TRUE, 2);
}



/* Se ejecuta cuando se cierra la ventana */
static gboolean Delete_Event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	Function_Exit((LifeGrid *)data);
	return TRUE;
}
