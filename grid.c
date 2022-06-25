
#include <gtk/gtk.h>
#include "grid.h"
#include "vida.h"
#include "misc.h"
#include "bars.h"




/* Funciones privadas */
static void Draw_Lines(LifeGrid *);
static void Draw_Cells(LifeGrid *);
static void Set_Zoom_Level(LifeGrid *, uint32);
static void Adjust_Scrollbar(LifeGrid *);
static void Scroolbar_Value_Changed(GtkAdjustment *, gpointer);
static gboolean Execute_Evolution_Function(gpointer);
static void Configure_Buffer(LifeGrid *);
static gboolean Configure_Event(GtkWidget *, GdkEventConfigure *, gpointer);
static gboolean Expose_Event(GtkWidget *, GdkEventExpose *, gpointer);

/* Datos privados */
static boolean gridcreated = FALSE;




/* Crea el área de la grilla, compuesta por un área de dibujo, barras de
   desplazamiento vertical y horizontal, y una tabla de empaquetado donde
   se mete todo. */
GtkWidget * Grid_Create_Area(LifeGrid *lifegrid)
{
	lifegrid->darea = gtk_drawing_area_new();
	gtk_widget_set_size_request(lifegrid->darea, 100, 100);
	
	lifegrid->vadjustment = gtk_adjustment_new(0, 0, 0, SCROLL_STEP_INCREMENT,
											   SCROLL_PAGE_INCREMENT, 0);
	lifegrid->hadjustment = gtk_adjustment_new(0, 0, 0, SCROLL_STEP_INCREMENT,
											   SCROLL_PAGE_INCREMENT, 0);

	lifegrid->vscrollbar = gtk_vscrollbar_new(GTK_ADJUSTMENT(
											  lifegrid->vadjustment));
	lifegrid->hscrollbar = gtk_hscrollbar_new(GTK_ADJUSTMENT(
											  lifegrid->hadjustment));

	lifegrid->table = gtk_table_new(2, 2, FALSE);
	
	gtk_table_attach_defaults(GTK_TABLE(lifegrid->table), lifegrid->darea,
							  0, 1, 0, 1);
	
	gtk_table_attach(GTK_TABLE(lifegrid->table), lifegrid->vscrollbar,
					 1, 2, 0, 1, GTK_SHRINK, GTK_FILL | GTK_EXPAND, 0, 0);

	gtk_table_attach(GTK_TABLE(lifegrid->table), lifegrid->hscrollbar,
					 0, 1, 1, 2, GTK_FILL | GTK_EXPAND, GTK_SHRINK, 0, 0);

	g_signal_connect(GTK_OBJECT(lifegrid->darea), "configure_event",
					 (GtkSignalFunc)Configure_Event, lifegrid);

	g_signal_connect(GTK_OBJECT(lifegrid->darea), "expose_event",
					 (GtkSignalFunc)Expose_Event, lifegrid);

	g_signal_connect(GTK_OBJECT(lifegrid->vadjustment), "value-changed",
					 (GtkSignalFunc)Scroolbar_Value_Changed, lifegrid);

	g_signal_connect(GTK_OBJECT(lifegrid->hadjustment), "value-changed",
					 (GtkSignalFunc)Scroolbar_Value_Changed, lifegrid);
					 
	return lifegrid->table;
}



/* Inicializa una nueva grilla del tamaño especificado por rows y columns */
void Grid_Create(LifeGrid *lifegrid, uint32 rows, uint32 columns)
{
	lifegrid->rows = rows;
	lifegrid->columns = columns;
	lifegrid->buffer = NULL;
	lifegrid->evolving = FALSE;
	lifegrid->delay = DEFAULT_DELAY;
	lifegrid->states = Life_Initial_State(lifegrid->rows, lifegrid->columns);
	Set_Zoom_Level(lifegrid, DEFAULT_ZOOM_LEVEL);
	gtk_widget_show_all(lifegrid->table);
	gridcreated = TRUE;
}



/* Destruye la grilla y libera memoria */
void Grid_Destroy(LifeGrid *lifegrid)
{
	g_object_unref(G_OBJECT(lifegrid->buffer));
	lifegrid->evolving = FALSE;
	Life_Terminate(lifegrid->states);
	gtk_widget_hide_all(lifegrid->table);
	gridcreated = FALSE;
}



