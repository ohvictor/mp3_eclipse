#include "fsm_motor.h"

STATE* fsm(STATE *pStatesTable,int currentEvent)

{
	while (pStatesTable -> event != currentEvent && pStatesTable -> event != TABLE_END)
	{
		++pStatesTable;
	}
	(*pStatesTable -> pToAction) ();
	pStatesTable=pStatesTable -> nextState;

	return(pStatesTable);

}
