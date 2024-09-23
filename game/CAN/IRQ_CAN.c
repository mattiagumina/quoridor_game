/*----------------------------------------------------------------------------
 * Name:    Can.c
 * Purpose: CAN interface for for LPC17xx with MCB1700
 * Note(s): see also http://www.port.de/engl/canprod/sv_req_form.html
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2009 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <lpc17xx.h>                  /* LPC17xx definitions */
#include "CAN.h"                      /* LPC17xx CAN adaption layer */
#include "../GLCD/GLCD.h"
#include "../gestCollBoards/gestCollBoards.h"

extern uint8_t icr ; 										//icr and result must be global in order to work with both real and simulated landtiger.
extern uint32_t result;
extern CAN_msg       CAN_TxMsg;    /* CAN message for sending */
extern CAN_msg       CAN_RxMsg;    /* CAN message for receiving */                                

extern int mioId;
extern int pronto;
extern int partitaFinita;
extern int freq_ok;

uint16_t val_RxCoordX = 0;            /* Locals used for display */
uint16_t val_RxCoordY = 0;

/*----------------------------------------------------------------------------
  CAN interrupt handler
 *----------------------------------------------------------------------------*/
void CAN_IRQHandler (void)  {
	int i;
	uint32_t mossa = 0;
	uint8_t playerId;
	unsigned char lose_str[] = "YOU LOSE THE GAME :(";
	icr = 0;
  icr = (LPC_CAN1->ICR | icr) & 0xFF;               /* clear interrupts */
	
  if (icr & (1 << 0)) {                          		/* CAN Controller #1 meassage is received */
		CAN_rdMsg (1, &CAN_RxMsg);	                		/* Read the message */
    LPC_CAN1->CMR = (1 << 2);                    		/* Release receive buffer */
		playerId = CAN_RxMsg.data[0];
		mossa |= (CAN_RxMsg.data[0] << 24);
		mossa |= (CAN_RxMsg.data[1] << 16);
		mossa |= (CAN_RxMsg.data[2] << 8);
		mossa |= (CAN_RxMsg.data[3]);
		
		if(playerId == 0xFF && mioId == 0)
			mioId = 2;
		else if(playerId == 0xFF && pronto < 2){
			pronto++;
			if(pronto == 2)
				turno();
		}
		else if((playerId + 1) != mioId){
			inserisciMossaNellaScacchiera(mossa);
			if(verificaVittoria()){
				disable_RIT();
				disable_timer(0);
				partitaFinita = 1;
				for(i = 235; i < 255; i++){
					LCD_DrawLine(0, i, 240, i, White);
				}
				GUI_Text(9, 238, lose_str, Black, Yellow);
				init_timer(3, freq_ok);
				enable_timer(3);
				enable_timer(1);
			}
			else
				turno();
		}
		
  }
	else{
		if (icr & (1 << 1)) {                         /* CAN Controller #1 meassage is transmitted */
			playerId = CAN_TxMsg.data[0];
			if(playerId == 0xFF && mioId == 0)
				mioId = 1;
			else if(playerId == 0xFF && pronto < 2){
				pronto++;
				if(pronto == 2)
					turno();
			}
		}
	}
}
