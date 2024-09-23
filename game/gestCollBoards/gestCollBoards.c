#include "gestCollBoards.h"

volatile int mioId = 0;
volatile int mod;
volatile int mossePercorsoOttimo[2][DIM_SCACCHIERA * DIM_SCACCHIERA];
volatile int lunghezzaPercorsoOttimo[2];
volatile int pronto = 0;

extern int scacchiera[DIM_SCACCHIERA * 2][DIM_SCACCHIERA * 2];
extern int coloriGiocatori[2];
extern int giocatoreUtente;
extern int posizionePedine[2][2];
extern char muriRimanenti[2];
extern int giocatore;
extern int posizioneMuro[2];
extern int orientamentoMuro;
extern int modalitaMuro;
extern int coloreMuro;
extern char muriRimanenti[];

int nuovePosizioni[2];
extern unsigned char p[];

void verificaCollegamentoBoard(void){
	CAN_TxMsg.data[0] = 0xFF;
	CAN_TxMsg.len = 4;
	CAN_TxMsg.id = 1;
	CAN_TxMsg.format = STANDARD_FORMAT;
	CAN_TxMsg.type = DATA_FRAME;
	CAN_wrMsg(1, &CAN_TxMsg);
}

void iniziaPartita1vNPCcon1board(){
	disegnaScacchiera();
	configurazioneInizialePartita();
	turno();
}

void muoviPedina(){
	int x, y;
	x = posizionePedine[giocatore - 1][0];
	y = posizionePedine[giocatore - 1][1];
	nuovePosizioni[0] = x;
	nuovePosizioni[1] = y;
	switch(mossePercorsoOttimo[giocatore - 1][0]){
		case UP:
			if(scacchiera[(y - 1) * 2][x * 2] == 0){
				nuovePosizioni[1] = y - 1;
			}
			else if(y - 2 >= 0 && scacchiera[((y - 1) * 2) - 1][x * 2] != 4){
				nuovePosizioni[1] = y - 2;
			}
			else if(x - 1 >= 0 || x + 1 < DIM_SCACCHIERA){
				if(mossePercorsoOttimo[giocatore - 1][1] == LEFT){ // up-left
					nuovePosizioni[0] = x - 1;
					nuovePosizioni[1] = y - 1;
				}
				else{ // up-right
					nuovePosizioni[0] = x - 1;
					nuovePosizioni[1] = y - 1;
				}
			}
			break;
		case LEFT:
			if(scacchiera[y * 2][(x - 1) * 2] == 0){
				nuovePosizioni[0] = x - 1;
			}
			else if(x - 2 >= 0 && scacchiera[y * 2][((x - 1) * 2) - 1] != 4){
				nuovePosizioni[0] = x - 2;
			}
			else if(y - 1 >= 0 || y + 1 < DIM_SCACCHIERA){
				if(mossePercorsoOttimo[giocatore - 1][1] == UP){ // left-up
					nuovePosizioni[0] = x - 1;
					nuovePosizioni[1] = y - 1;
				}
				else{ // left-down
					nuovePosizioni[0] = x - 1;
					nuovePosizioni[1] = y + 1;
				}
			}
			break;
		case RIGHT:
			if(scacchiera[y * 2][(x + 1) * 2] == 0){
				nuovePosizioni[0] = x + 1;
			}
			else if(x + 2 < DIM_SCACCHIERA && scacchiera[y * 2][((x + 1) * 2) + 1] != 4){
				nuovePosizioni[0] = x + 2;
			}
			else if(y - 1 >= 0 || y + 1 < DIM_SCACCHIERA){
				if(mossePercorsoOttimo[giocatore - 1][1] == UP){ // right-up
					nuovePosizioni[0] = x + 1;
					nuovePosizioni[1] = y - 1;
				}
				else{ // right-down
					nuovePosizioni[0] = x + 1;
					nuovePosizioni[1] = y + 1;
				}
			}
			break;
		case DOWN:
			if(scacchiera[(y + 1) * 2][x * 2] == 0){
				nuovePosizioni[1] = y + 1;
			}
			else if(y + 2 < DIM_SCACCHIERA && scacchiera[((y + 1) * 2) + 1][x * 2] != 4){
				nuovePosizioni[1] = y + 2;
			}
			else if(x - 1 >= 0 || x + 1 < DIM_SCACCHIERA){
				if(mossePercorsoOttimo[giocatore - 1][1] == LEFT){ // down-left
					nuovePosizioni[0] = x - 1;
					nuovePosizioni[1] = y + 1;
				}
				else{ // down-right
					nuovePosizioni[0] = x + 1;
					nuovePosizioni[1] = y + 1;
				}
			}
			break;
	}
}

