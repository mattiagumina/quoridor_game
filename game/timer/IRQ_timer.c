/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include <string.h>
#include "lpc17xx.h"
#include "timer.h"
#include "../GLCD/GLCD.h" 
#include "../scacchiera/scacchiera.h"

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

unsigned char * decrementa(unsigned char * t);

extern unsigned char tempo_rimanente[];
extern unsigned char pedina[];
extern int giocatore;
extern int coloriGiocatori[];
extern int scritto_m;
extern char muriRimanenti[2];
extern int modalitaMuro;
extern int coloreSfondoTempo;
extern int partitaFinita;
extern int partitaIniziata;
extern int orientamentoMuro;
extern int posizioneMuro[2];
extern int posizionePedine[2][2];
extern uint32_t codificaMossa;
extern int mod;
extern int mioId;
extern unsigned char npc[];
extern unsigned char win_str[];
extern unsigned char you[];

int fineTurno = 0;
uint16_t SinTable[45] =                                       /*                      */
{
    410, 467, 523, 576, 627, 673, 714, 749, 778,
    799, 813, 819, 817, 807, 789, 764, 732, 694, 
    650, 602, 550, 495, 438, 381, 324, 270, 217,
    169, 125, 87 , 55 , 30 , 12 , 2  , 0  , 6  ,   
    20 , 41 , 70 , 105, 146, 193, 243, 297, 353
};

void TIMER0_IRQHandler (void)
{
	int i;
	unsigned char s[] = "TIME OUT!!!";
	decrementa(tempo_rimanente);
	if(tempo_rimanente[0] == '0' && tempo_rimanente[1] == '5'){
		coloreSfondoTempo = Yellow;
	}
	GUI_Text(110, 275, tempo_rimanente, Black, coloreSfondoTempo);
	if(tempo_rimanente[0] == '0' && tempo_rimanente[1] == '0'){
		disable_timer(0);
		for(i = 235; i < 255; i++){
			LCD_DrawLine(0, i, 240, i, White);
		}
		GUI_Text(9, 238, s, Black, Yellow);
		enable_timer(3);
		enable_timer(1);
		fineTurno = 1;
	}
  LPC_TIM0->IR = 1;			/* clear interrupt flag */
  return;
}

unsigned char * decrementa(unsigned char str_time[]){
	int int_time = 0;
	int_time = ((int) (str_time[0] - '0') * 10) + (int) (str_time[1] - '0');
	int_time--;
	str_time[0] = (int_time / 10) + '0';
	str_time[1] = (int_time % 10) + '0';
	return str_time;
}

/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER1_IRQHandler (void)
{
	int i;
	unsigned char s[] = "Turn of: ";
	unsigned char s1[] = "You are: ";
	unsigned char s2[] = "It's your turn";
	unsigned char s3[] = "It's opponent's turn";
	disable_timer(3);
	reset_timer(3);
	if(!partitaFinita){
		for(i = 235; i < 255; i++){
			LCD_DrawLine(0, i, 240, i, White);
		}
		if(mod == SINGLE_BOARD_V_HUMAN)
			GUI_Text(9, 238, s, Black, White);
		else if(mod == SINGLE_BOARD_V_NPC){
			GUI_Text(9, 238, s1, Black, White);
			GUI_Text(81, 238, pedina, coloriGiocatori[giocatore - 1], coloriGiocatori[giocatore - 1]);
		}
		else if(mod == TWO_BOARDS_HUMAN){
			if(giocatore == mioId){
				GUI_Text(9, 238, s2, Black, White);
				GUI_Text(125, 238, pedina, coloriGiocatori[mioId - 1], coloriGiocatori[mioId - 1]);
			}
			else{
				GUI_Text(9, 238, s3, Black, White);
				GUI_Text(181, 238, pedina, coloriGiocatori[mioId - 1], coloriGiocatori[mioId - 1]);
			}
		}
		if(fineTurno){
			codificaMossa = compilaCodificaMossa(0, giocatore, modalitaMuro, orientamentoMuro, posizioneMuro, posizionePedine[giocatore - 1]);
			if(mod == TWO_BOARDS_HUMAN)
				inviaMossa(codificaMossa);
			turno();
			fineTurno = 0;
		}
		else if(mod == SINGLE_BOARD_V_HUMAN)
			GUI_Text(81, 238, pedina, coloriGiocatori[giocatore - 1], coloriGiocatori[giocatore - 1]);
	}
  LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}

void TIMER2_IRQHandler(void){
	uint32_t mossa;
	if(modalitaMuro){
		if(scritto_m){
			if(giocatore == 1)
				PutChar(40, 280, muriRimanenti[0], White, White);
			else
				PutChar(194, 280, muriRimanenti[1], White, White);
			scritto_m = 0;
		}
		else{
			if(giocatore == 1)
				PutChar(40, 280, muriRimanenti[0], Black, White);
			else
				PutChar(194, 280, muriRimanenti[1], Black, White);
			scritto_m = 1;
		}
	}
	else if(mod == TWO_BOARDS_NPC && giocatore == mioId){
		mossa = generaMossaNPC();
		inserisciMossaNellaScacchiera(mossa);
		inviaMossa(mossa);
		if(verificaVittoria()){
			GUI_Text(9, 238, you, Black
			, Yellow);
			GUI_Text(33, 238, win_str, Black, Yellow);
			disable_RIT();
			disable_timer(0);
			partitaFinita = 1;
			enable_timer(3);
			enable_timer(1);
		}
		else
			turno();
	}
	LPC_TIM2->IR = 1; // clear interrupt flag
	return;
}

void TIMER3_IRQHandler(void){
 	static int ticks = 0;
	LPC_DAC->DACR = SinTable[ticks]<<6;
	ticks++;
	if(ticks==45) ticks=0;

	
  LPC_TIM0->IR = 1;			/* clear interrupt flag */
	
	LPC_TIM3->IR = 1;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