/* Retorna si la grilla está creada o no */
boolean Grid_Is_Created()
{
	return gridcreated;
}



/* Comienza/continua con la evolución */
void Grid_Start_Evolution(LifeGrid *lifegrid)
{
	lifegrid->evolving = TRUE;
	lifegrid->idevol = g_timeout_add(lifegrid->delay,
										 Execute_Evolution_Function,
										 lifegrid);
}



/* Detiene la evolución */
void Grid_Stop_Evolution(LifeGrid *lifegrid)
{
	g_source_remove(lifegrid->idevol);
	lifegrid->evolving = FALSE;
}



/* Retorna si la grilla está evolucionando o no */
boolean Grid_Is_Evolving(LifeGrid *lifegrid)
{
	return lifegrid->evolving;
}



/* Aumenta el zoom de la grilla */
void Grid_Zoom_In(LifeGrid *lifegrid)
{
	Set_Zoom_Level(lifegrid, lifegrid->zoomlevel + 1);
	Configure_Buffer(lifegrid);
	Adjust_Scrollbar(lifegrid);
	Draw_Lines(lifegrid);
	Draw_Cells(lifegrid);
}



/* Disminuye el zoom de la grilla */
void Grid_Zoom_Out(LifeGrid *lifegrid)
{
	Set_Zoom_Level(lifegrid, lifegrid->zoomlevel - 1);
	Configure_Buffer(lifegrid);
	Adjust_Scrollbar(lifegrid);
	Draw_Lines(lifegrid);
	Draw_Cells(lifegrid);
}



/* Retorna el nivel del zoom */
uint32 Grid_Get_Zoom_Level(LifeGrid *lifegrid)
{
	return lifegrid->zoomlevel;
}



/* Modifica el delay */
void Grid_Change_Delay(LifeGrid *lifegrid, uint32 delay)
{
	lifegrid->delay = delay;
}




/****************************************/
/********** Funciones privadas **********/
/****************************************/



/* Dibuja una grilla nueva en el buffer y actualiza el área de dibujo. Si la
   grilla entra toda en el área de dibujo entonces se dibuja completamente, en
   caso contrario solo se dibujan las filas y columnas que serán visibles. */
