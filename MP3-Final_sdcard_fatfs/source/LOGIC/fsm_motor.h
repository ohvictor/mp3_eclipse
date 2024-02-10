#ifndef SOURCES_FSM_H_
#define SOURCES_FSM_H_

#define TABLE_END 0xFF

#include <stdlib.h>

typedef	int BYTE;
typedef struct statesTable STATE;

struct statesTable
{
	BYTE event;
	STATE *nextState;
	void (*pToAction)(void);
};


STATE* fsm(STATE *pStatesTable, int currentEvent);

#endif /* SOURCES_FSM_H_ */
