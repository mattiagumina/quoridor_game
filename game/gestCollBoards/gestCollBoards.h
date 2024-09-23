#include "../CAN/CAN.h"
#include "../GLCD/GLCD.h"
#include "../templates/templates.h"
#include "../scacchiera/scacchiera.h"
#include "../timer/timer.h"

#define SINGLE_BOARD_V_HUMAN 0
#define SINGLE_BOARD_V_NPC 1
#define TWO_BOARDS_HUMAN 2
#define TWO_BOARDS_NPC 3

void verificaCollegamentoBoard(void);
void iniziaPartita1vNPCcon1board(void);
uint32_t generaMossaNPC(void);
void inserisciMossaNellaScacchiera(uint32_t mossa);
void inviaMossa(uint32_t mossa);
void giocatorePronto(void);