void provaAdInserireMuro(void){
	int x, y, i;
	int messo = 0;
	x = posizionePedine[giocatoreUtente - 1][0];
	y = posizionePedine[giocatoreUtente - 1][1];
	for(i = 0; i < lunghezzaPercorsoOttimo[giocatoreUtente - 1] && !messo; i++){
		switch(mossePercorsoOttimo[giocatoreUtente - 1][i]){
			case UP:
				if(((i + 1 < lunghezzaPercorsoOttimo[giocatoreUtente - 1] && mossePercorsoOttimo[giocatoreUtente - 1][i + 1] == LEFT) || !(x + 1 < DIM_SCACCHIERA && scacchiera[(y * 2) - 1][(x * 2)] == 0 && scacchiera[(y * 2) - 1][(x * 2) + 1] == 0 && scacchiera[(y * 2) - 1][(x * 2) + 2] == 0) || (i - 1 >= 0 && mossePercorsoOttimo[giocatoreUtente - 1][i - 1] == LEFT)) && x - 1 >= 0 && scacchiera[(y * 2) - 1][(x * 2) - 2] == 0 && scacchiera[(y * 2) - 1][(x * 2) - 1] == 0 && scacchiera[(y * 2) - 1][(x * 2)] == 0){
					posizioneMuro[0] = x - 1;
					posizioneMuro[1] = y - 1;
					orientamentoMuro = ORIZZONTALE;
					if(!muroIntrappolaGiocatori()){
						messo = 1;
						modalitaMuro = 1;
					}
				}
				else if(x + 1 < DIM_SCACCHIERA && scacchiera[(y * 2) - 1][(x * 2)] == 0 && scacchiera[(y * 2) - 1][(x * 2) + 1] == 0 && scacchiera[(y * 2) - 1][(x * 2) + 2] == 0){
					posizioneMuro[0] = x;
					posizioneMuro[1] = y - 1;
					orientamentoMuro = ORIZZONTALE;
					if(!muroIntrappolaGiocatori()){
						messo = 1;
						modalitaMuro = 1;
					}
				}
				y--;
				break;
			case LEFT:
				if(((i + 1 < lunghezzaPercorsoOttimo[giocatoreUtente - 1] && mossePercorsoOttimo[giocatoreUtente - 1][i + 1] == UP) || !(y + 1 < DIM_SCACCHIERA && scacchiera[(y * 2)][(x * 2) - 1] == 0 && scacchiera[(y * 2) + 1][(x * 2) - 1] == 0 && scacchiera[(y * 2) + 2][(x * 2) - 1] == 0) || (i - 1 >= 0 && mossePercorsoOttimo[giocatoreUtente -1][i - 1] == UP)) && y - 1 >= 0 && scacchiera[(y * 2) - 2][(x * 2) - 1] == 0 && scacchiera[(y * 2) - 1][(x * 2) - 1] == 0 && scacchiera[(y * 2)][(x * 2) - 1] == 0){
					posizioneMuro[0] = x - 1;
					posizioneMuro[1] = y - 1;
					orientamentoMuro = VERTICALE;
					if(!muroIntrappolaGiocatori()){
						messo = 1;
						modalitaMuro = 1;
					}
				}
				else if(y + 1 < DIM_SCACCHIERA && scacchiera[(y * 2)][(x * 2) - 1] == 0 && scacchiera[(y * 2) + 1][(x * 2) - 1] == 0 && scacchiera[(y * 2) + 2][(x * 2) - 1] == 0){
					posizioneMuro[0] = x - 1;
					posizioneMuro[1] = y;
					orientamentoMuro = VERTICALE;
					if(!muroIntrappolaGiocatori()){
						messo = 1;
						modalitaMuro = 1;
					}
				}
				x--;
				break;
			case RIGHT:
				if(((i + 1 < lunghezzaPercorsoOttimo[giocatoreUtente - 1] && mossePercorsoOttimo[giocatoreUtente - 1][i + 1] == UP) || !(y + 1 < DIM_SCACCHIERA && scacchiera[(y * 2)][(x * 2) + 1] == 0 && scacchiera[(y * 2) + 1][(x * 2) + 1] == 0 && scacchiera[(y * 2) + 2][(x * 2) + 1] == 0) || (i - 1 >= 0 && mossePercorsoOttimo[giocatoreUtente - 1][i - 1] == UP)) && y - 1 >= 0 && scacchiera[(y * 2) - 2][(x * 2) + 1] == 0 && scacchiera[(y * 2) - 1][(x * 2) + 1] == 0 && scacchiera[(y * 2)][(x * 2) + 1] == 0){
					posizioneMuro[0] = x;
					posizioneMuro[1] = y - 1;
					orientamentoMuro = VERTICALE;
					if(!muroIntrappolaGiocatori()){
						messo = 1;
						modalitaMuro = 1;
					}
				}
				else if(y + 1 < DIM_SCACCHIERA && scacchiera[(y * 2)][(x * 2) + 1] == 0 && scacchiera[(y * 2) + 1][(x * 2) + 1] == 0 && scacchiera[(y * 2) + 2][(x * 2) + 1] == 0){
					posizioneMuro[0] = x;
					posizioneMuro[1] = y;
					orientamentoMuro = VERTICALE;
					if(!muroIntrappolaGiocatori()){
						messo = 1;
						modalitaMuro = 1;
					}
				}
				x++;
				break;
			case DOWN:
				if(((i + 1 < lunghezzaPercorsoOttimo[giocatoreUtente - 1] && mossePercorsoOttimo[giocatoreUtente - 1][i + 1] == LEFT) || !(x + 1 < DIM_SCACCHIERA && scacchiera[(y * 2) + 1][(x * 2)] == 0 && scacchiera[(y * 2) + 1][(x * 2) + 1] == 0 && scacchiera[(y * 2) + 1][(x * 2) + 2] == 0) || (i - 1 >= 0 && mossePercorsoOttimo[giocatoreUtente - 1][i - 1] == LEFT)) && x - 1 >= 0 && scacchiera[(y * 2) + 1][(x * 2) - 2] == 0 && scacchiera[(y * 2) + 1][(x * 2) - 1] == 0 && scacchiera[(y * 2) + 1][(x * 2)] == 0){
					posizioneMuro[0] = x - 1;
					posizioneMuro[1] = y;
					orientamentoMuro = ORIZZONTALE;
					if(!muroIntrappolaGiocatori()){
						messo = 1;
						modalitaMuro = 1;
					}
				}
				else if(x + 1 < DIM_SCACCHIERA && scacchiera[(y * 2) + 1][(x * 2)] == 0 && scacchiera[(y * 2) + 1][(x * 2) + 1] == 0 && scacchiera[(y * 2) + 1][(x * 2) + 2] == 0){
					posizioneMuro[0] = x;
					posizioneMuro[1] = y;
					orientamentoMuro = ORIZZONTALE;
					if(!muroIntrappolaGiocatori()){
						messo = 1;
						modalitaMuro = 1;
					}
				}
				y++;
				break;
			default:
				break;
		}
	}
	if(!messo)
		muoviPedina();
}

