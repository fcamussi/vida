
#include <stdlib.h>
#include <time.h>
#include "life.h"
#include "misc.h"



/* Funciones privadas */
static int Adjacents_Cells(State *, uint32, uint32, uint32, uint32);




/* Setea aleatoriamente la matriz de estados */
State * Life_Initial_State(uint32 rows, uint32 columns)
{
	State *states;
	uint32 cells = rows * columns;
	uint32 t = cells / 4; /* Cantidad de bichos */
	uint32 c;
	uint32 tmp;
	
	states = malloc(cells * sizeof(State));

	for (c = 0; c < cells; c++)
		states[c] = Dead;

	srand(time(0));
	c = 0;
	while (c < t) {
		tmp = rand() % cells;
		if (states[tmp] == Dead) {
			states[tmp] = Alive;
			c++;
		}
	}
	
	/*
	states[0] = Alive;
	states[cells - 1] = Alive;*/
	
	return states;
}



/* Esta función se ejecuta cada cierto periodo y modifica la matriz de
estados */
State * Life_Evolution(State *states, uint32 rows, uint32 columns)
{	
	State *new_states;
	uint32 cells = rows * columns;
	uint32 c, d, pos = 0;

	new_states = malloc(cells * sizeof(State));
	
	for (c = 1; c <= rows; c++)
	{
		for(d = 1; d <= columns; d++)
		{
			if (states[pos] == Dead) {
				if (Adjacents_Cells(states, rows, columns, c, d) == 3)
					new_states[pos] = Alive;
				else
					new_states[pos] = Dead;
			}
			else
			{
				if (Adjacents_Cells(states, rows, columns, c, d) < 2 ||
					Adjacents_Cells(states, rows, columns, c, d) > 3)
				{
					new_states[pos] = Dead;
				}
				else
				{
					new_states[pos] = Alive;
				}
			}
			pos++;
		}
	}

	free(states);
	return new_states;
}



/* Libera memoria */
void Life_Terminate(State *states)
{
	free(states);
}




/****************************************/
/********** Funciones privadas **********/
/****************************************/



/* Retorna la cantidad de células adyacentes */
/* Se podría mejorar */
static int Adjacents_Cells(State *states, uint32 rows, uint32 columns,
						   uint32 row, uint32 column)
{
	uint32 _row = row - 1;
	uint32 _column = column - 1;
	int c = 0;
	
	if (row > 1) {
		if (states[(_row - 1) * columns + _column] == Alive)
			c++;
	}
	if (column > 1) {
		if (states[_row * columns + (_column - 1)] == Alive)
			c++;
	}
	if (row > 1 && column > 1) {
		if (states[(_row - 1) * columns + (_column - 1)] == Alive)
			c++;
	}

	if (row < rows) {
		if (states[(_row + 1) * columns + _column] == Alive)
			c++;
	}
	if (column < columns) {
		if (states[_row * columns + (_column + 1)] == Alive)
			c++;
	}
	if (row < rows && column < columns) {
		if (states[(_row + 1) * columns + (_column + 1)] == Alive)
			c++;
	}

	if (row > 1 && column < columns) {
		if (states[(_row - 1) * columns + (_column + 1)] == Alive)
			c++;
	}
	if (row < rows && column > 1) {
		if (states[(_row + 1) * columns + (_column - 1)] == Alive)
			c++;
	}

	return c;
}
