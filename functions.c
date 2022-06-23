
#include "functions.h"
#include "grid.h"
#include "misc.h"



void Function_Exit(LifeGrid *lifegrid)
{
	Function_Close(lifegrid);
	gtk_main_quit();
}



void Function_New(LifeGrid *lifegrid)
{
	Function_Close(lifegrid);
	Grid_Create(lifegrid, 400, 600);
}



void Function_Close(LifeGrid *lifegrid)
{
	if (Grid_Is_Evolving(lifegrid))
	{
		Grid_Stop_Evolution(lifegrid);
	}
	if (Grid_Is_Created())
	{
		Grid_Destroy(lifegrid);
	}
}



void Function_Zoom_In(LifeGrid *lifegrid)
{
	if (Grid_Is_Created())
	{
		if (Grid_Get_Zoom_Level(lifegrid) < ZOOM_LEVEL_MAX)
		{
			Grid_Zoom_In(lifegrid);
		}
	}
}	



void Function_Zoom_Out(LifeGrid *lifegrid)
{
	if (Grid_Is_Created())
	{
		if (Grid_Get_Zoom_Level(lifegrid) > 1)
		{
			Grid_Zoom_Out(lifegrid);
		}
	}
}	



void Function_Continue(LifeGrid *lifegrid)
{
	if (Grid_Is_Created())
	{
		if (! Grid_Is_Evolving(lifegrid))
		{
			Grid_Start_Evolution(lifegrid);
		}
	}
}



void Function_Stop(LifeGrid *lifegrid)
{
	if (Grid_Is_Created())
	{
		if (Grid_Is_Evolving(lifegrid))
		{
			Grid_Stop_Evolution(lifegrid);
		}
	}
}



void Function_Change_Delay(LifeGrid *lifegrid, uint32 delay)
{
	if (Grid_Is_Created())
	{
		Grid_Change_Delay(lifegrid, delay);
		if (Grid_Is_Evolving(lifegrid))
		{
			Grid_Stop_Evolution(lifegrid);
			Grid_Start_Evolution(lifegrid);
		}
	}
}