static void Draw_Lines(LifeGrid *lifegrid)
{
	uint32 rows = lifegrid->rows;
	uint32 columns = lifegrid->columns;
	uint32 rowsize = lifegrid->rowsize;
	uint32 colsize = lifegrid->colsize;
	uint32 height = lifegrid->darea->allocation.height; /* Altura del área */
	uint32 width = lifegrid->darea->allocation.width; /* Anchura del área */
	gint32 bheight, bwidth; /* Longitudes del buffer */
	uint32 rheight = rowsize * rows + 1; /* Altura real de la grilla */
	uint32 rwidth = colsize * columns + 1; /* Anchura real de la grilla */
	uint32 mtop, mleft, mbottom, mright; /* Márgenes */
	uint32 vlines; /* Cantidad de lineas verticales que hay que dibujar */
	uint32 hlines; /* Cantidad de lineas horizontales que hay que dibujar */
	GdkSegment *hsegments, *vsegments;
	uint32 c, pos;
	

	gdk_drawable_get_size(lifegrid->buffer, &bwidth, &bheight);

	/* Determina la cantidad de lineas verticales y horizontales */
	if (rheight <= height)
	{
		hlines = rows + 1;
		mtop = (height - rheight) / 2;
		mbottom = height - mtop - 2;
	}
	else
	{
		hlines = (height - 1) / rowsize + 2;
		mtop = 0;
		mbottom = bheight - 1;
	}

	if (rwidth <= width)
	{
		vlines = columns + 1;
		mleft = (width - rwidth) / 2;
		mright = width - mleft - 2;
	}
	else
	{
		vlines = (width - 1) / colsize + 2;
		mleft = 0;
		mright = bwidth - 1;
	}
	
	hsegments = g_malloc(hlines * sizeof(GdkSegment));
	vsegments = g_malloc(vlines * sizeof(GdkSegment));
	g_assert(hsegments && vsegments);


	/* Se cargan los datos para los segmentos */
	for (c = 0, pos = mtop; c < hlines; c++, pos+=rowsize) {
		hsegments[c].x1 = mleft;
		hsegments[c].y1 = pos;
		hsegments[c].x2 = mright;
		hsegments[c].y2 = pos;
		// Rutina de depuración (controla que no se dibuje afuera del área) //
		if (hsegments[c].x1 < 0)
			g_print("x1 es negativo\n");
		if (hsegments[c].x2 > (bwidth - 1))
			g_print("x2 es mayor que bwidth - 1: dif: %d\n",
					hsegments[c].x2 - (bwidth - 1));
		if (hsegments[c].y1 < 0)
			g_print("y1 es negativo\n");
		if (hsegments[c].y2 > (bheight - 1))
			g_print("y2 es mayor que bheight - 1: dif: %d\n",
					hsegments[c].y2 - (bheight - 1));
		// Fin rutina de depuración //
	}
	for (c = 0, pos = mleft; c < vlines; c++, pos+=colsize) {
		vsegments[c].x1 = pos;
		vsegments[c].y1 = mtop;
		vsegments[c].x2 = pos;
		vsegments[c].y2 = mbottom;
		// Rutina de depuración (controla que no se dibuje afuera del área) //
		if (vsegments[c].x1 < 0)
			g_print("x1 es negativo\n");
		if (vsegments[c].x2 > (bwidth - 1))
			g_print("x2 es mayor que bwidth - 1: dif: %d\n",
					vsegments[c].x2 - (bwidth - 1));
		if (vsegments[c].y1 < 0)
			g_print("y1 es negativo\n");
		if (vsegments[c].y2 > (bheight - 1))
			g_print("y2 es mayor que bheight - 1: dif: %d\n",
					vsegments[c].y2 - (bheight - 1));
		// Fin rutina de depuración //
	}


	gdk_draw_rectangle(lifegrid->buffer, lifegrid->darea->style->white_gc,
					   TRUE, 0, 0, bwidth, bheight);
	gdk_draw_segments(lifegrid->buffer, lifegrid->darea->style->black_gc,
					  hsegments, hlines);
	gdk_draw_segments(lifegrid->buffer, lifegrid->darea->style->black_gc,
					  vsegments, vlines);

	g_free(hsegments);
	g_free(vsegments);
	
	gtk_widget_queue_draw_area(lifegrid->darea, 0, 0, width, height);
}



/* Dibuja las células en el buffer apartir de una matriz de estados y
   actualiza el área de dibujo. Si la grilla entra toda en el área de dibujo
   entonces se dibuja completamente, en caso contrario solo se dibujan las
   filas y columnas que serán visibles. */
