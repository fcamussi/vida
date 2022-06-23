
#ifndef __BARS_H__
#define __BARS_H__

#include <gtk/gtk.h>
#include "grid.h"


#define SCALE_VALUE_MIN 50
#define SCALE_VALUE_MAX 1000
#define SCALE_STEP_INCREMENT 10
#define SCALE_PAGE_INCREMENT 100




typedef struct _MenuBar MenuBar;

struct _MenuBar
{
	/* Datos privados */
	GtkWidget *menubar;
	GtkAccelGroup *agroup;
	GtkItemFactory *ifactory;
};


typedef struct _ToolBar ToolBar;

struct _ToolBar
{
	/* Datos privados */
	GtkWidget *toolbar;
	GtkObject *adjustment;
	GtkWidget *hscale;
	GtkToolItem *bnew;
	GtkToolItem *bcontinue;
	GtkToolItem *bstop;
	GtkToolItem *bzoomin;
	GtkToolItem *bzoomout;
	GtkToolItem *scale;
};



GtkWidget * Menu_Create(MenuBar *, LifeGrid *lifegrid);
GtkAccelGroup * Menu_Get_Accel_Group(MenuBar *);
GtkWidget * Toolbar_Create(ToolBar *, LifeGrid *lifegrid);
void Toolbar_Scale_Set_Value(ToolBar *, uint32);


#endif
