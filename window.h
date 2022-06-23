
#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <gtk/gtk.h>
#include "bars.h"
#include "grid.h"


#define MAIN_WINDOW_HEIGHT 300
#define MAIN_WINDOW_WIDTH 200




typedef struct _MainWindow MainWindow;

struct _MainWindow
{
	/* Datos privados */
	GtkWidget *window;
	GtkWidget *vbox;
	MenuBar menubar;
	ToolBar toolbar;
	LifeGrid lifegrid;
};



void Create_Main_Window();


#endif
