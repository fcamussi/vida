
#include <gtk/gtk.h>
#include "bars.h"
#include "grid.h"
#include "functions.h"



/* Funciones privadas */
static void Menu_Items(gpointer, guint, GtkWidget *);
static void Button_New(GtkToolButton *, gpointer);
static void Button_Continue(GtkToolButton *, gpointer);
static void Button_Stop(GtkToolButton *, gpointer);
static void Button_Zoom_In(GtkToolButton *, gpointer);
static void Button_Zoom_Out(GtkToolButton *, gpointer);
static void Scale_Value_Changed(GtkRange *, gpointer);

/* Datos privados */
static GtkItemFactoryEntry menuitems[] = {
{"/_Archivo",				NULL		,			0,			0, "<Branch>"},
{"/Archivo/_Nuevo",			"<Control>N",			Menu_Items,	11, "<Item>"},
{"/Archivo/_Cerrar",		NULL,					Menu_Items,	12, "<Item>"},
{"/Archivo/_Salir",			"<Control>S",			Menu_Items,	13, "<Item>"},
{"/_Ver",					NULL,					0,			0, "<Branch>"},
{"/Ver/_Ampliar",			"<Control>Z",			Menu_Items,	21, "<Item>"},
{"/Ver/_Reducir",			"<Control><Shift>Z",	Menu_Items,	22, "<Item>"},
{"/_Evolución",				NULL,					0,			0, "<Branch>"},
{"/Evolución/_Continuar",	"<Control>C",			Menu_Items,	31, "<Item>"},
{"/Evolución/_Detener",		"<Control>D",			Menu_Items,	32, "<Item>"},
{"/Ayuda",					NULL,					0,			0, "<Branch>"},
{"/Ayuda/Acerca de",		NULL,					Menu_Items,	41, "<Item>"}
};




/* Crea la barra de menú */
GtkWidget * Menu_Create(MenuBar *menubar, LifeGrid *lifegrid)
{
	unsigned nitems = sizeof(menuitems) / sizeof(GtkItemFactoryEntry);
	
	menubar->agroup = gtk_accel_group_new();
	
	menubar->ifactory = gtk_item_factory_new(GTK_TYPE_MENU_BAR, "<Menu>",
											 menubar->agroup);
	menubar->menubar = gtk_item_factory_get_widget(menubar->ifactory, "<Menu>");

	gtk_item_factory_create_items(menubar->ifactory, nitems, menuitems,
								  lifegrid);

	return menubar->menubar;	
}