uint32_t generaMossaNPC(void){
	int i, j;
	uint8_t x_utente, y_utente, x_NPC, y_NPC, scacchieraGiaEsplorata[DIM_SCACCHIERA][DIM_SCACCHIERA], v[DIM_SCACCHIERA * DIM_SCACCHIERA];
	uint32_t mossa;
	x_utente = posizionePedine[giocatoreUtente - 1][0];
	y_utente = posizionePedine[giocatoreUtente - 1][1];
	for(i = 0; i < DIM_SCACCHIERA; i++)
		for(j = 0; j < DIM_SCACCHIERA; j++)
			scacchieraGiaEsplorata[i][j] = 0;
	lunghezzaPercorsoOttimo[giocatoreUtente - 1] = 1000; // valore alto così aggiorna subito al primo percorso che trova
	ricercaPercorsoOttimo(giocatoreUtente - 1, x_utente, y_utente, scacchieraGiaEsplorata, v, 0);
	x_NPC = posizionePedine[(3 - giocatoreUtente) - 1][0];
	y_NPC = posizionePedine[(3 - giocatoreUtente) - 1][1];
	for(i = 0; i < DIM_SCACCHIERA; i++)
		for(j = 0; j < DIM_SCACCHIERA; j++)
			scacchieraGiaEsplorata[i][j] = 0;
	lunghezzaPercorsoOttimo[(3 - giocatoreUtente) - 1] = 1000;
	ricercaPercorsoOttimo((3 - giocatoreUtente) - 1, x_NPC, y_NPC, scacchieraGiaEsplorata, v, 0);
	if(muriRimanenti[(3 - giocatoreUtente) - 1] == '0' || ((giocatoreUtente == 1 && lunghezzaPercorsoOttimo[(3 - giocatoreUtente) - 1] <= lunghezzaPercorsoOttimo[giocatoreUtente - 1]) || (giocatoreUtente == 2 && lunghezzaPercorsoOttimo[(3 - giocatoreUtente) - 1] < lunghezzaPercorsoOttimo[giocatoreUtente - 1])))
		muoviPedina();
	else
		provaAdInserireMuro();
	mossa = compilaCodificaMossa(1, giocatore, modalitaMuro, orientamentoMuro, posizioneMuro, nuovePosizioni);
	return mossa;
}