static void Draw_Cells(LifeGrid *lifegrid)
{
	uint32 rows = lifegrid->rows;
	uint32 columns = lifegrid->columns;
	uint32 rowsize = lifegrid->rowsize;
	uint32 colsize = lifegrid->colsize;
	uint32 height = lifegrid->darea->allocation.height; /* Altura del área */
	uint32 width = lifegrid->darea->allocation.width; /* Anchura del área */
	gint bheight, bwidth; /* Longitudes del buffer */
	uint32 rheight = rowsize * rows + 1; /* Altura real de la grilla */
	uint32 rwidth = colsize * columns + 1; /* Anchura real de la grilla */
	uint32 mtop, mleft, mbottom, mright; /* Márgenes */
	uint32 disph; /* Desplazamiento horizontal de la grilla */
	uint32 dispv; /* Desplazamiento vertical de la grilla */
	uint32 posx; /* Posición x apartir de la cual se dibujan las células */
	uint32 posy; /* Posición y apartir de la cual se dibujan las células */
	uint32 drows; /* Cantidad de filas que se dibujan */
	uint32 dcols; /* Cantidad de columnas que se dibujan */
	uint32 ndrows; /* Cantidad de filas que no se dibujan */
	uint32 ndcols; /* Cantidad de columnas que no se dibujan */
	uint32 x, y, col, row, pos;


	gdk_drawable_get_size(lifegrid->buffer, &bwidth, &bheight);
	
	disph = gtk_adjustment_get_value(GTK_ADJUSTMENT(lifegrid->hadjustment));
	dispv = gtk_adjustment_get_value(GTK_ADJUSTMENT(lifegrid->vadjustment));
	
	if (rheight <= height)
	{
		drows = rows;
		ndrows = 0;
		posy = 0;
		mtop = (height - rheight) / 2;
		mbottom = height - mtop - 2;
	}
	else
	{
		drows = (height - 1) / rowsize + 2;
		ndrows = rows - drows;
		posy = dispv / rowsize;
		mtop = 0;
		mbottom = height - 1;
	}

	if (rwidth <= width)
	{
		dcols = columns;
		ndcols = 0;
		posx = 0;
		mleft = (width - rwidth) / 2;
		mright = width - mleft - 2;
	}
	else
	{
		dcols = (width - 1) / colsize + 2;
		ndcols = columns - dcols;
		posx = disph / colsize;
		mleft = 0;
		mright = width - 1;
	}
	

	/* Posición de la matriz (vista como un array) donde se empieza a
	   recorrerla */
	pos = posy * columns + posx;
	
	/* Se recorre la matriz como un array y se dibujan las células vivas */
	y = mtop;
	for (row = 0; row < drows; row++)
	{
		x = mleft;
		for (col = 0; col < dcols; col++) {
			if (lifegrid->states[pos] == Alive) {
				gdk_draw_rectangle(lifegrid->buffer,
								   lifegrid->darea->style->black_gc,
								   TRUE, x, y, colsize, rowsize);
				// Rutina de depuración //
				if (x < 0)
					g_print("x es negativo\n");
				if (x + colsize > (bwidth - 1))
					g_print("x + colsize es mayor que bwidth - 1: dif: %d\n",
							x + colsize - (bwidth - 1));
				if (y < 0)
					g_print("y es negativo\n");
				if (y + rowsize > (bheight - 1))
					g_print("y + rowsize es mayor que bheight - 1: dif: %d\n",
							y + rowsize - (bheight - 1));
				// Fin de rutina de depuración //
			}
			pos++;
			x += colsize;
		}
		pos+= ndcols; /* Se saltean las columnas que no se dibujan */
		y += rowsize;
	}

	gtk_widget_queue_draw_area(lifegrid->darea, 0, 0, width, height);
}



/* Setea el nivel de zoom para la grilla */
static void Set_Zoom_Level(LifeGrid *lifegrid, uint32 level)
{
	uint32 rowsize, colsize;

	switch(level)
	{
		case 1:
			rowsize = colsize = 2;
			break;
		case 2:
			rowsize = colsize = 5;
			break;
		case 3:
			rowsize = colsize = 10;
			break;
		case 4:
			rowsize = colsize = 20;
			break;
		case 5:
			rowsize = colsize = 30;
			break;
		default:
			g_assert(0);
	}
	lifegrid->zoomlevel = level;
	lifegrid->rowsize = rowsize;
	lifegrid->colsize = colsize;
}



/* Ajusta las barras de desplazamiento */
static void Adjust_Scrollbar(LifeGrid *lifegrid)
{
	uint32 rows = lifegrid->rows;
	uint32 columns = lifegrid->columns;
	uint32 rowsize = lifegrid->rowsize;
	uint32 colsize = lifegrid->colsize;
	uint32 rheight = rowsize * rows + 1; /* Altura real de la grilla */
	uint32 rwidth = colsize * columns + 1; /* Anchura real de la grilla */
	uint32 height = lifegrid->darea->allocation.height; /* Altura del área */
	uint32 width = lifegrid->darea->allocation.width; /* Achura del área */

	/* Ajusta la barra de desplazamiento vertical */
	GTK_ADJUSTMENT(lifegrid->vadjustment)->lower = 0;
	GTK_ADJUSTMENT(lifegrid->vadjustment)->upper = rheight;
	GTK_ADJUSTMENT(lifegrid->vadjustment)->value = 0;
	GTK_ADJUSTMENT(lifegrid->vadjustment)->page_size = height;

	/* Ajusta la barra de desplazamiento horizontal */
	GTK_ADJUSTMENT(lifegrid->hadjustment)->lower = 0;
	GTK_ADJUSTMENT(lifegrid->hadjustment)->upper = rwidth;
	GTK_ADJUSTMENT(lifegrid->hadjustment)->value = 0;
	GTK_ADJUSTMENT(lifegrid->hadjustment)->page_size = width;

	gtk_adjustment_changed(GTK_ADJUSTMENT(lifegrid->hadjustment));
	gtk_adjustment_changed(GTK_ADJUSTMENT(lifegrid->vadjustment));
}