/* Crea la barra de herramientas */
GtkWidget * Toolbar_Create(ToolBar *toolbar, LifeGrid *lifegrid)
{
	toolbar->toolbar = gtk_toolbar_new();
	gtk_toolbar_set_orientation(GTK_TOOLBAR(toolbar->toolbar),
								GTK_ORIENTATION_HORIZONTAL);
	
	toolbar->bnew = gtk_tool_button_new_from_stock(GTK_STOCK_NEW);
	toolbar->bcontinue = gtk_tool_button_new_from_stock(GTK_STOCK_MEDIA_PLAY);
	toolbar->bstop = gtk_tool_button_new_from_stock(GTK_STOCK_MEDIA_STOP);
	toolbar->bzoomin = gtk_tool_button_new_from_stock(GTK_STOCK_ZOOM_IN);
	toolbar->bzoomout = gtk_tool_button_new_from_stock(GTK_STOCK_ZOOM_OUT);

	gtk_toolbar_insert(GTK_TOOLBAR(toolbar->toolbar), 
					   GTK_TOOL_ITEM(toolbar->bnew), -1);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar->toolbar),
					   GTK_TOOL_ITEM(gtk_separator_tool_item_new()), -1);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar->toolbar), 
					   GTK_TOOL_ITEM(toolbar->bcontinue), -1);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar->toolbar), 
					   GTK_TOOL_ITEM(toolbar->bstop), -1);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar->toolbar),
					   GTK_TOOL_ITEM(gtk_separator_tool_item_new()), -1);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar->toolbar), 
					   GTK_TOOL_ITEM(toolbar->bzoomin), -1);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar->toolbar), 
					   GTK_TOOL_ITEM(toolbar->bzoomout), -1);

	g_signal_connect(G_OBJECT(toolbar->bnew), "clicked",
					 G_CALLBACK(Button_New), lifegrid);
	g_signal_connect(G_OBJECT(toolbar->bcontinue), "clicked",
					 G_CALLBACK(Button_Continue), lifegrid);
	g_signal_connect(G_OBJECT(toolbar->bstop), "clicked",
					 G_CALLBACK(Button_Stop), lifegrid);
	g_signal_connect(G_OBJECT(toolbar->bzoomin), "clicked",
					 G_CALLBACK(Button_Zoom_In), lifegrid);
	g_signal_connect(G_OBJECT(toolbar->bzoomout), "clicked",
					 G_CALLBACK(Button_Zoom_Out), lifegrid);


	/* Regulador de tiempo de la barra de herramientas */
	toolbar->adjustment = gtk_adjustment_new(DEFAULT_DELAY, SCALE_VALUE_MIN,
											 SCALE_VALUE_MAX,
											 SCALE_STEP_INCREMENT,
											 SCALE_PAGE_INCREMENT, 0);
	toolbar->hscale = gtk_hscale_new(GTK_ADJUSTMENT(toolbar->adjustment));
	gtk_scale_set_draw_value(GTK_SCALE(toolbar->hscale), FALSE);

	toolbar->scale = gtk_tool_item_new();
	gtk_tool_item_set_expand(GTK_TOOL_ITEM(toolbar->scale), TRUE);

	gtk_container_add(GTK_CONTAINER(toolbar->scale), toolbar->hscale);

	gtk_toolbar_insert(GTK_TOOLBAR(toolbar->toolbar),
					   GTK_TOOL_ITEM(gtk_separator_tool_item_new()), -1);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar->toolbar),
					   GTK_TOOL_ITEM(toolbar->scale), -1);
					   
	g_signal_connect(G_OBJECT(toolbar->hscale), "value-changed",
					 G_CALLBACK(Scale_Value_Changed), lifegrid);
	
	return toolbar->toolbar;
}



GtkAccelGroup * Menu_Get_Accel_Group(MenuBar *menubar)
{
	return menubar->agroup;
}



void Toolbar_Scale_Set_Value(ToolBar *toolbar, uint32 value)
{
	gtk_adjustment_set_value(GTK_ADJUSTMENT(toolbar->adjustment), value);
}



/****************************************/
/********** Funciones privadas **********/
/****************************************/



/* Retrollamada de los items de la barra de menú */
static void Menu_Items(gpointer data, guint action, GtkWidget *widget)
{
	LifeGrid *lifegrid = data;
	
	switch(action) {
		case 11:
			Function_New(lifegrid);
			break;
		case 12:
			Function_Close(lifegrid);
			break;
		case 13:
			Function_Exit(lifegrid);
			break;
		case 21:
			Function_Zoom_In(lifegrid);
			break;
		case 22:
			Function_Zoom_Out(lifegrid);
			break;
		case 31:
			Function_Continue(lifegrid);
			break;
		case 32:
			Function_Stop(lifegrid);
			break;
		default:
			g_message("Función no implementada.\n");
	}
}



/* Retrollamada del botón de nuevo */
static void Button_New(GtkToolButton *toolbutton, gpointer data)
{
	Function_New((LifeGrid *)data);
}



/* Retrollamada del botón de continuar */
static void Button_Continue(GtkToolButton *toolbutton, gpointer data)
{
	Function_Continue((LifeGrid *)data);
}



/* Retrollamada del botón de detener */
static void Button_Stop(GtkToolButton *toolbutton, gpointer data)
{
	Function_Stop((LifeGrid *)data);
}



/* Retrollamada del botón de Zoom in */
static void Button_Zoom_In(GtkToolButton *toolbutton, gpointer data)
{
	Function_Zoom_In((LifeGrid *)data);
}



/* Retrollamada del botón de Zoom out */
static void Button_Zoom_Out(GtkToolButton *toolbutton, gpointer data)
{
	Function_Zoom_Out((LifeGrid *)data);
}



/* Retrollamada de la barra de escala */
static void Scale_Value_Changed(GtkRange *range, gpointer data)
{
	Function_Change_Delay((LifeGrid *)data, gtk_range_get_value(range)); 
}