void inserisciMossaNellaScacchiera(uint32_t mossa){
	int g, mm, om, p[2], x, y;
	g = (mossa & 0xFF000000)>>24;
	mm = (mossa & 0x00F00000)>>20;
	om = (mossa & 0x000F0000)>>16;
	p[0] = (mossa & 0xFF);
	p[1] = (mossa & 0xFF00)>>8;
	if(!(mm == 0 && om == 1)){
		if(mm == 0){
			x = posizionePedine[g][0];
			y = posizionePedine[g][1];
			scacchiera[y * 2][x * 2] = 0;
			rimuoviPedina(x, y);
			posizionePedine[g][0] = p[0];
			posizionePedine[g][1] = p[1];
			scacchiera[p[1] * 2][p[0] * 2] = g + 1;
			inserisciPedina(p[0], p[1], coloriGiocatori[g]);
		}
		else{
			if(om == ORIZZONTALE){
				scacchiera[(p[1] * 2) + 1][p[0] * 2] = 4;
				scacchiera[(p[1] * 2) + 1][(p[0] * 2) + 1] = 7;
				scacchiera[(p[1] * 2) + 1][(p[0] * 2) + 2] = 4;
			}
			else{
				scacchiera[p[1] * 2][(p[0] * 2) + 1] = 4;
				scacchiera[(p[1] * 2) + 1][(p[0] * 2) + 1] = 6;
				scacchiera[(p[1] * 2) + 2][(p[0] * 2) + 1] = 4;
			}
			inserisciMuro(p[0], p[1], coloreMuro, om);
			muriRimanenti[g]--;
			if(g == 0)
				PutChar(40, 280, muriRimanenti[0], Black, White);
			else
				PutChar(194, 280, muriRimanenti[1], Black, White);
		}
	}
}

void inviaMossa(uint32_t mossa){
	CAN_TxMsg.data[0] = (mossa & 0xFF000000)>>24;
	CAN_TxMsg.data[1] = (mossa & 0x00FF0000)>>16;
	CAN_TxMsg.data[2] = (mossa & 0x0000FF00)>>8;
	CAN_TxMsg.data[3] = (mossa & 0x000000FF);
	CAN_TxMsg.len = 4;
	CAN_TxMsg.id = 1;
	CAN_TxMsg.format = STANDARD_FORMAT;
	CAN_TxMsg.type = DATA_FRAME;
	CAN_wrMsg(1, &CAN_TxMsg);
}

void giocatorePronto(void){
	disegnaScacchiera();
	configurazioneInizialePartita();
	CAN_TxMsg.data[0] = 0xFF;
	CAN_TxMsg.len = 4;
	CAN_TxMsg.id = 1;
	CAN_TxMsg.format = STANDARD_FORMAT;
	CAN_TxMsg.type = DATA_FRAME;
	CAN_wrMsg(1, &CAN_TxMsg);
}
