/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The GLCD application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-7
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             Paolo Bernardi
** Modified date:           03/01/2020
** Version:                 v2.0
** Descriptions:            basic program for LCD and Touch Panel teaching
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "LPC17xx.h"
#include "GLCD/GLCD.h" 
#include "timer/timer.h"
#include "scacchiera/scacchiera.h"
#include "button_EXINT/button.h"
#include "RIT/RIT.h"
#include "joystick/joystick.h"
#include "templates/templates.h"
#include "CAN/CAN.h"

//#define SIMULATOR 1

#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif

volatile int partitaIniziata = 0;
volatile int giocatore;
volatile int posizionePedine[2][2];
volatile int coloriGiocatori[2] = {Red, Blue};
volatile int coloreMuroProvvisorio = Cyan;
volatile int coloreMuro = Magenta;
volatile int modalitaMuro;
volatile int posizioneMuro[2];
volatile int orientamentoMuro;
volatile int muroProvvisorio;
volatile int ultimaMossa;
volatile int scritto_m;
volatile int coloreSfondoTempo = White;
volatile unsigned char tempo_rimanente[] = "20";
volatile char muriRimanenti[] = {'8', '8'};
volatile int freq_ko = 1592;
volatile int freq_ok = 1062;
volatile int partitaFinita = 0;
volatile uint32_t codificaMossa = 0;
volatile int schermata;
volatile int selezione;
volatile int giocatoreUtente;
volatile unsigned char p[] = "  ";

int main(void)
{
  SystemInit();  												/* System Initialization (i.e., PLL)  */
	BUTTON_init();
	joystick_init();
	CAN_Init();
	LCD_Initialization();
	
	init_RIT(0x004C4B40);									/* RIT Initialization 50 msec       */
	init_timer(0, 1.0*25000000);
	init_timer(1, 1.0*25000000);
	init_timer(2, 0.5*25000000);
	init_timer(3, freq_ko);
	
	templateIniziale();
	enable_RIT();
	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);

	LPC_PINCON->PINSEL1 |= (1<<21);
	LPC_PINCON->PINSEL1 &= ~(1<<20);
	LPC_GPIO0->FIODIR |= (1<<26);
	
  while (1)	
  {
		__ASM("wfi");
  }
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