/* Retrollamada para las barras de desplazamiento */
static void Scroolbar_Value_Changed(GtkAdjustment *adjustment, gpointer data)
{
	LifeGrid *lifegrid = (LifeGrid *)data;
	Draw_Lines(lifegrid);
	Draw_Cells(lifegrid);
}



/* Ejecuta la función de evolución y actualiza la grilla */
static gboolean Execute_Evolution_Function(gpointer data)
{
	LifeGrid *lifegrid = (LifeGrid *)data;
	lifegrid->states = Life_Evolution(lifegrid->states, lifegrid->rows,
								 lifegrid->columns);
	Draw_Lines(lifegrid);
	Draw_Cells(lifegrid);
	
	return TRUE;
}



/* Redimenciona el buffer */
static void Configure_Buffer(LifeGrid *lifegrid)
{
	uint32 rows = lifegrid->rows;
	uint32 columns = lifegrid->columns;
	uint32 rowsize = lifegrid->rowsize;
	uint32 colsize = lifegrid->colsize;
	uint32 height = lifegrid->darea->allocation.height;
	uint32 width = lifegrid->darea->allocation.width;
	uint32 rheight = rowsize * rows + 1; /* Altura real de la grilla */
	uint32 rwidth = colsize * columns + 1; /* Anchura real de la grilla */
	uint32 bufferheight, bufferwidth;
	
	if (rheight <= height)
		bufferheight = height;
	else
		bufferheight = height - (height % rowsize) + 2 * rowsize + 1;

	if (rwidth <= width)
		bufferwidth = width;
	else
		bufferwidth = width - (width % colsize) + 2 * colsize + 1;
	
	if (lifegrid->buffer)
		g_object_unref(G_OBJECT(lifegrid->buffer));

	lifegrid->buffer = gdk_pixmap_new(lifegrid->darea->window, bufferwidth,
									  bufferheight, -1);
}



/* Redimenciona el buffer cuando el área de dibujo cambia de tamaño y
se actualiza la misma */
static gboolean Configure_Event(GtkWidget *darea, GdkEventConfigure *event,
								gpointer data)
{
	LifeGrid *lifegrid = (LifeGrid *)data;
	uint32 height = lifegrid->darea->allocation.height;
	uint32 width = lifegrid->darea->allocation.width;

	//g_print("%d, %d\n", height, width);
	if (height == 1 && width == 1) return TRUE; /* ¿Por qué? */

	Configure_Buffer(lifegrid);
	Adjust_Scrollbar(lifegrid);
	Draw_Lines(lifegrid);
	Draw_Cells(lifegrid);

	return TRUE;
}



/* Redibuja el área de dibujo utilizando el buffer */
static gboolean Expose_Event(GtkWidget *darea, GdkEventExpose *event,
							 gpointer data)
{
	LifeGrid *lifegrid = (LifeGrid *)data;
	uint32 rows = lifegrid->rows;
	uint32 columns = lifegrid->columns;
	uint32 rowsize = lifegrid->rowsize;
	uint32 colsize = lifegrid->colsize;
	uint32 height = lifegrid->darea->allocation.height;
	uint32 width = lifegrid->darea->allocation.width;
	uint32 rheight = rowsize * rows + 1; /* Altura real de la grilla */
	uint32 rwidth = colsize * columns + 1; /* Anchura real de la grilla */
	uint32 dispv; /* Desplazamiento vertical de la grilla */
	uint32 disph; /* Desplazamiento horizontal de la grilla */
	uint32 restv; /* Desplazamiento vertical del buffer */
	uint32 resth; /* Desplazamiento horizontal del buffer */

	dispv = gtk_adjustment_get_value(GTK_ADJUSTMENT(lifegrid->vadjustment));
	disph = gtk_adjustment_get_value(GTK_ADJUSTMENT(lifegrid->hadjustment));


	/* Si rheight <= height, acaso dispv no seria 0 siempre? */
	if (rheight <= height)
		restv = 0;
	else
		restv = dispv % rowsize;

	if (rwidth <= width)
		resth = 0;
	else
		resth = disph % colsize;
	
	gdk_draw_drawable(darea->window,
					  darea->style->fg_gc[GTK_WIDGET_STATE(darea)],
					  lifegrid->buffer,
					  event->area.x + resth, event->area.y + restv,
					  event->area.x, event->area.y,
					  event->area.width, event->area.height);
	
	return FALSE;
}
