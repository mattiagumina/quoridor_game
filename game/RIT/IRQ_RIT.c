/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "RIT.h"
#include "../scacchiera/scacchiera.h"
#include "../timer/timer.h"
#include "../templates/templates.h"
#include "../gestCollBoards/gestCollBoards.h"

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

volatile int EINT0_down = 0;
volatile int EINT1_down = 0;
volatile int EINT2_down = 0;

extern int scacchiera[DIM_SCACCHIERA * 2][DIM_SCACCHIERA * 2];
extern int mod;

extern int partitaIniziata;
extern int giocatore;
extern int posizionePedine[2][2];
extern int coloriGiocatori[2];
extern int coloreMuroProvvisorio;;
extern int coloreMuro;
extern int modalitaMuro;
extern int posizioneMuro[2];
extern int orientamentoMuro;
extern int muroProvvisorio;
extern int ultimaMossa;
extern int scritto_m;
extern int coloreSfondoTempo;
extern unsigned char tempo_rimanente[];
extern char muriRimanenti[];
unsigned char s[] = "NO WALLS, MOVE THE TOKEN!";
unsigned char p1[] = "P1";
unsigned char p2[] = "P2";
unsigned char win_str[] = " WIN THE GAME!";
unsigned char you[] = "YOU";
extern int freq_ko;
extern int freq_ok;
extern int partitaFinita;
extern uint32_t codificaMossa;
extern int schermata;
extern int selezione;
extern int giocatoreUtente;
extern int mioId;
unsigned char sb[] = "Single Board";
unsigned char tb[] = "Two Boards";
unsigned char human[] = "Human";
unsigned char npc[] = "NPC";
unsigned char pl1[] = "Player 1";
unsigned char pl2[] = "Player 2";
unsigned char s1[] = "Connection failed!";
unsigned char s2[] = "Second board not found!";
unsigned char s3[] = "Select 'Single Board' mode";
extern unsigned char p[];


