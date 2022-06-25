
#ifndef __VIDA_H__
#define __VIDA_H__

#include "misc.h"



typedef char State;

enum State {
	Alive, /* Vivo */
	Dead /* Muerto */
};



State * Life_Initial_State(uint32, uint32);
State * Life_Evolution(State *, uint32, uint32);
void Life_Terminate(State *states);


#endif
