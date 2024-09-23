#include "../GLCD/GLCD.h"
#include "../timer/timer.h"
#include "../RIT/RIT.h"
#include "../templates/templates.h"

#define SINGLE_BOARD_V_HUMAN 0
#define SINGLE_BOARD_V_NPC 1
#define TWO_BOARDS_HUMAN 2
#define TWO_BOARDS_NPC 3

#define DIM_SCACCHIERA 7
#define VERTICALE 0
#define ORIZZONTALE 1
#define DOWN 0
#define LEFT 1
#define RIGHT 2
#define UP 3
#define UP_LEFT 4
#define UP_RIGHT 5
#define DOWN_LEFT 6
#define DOWN_RIGHT 7

void disegnaScacchiera(void);
void inserisciPedina(int x, int y, int colore);
void rimuoviPedina(int x, int y);
void inserisciMuro(int x, int y, int colore, int orientameno);
void rimuoviMuro(int x, int y, int orientamento);
void configurazioneInizialePartita(void);
void coloraCella(int x, int y, int colore);
void turno(void);
void passaModalitaMuro(void);
void esciModalitaMuro(void);
void ruotaMuro(void);
void controllaMossa(int direzione);
int confermaMossa(void);
int verificaVittoria(void);
uint32_t compilaCodificaMossa(int valida, int g, int mm, int om, int pm[2], int pp[2]);
int muroIntrappolaGiocatori(void);
void ricercaPercorsoOttimo(uint8_t g, uint8_t x, uint8_t y, uint8_t s[DIM_SCACCHIERA][DIM_SCACCHIERA], uint8_t p[2], uint8_t l);

extern uint32_t generaMossaNPC(void);
extern void inserisciMossaNellaScacchiera(uint32_t);
extern void inviaMossa(uint32_t mossa);