void RIT_IRQHandler (void)
{					
	static int J_select=0; 	// p1.25
	static int J_down = 0; 	// p1.26
	static int J_left = 0; 	// p1.27
	static int J_right = 0;	// p1.28
	static int J_up = 0;		// p1.29
	static int J_up_left = 0;
	static int J_up_right = 0;
	static int J_down_left = 0;
	static int J_down_right = 0;
	int i;
	
	
	if((LPC_GPIO1->FIOPIN & (1<<25)) == 0 && partitaIniziata && (mod == SINGLE_BOARD_V_HUMAN || (mod == SINGLE_BOARD_V_NPC && giocatore == giocatoreUtente) || (mod == TWO_BOARDS_HUMAN && giocatore == mioId))){	
		/* Joytick J_Select pressed p1.25*/
		
		J_select++;
		switch(J_select){
			case 1:
				if(confermaMossa()){
					codificaMossa = compilaCodificaMossa(1, giocatore, modalitaMuro, orientamentoMuro, posizioneMuro, posizionePedine[giocatore - 1]);
					if(mod == TWO_BOARDS_HUMAN || mod == TWO_BOARDS_NPC)
						inviaMossa(codificaMossa);
					if(!verificaVittoria()){
						disable_timer(0);
						reset_timer(0);
						turno();
					}
					else{
						disable_RIT();
						disable_timer(0);
						partitaFinita = 1;
						if(mod == SINGLE_BOARD_V_HUMAN){
							if(giocatore == 1)
								GUI_Text(9, 238, p1, coloriGiocatori[giocatore - 1], Yellow);
							else
								GUI_Text(9, 238, p2, coloriGiocatori[giocatore - 1], Yellow);
							GUI_Text(25, 238, win_str, Black, Yellow);
						}
						else if(mod == SINGLE_BOARD_V_NPC || mod == TWO_BOARDS_HUMAN || mod == TWO_BOARDS_NPC){
							GUI_Text(9, 238, you, Black, Yellow);
							GUI_Text(33, 238, win_str, Black, Yellow);
						}
						init_timer(3, freq_ok);
						enable_timer(3);
						enable_timer(1);
					}
				}
				break;
			default:
				break;
		}
	}
	else if((LPC_GPIO1->FIOPIN & (1<<25)) == 0){
		switch(schermata){
			case SEL_GAME_MODE:
				switch(selezione){
					case 0:
						templateSingleBoard();
						break;
					case 1:
						if(mioId == 0){
							GUI_Text(45, 265, s1, Black, Yellow);
							GUI_Text(25, 280, s2, Black, Yellow);
							GUI_Text(13, 295, s3, Black, Yellow);
						}
						else
							templateTwoBoard();
						break;
					default:
						break;
				}
				break;
			case SINGLE_BOARD:
				switch(selezione){
					case 0:
						mod = SINGLE_BOARD_V_HUMAN;
						disegnaScacchiera();
						configurazioneInizialePartita();
						turno();
						break;
					case 1:
						mod = SINGLE_BOARD_V_NPC;
						templateSelezionaGiocatoreUtente();
						break;
					default:
						break;
				}
				break;
			case SEL_GIOC_UTENTE:
				switch(selezione){
					case 0:
						giocatoreUtente = 1;
						iniziaPartita1vNPCcon1board();
						break;
					case 1:
						giocatoreUtente = 2;
						iniziaPartita1vNPCcon1board();
						break;
					default:
						break;
				}
				break;
			case TWO_BOARDS:
				switch(selezione){
					case 0:
						mod = TWO_BOARDS_HUMAN;
						giocatorePronto();
						break;
					case 1:
						mod = TWO_BOARDS_NPC;
						init_timer(2, 3 * 25000000);
						LPC_TIM2->MCR = 7;
						giocatorePronto();
						break;
					default:
						break;
				}
				break;
			default:
				break;
		}
	}
	else{
			J_select=0;
	}
		
	if((LPC_GPIO1->FIOPIN & (1<<26)) == 0 && (LPC_GPIO1->FIOPIN & (1<<27)) != 0 && (LPC_GPIO1->FIOPIN & (1<<28)) != 0 && partitaIniziata && (mod == SINGLE_BOARD_V_HUMAN || (mod == SINGLE_BOARD_V_NPC && giocatore == giocatoreUtente) || (mod == TWO_BOARDS_HUMAN && giocatore == mioId))){	
		/* Joytick J_Down pressed p1.26*/
		
		J_down++;
		switch(J_down){
			case 1:
				controllaMossa(DOWN);
				break;
			default:
				break;
		}
	}
	else if((LPC_GPIO1->FIOPIN & (1<<26)) == 0 && (LPC_GPIO1->FIOPIN & (1<<27)) != 0 && (LPC_GPIO1->FIOPIN & (1<<28)) != 0){
		switch(schermata){
			case SEL_GAME_MODE:
				if(selezione == 0){
					for(i = 161; i < 190; i++)
						LCD_DrawLine(61, i, 179, i, White);
					GUI_Text(72, 167, sb, Black, White);
					for(i = 211; i < 240; i++)
						LCD_DrawLine(61, i, 179, i, Yellow);
					GUI_Text(80, 217, tb, Black, Yellow);
					selezione = 1;
				}
				break;
			case SINGLE_BOARD:
				if(selezione == 0){
					for(i = 161; i < 190; i++)
						LCD_DrawLine(61, i, 179, i, White);
					GUI_Text(100, 167, human, Black, White);
					for(i = 211; i < 240; i++)
						LCD_DrawLine(61, i, 179, i, Yellow);
					GUI_Text(107, 217, npc, Black, Yellow);
					selezione = 1;
				}
				break;
			case TWO_BOARDS:
				if(selezione == 0){
					for(i = 161; i < 190; i++)
						LCD_DrawLine(61, i, 179, i, White);
					GUI_Text(100, 167, human, Black, White);
					for(i = 211; i < 240; i++)
						LCD_DrawLine(61, i, 179, i, Yellow);
					GUI_Text(107, 217, npc, Black, Yellow);
					selezione = 1;
				}
				break;
			case SEL_GIOC_UTENTE:
				if(selezione == 0){
					for(i = 161; i < 190; i++)
						LCD_DrawLine(61, i, 179, i, White);
					GUI_Text(80, 167, pl1, Black, White);
					GUI_Text(150, 167, p, coloriGiocatori[0], coloriGiocatori[0]);
					for(i = 211; i < 240; i++)
						LCD_DrawLine(61, i, 179, i, Yellow);
					GUI_Text(80, 217, pl2, Black, Yellow);
					GUI_Text(150, 217, p, coloriGiocatori[1], coloriGiocatori[1]);
					selezione = 1;
				}
				break;
			default:
				break;
		}
	}
	else{
			J_down=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<27)) == 0 && (LPC_GPIO1->FIOPIN & (1<<29)) != 0 && (LPC_GPIO1->FIOPIN & (1<<26)) != 0 && partitaIniziata && (mod == SINGLE_BOARD_V_HUMAN || (mod == SINGLE_BOARD_V_NPC && giocatore == giocatoreUtente) || (mod == TWO_BOARDS_HUMAN && giocatore == mioId))){	
		/* Joytick J_left pressed p1.25*/
		
		J_left++;
		switch(J_left){
			case 1:
				controllaMossa(LEFT);
				break;
			default:
				break;
		}
	}
	else{
			J_left=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<28)) == 0 && (LPC_GPIO1->FIOPIN & (1<<29)) != 0 && (LPC_GPIO1->FIOPIN & (1<<26)) != 0 && partitaIniziata && (mod == SINGLE_BOARD_V_HUMAN || (mod == SINGLE_BOARD_V_NPC && giocatore == giocatoreUtente) || (mod == TWO_BOARDS_HUMAN && giocatore == mioId))){	
		/* Joytick J_right pressed p1.25*/
		
		J_right++;
		switch(J_right){
			case 1:
				controllaMossa(RIGHT);
				break;
			default:
				break;
		}
	}
	else{
			J_right=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<29)) == 0 && (LPC_GPIO1->FIOPIN & (1<<27)) != 0 && (LPC_GPIO1->FIOPIN & (1<<28)) != 0 && partitaIniziata && (mod == SINGLE_BOARD_V_HUMAN || (mod == SINGLE_BOARD_V_NPC && giocatore == giocatoreUtente) || (mod == TWO_BOARDS_HUMAN && giocatore == mioId))){	
		/* Joytick J_up pressed p1.25*/
		
		J_up++;
		switch(J_up){
			case 1:
				controllaMossa(UP);
				break;
			default:
				break;
		}
	}
	else if((LPC_GPIO1->FIOPIN & (1<<29)) == 0 && (LPC_GPIO1->FIOPIN & (1<<27)) != 0 && (LPC_GPIO1->FIOPIN & (1<<28)) != 0){
		switch(schermata){
			case SEL_GAME_MODE:
				if(selezione == 1){
					for(i = 161; i < 190; i++)
						LCD_DrawLine(61, i, 179, i, Yellow);
					GUI_Text(72, 167, sb, Black, Yellow);
					for(i = 211; i < 240; i++)
						LCD_DrawLine(61, i, 179, i, White);
					GUI_Text(80, 217, tb, Black, White);
					selezione = 0;
				}
				break;
			case SINGLE_BOARD:
				if(selezione == 1){
					for(i = 161; i < 190; i++)
						LCD_DrawLine(61, i, 179, i, Yellow);
					GUI_Text(100, 167, human, Black, Yellow);
					for(i = 211; i < 240; i++)
						LCD_DrawLine(61, i, 179, i, White);
					GUI_Text(107, 217, npc, Black, White);
					selezione = 0;
				}
				break;
			case TWO_BOARDS:
				if(selezione == 1){
					for(i = 161; i < 190; i++)
						LCD_DrawLine(61, i, 179, i, Yellow);
					GUI_Text(100, 167, human, Black, Yellow);
					for(i = 211; i < 240; i++)
						LCD_DrawLine(61, i, 179, i, White);
					GUI_Text(107, 217, npc, Black, White);
					selezione = 0;
				}
				break;
			case SEL_GIOC_UTENTE:
				if(selezione == 1){
					for(i = 161; i < 190; i++)
						LCD_DrawLine(61, i, 179, i, Yellow);
					GUI_Text(80, 167, pl1, Black, Yellow);
					GUI_Text(150, 167, p, coloriGiocatori[0], coloriGiocatori[0]);
					for(i = 211; i < 240; i++)
						LCD_DrawLine(61, i, 179, i, White);
					GUI_Text(80, 217, pl2, Black, White);
					GUI_Text(150, 217, p, coloriGiocatori[1], coloriGiocatori[1]);
					selezione = 0;
				}
				break;
			default:
				break;
		}
	}
	else{
			J_up=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<29)) == 0 && (LPC_GPIO1->FIOPIN & (1<<27)) == 0 && partitaIniziata && (mod == SINGLE_BOARD_V_HUMAN || (mod == SINGLE_BOARD_V_NPC && giocatore == giocatoreUtente) || (mod == TWO_BOARDS_HUMAN && giocatore == mioId))){ // up-left
		
		J_up_left++;
		switch(J_up_left){
			case 1:
				controllaMossa(UP_LEFT);
				break;
			default:
				break;
		}
	}
	else{
		J_up_left = 0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<29)) == 0 && (LPC_GPIO1->FIOPIN & (1<<28)) == 0 && partitaIniziata && (mod == SINGLE_BOARD_V_HUMAN || (mod == SINGLE_BOARD_V_NPC && giocatore == giocatoreUtente) || (mod == TWO_BOARDS_HUMAN && giocatore == mioId))){ // up-right
		
		J_up_right++;
		switch(J_up_right){
			case 1:
				controllaMossa(UP_RIGHT);
				break;
			default:
				break;
		}
	}
	else{
		J_up_right = 0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<26)) == 0 && (LPC_GPIO1->FIOPIN & (1<<27)) == 0 && partitaIniziata && (mod == SINGLE_BOARD_V_HUMAN || (mod == SINGLE_BOARD_V_NPC && giocatore == giocatoreUtente) || (mod == TWO_BOARDS_HUMAN && giocatore == mioId))){ // down-left
		
		J_down_left++;
		switch(J_down_left){
			case 1:
				controllaMossa(DOWN_LEFT);
				break;
			default:
				break;
		}
	}
	else{
		J_down_left = 0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<26)) == 0 && (LPC_GPIO1->FIOPIN & (1<<28)) == 0 && partitaIniziata && (mod == SINGLE_BOARD_V_HUMAN || (mod == SINGLE_BOARD_V_NPC && giocatore == giocatoreUtente) || (mod == TWO_BOARDS_HUMAN && giocatore == mioId))){ // down-right
		
		J_down_right++;
		switch(J_down_right){
			case 1:
				controllaMossa(DOWN_RIGHT);
				break;
			default:
				break;
		}
	}
	else{
		J_down_right = 0;
	}
	
	/* button management */
	if(EINT0_down!=0){ 
		if((LPC_GPIO2->FIOPIN & (1<<10)) == 0 && !partitaIniziata && schermata == TEMPL_INIZ){	/* KEY1 pressed */
			EINT0_down++;				
			switch(EINT0_down){
				case 2:
					if(mioId == 0)
						verificaCollegamentoBoard();
					templateSelezionaGameMode();
					break;
				default:
					break;
			}
		}
		else {	/* button released */
			EINT0_down=0;			
			NVIC_EnableIRQ(EINT0_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt 0 pin selection */
		}
	}
	
	if(EINT1_down != 0){
		if((LPC_GPIO2->FIOPIN & (1 << 11)) == 0 && partitaIniziata && (mod == SINGLE_BOARD_V_HUMAN || (mod == SINGLE_BOARD_V_NPC && giocatore == giocatoreUtente) || (mod == TWO_BOARDS_HUMAN && giocatore == mioId))){
			EINT1_down++;
			switch(EINT1_down){
				case 2:
					if(muriRimanenti[giocatore - 1] > '0'){
						if(!modalitaMuro){
							passaModalitaMuro();
						}
						else{
							esciModalitaMuro();
						}
					}
					else{
						GUI_Text(9, 238, s, Black, Yellow);
						enable_timer(3);
						enable_timer(1);
					}
					break;
				default:
					break;
			}
		}
		else{
			EINT1_down = 0;
			NVIC_EnableIRQ(EINT1_IRQn);
			LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
		}
	}
	
	if(EINT2_down != 0){
		if((LPC_GPIO2->FIOPIN & (1 << 12)) == 0 && partitaIniziata && modalitaMuro && (mod == SINGLE_BOARD_V_HUMAN || (mod == SINGLE_BOARD_V_NPC && giocatore == giocatoreUtente) || (mod == TWO_BOARDS_HUMAN && giocatore == mioId))){
			EINT2_down++;
			switch(EINT2_down){
				case 2:
					ruotaMuro();
					break;
				default:
					break;
			}
		}
		else{
			EINT2_down = 0;
			NVIC_EnableIRQ(EINT2_IRQn);
			LPC_PINCON->PINSEL4 |= (1 << 24);
		}
	}
	
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
