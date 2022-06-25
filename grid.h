
#ifndef __GRID_H__
#define __GRID_H__

#include <gtk/gtk.h>
#include "vida.h"
#include "misc.h"


#define DEFAULT_DELAY 200
#define DEFAULT_ZOOM_LEVEL 3
#define ZOOM_LEVEL_MAX 5
#define SCROLL_STEP_INCREMENT 10
#define SCROLL_PAGE_INCREMENT 100




typedef struct _LifeGrid LifeGrid;

struct _LifeGrid
{
	/* Datos privados */
	GtkWidget *table; /* Tabla de empaquetado */
	GtkWidget *darea; /* Área de dibujo donde se dibuja la grilla */
	GtkWidget *vscrollbar; /* Barra de desplazamiento vertical */
	GtkWidget *hscrollbar; /* Barra de desplazamiento horizontal */
	GtkObject *vadjustment; /* Valores de ajuste para la barra vertical */
	GtkObject *hadjustment; /* Valores de ajuste para la barra horizontal */
	uint32 rows; /* Número de filas */
	uint32 columns; /* Número de columnas */
	uint32 rowsize; /* Tamaño de las filas */
	uint32 colsize; /* Tamaño de las columnas */
	GdkPixmap *buffer; /* Buffer para el área donde se dibuja la grilla */
	uint32 zoomlevel; /* Nivel de zoom */
	State *states; /* Puntero a una matriz de estados */
	uint32 delay; /* Demora entre cada llamada a la función de evolución */
	guint idevol; /* Identificador que retorna g_timeout_add */
	boolean evolving; /* Indica que la evolución está activa */
};




GtkWidget * Grid_Create_Area(LifeGrid *);
void Grid_Create(LifeGrid *, uint32, uint32);
void Grid_Destroy(LifeGrid *);
boolean Grid_Is_Created();
void Grid_Zoom_In(LifeGrid *);
void Grid_Zoom_Out(LifeGrid *);
uint32 Grid_Get_Zoom_Level(LifeGrid *);
void Grid_Start_Evolution(LifeGrid *);
void Grid_Stop_Evolution(LifeGrid *);
boolean Grid_Is_Evolving(LifeGrid *);
void Grid_Change_Delay(LifeGrid *, uint32);



#endif
