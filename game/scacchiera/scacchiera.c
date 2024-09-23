#include "scacchiera.h"

unsigned char pedina[] = "  ";
unsigned char your_turn[] = "It's your turn";
unsigned char opp_turn[] = "It's opponent's turn";

extern int coloriGiocatori[2];
extern int coloreMuro;
extern int coloreMuroProvvisorio;
extern int coloreSfondoTempo;
extern int giocatore;
extern int posizionePedine[2][2];
extern int posizioneMuro[2];
extern int orientamentoMuro;
extern int modalitaMuro;
extern int muroProvvisorio;
extern int partitaIniziata;
extern int ultimaMossa;
extern int scritto_m;
extern int freq_ko;
extern int freq_ok;
extern char muriRimanenti[2];
extern unsigned char tempo_rimanente[2];
extern uint32_t codificaMossa;
extern int mod;
extern int giocatoreUtente;
extern int lunghezzaPercorsoOttimo[2];
extern int mossePercorsoOttimo[2][DIM_SCACCHIERA * DIM_SCACCHIERA];
extern int partitaFinita;
extern int mioId;
extern int schermata;

int scacchiera[DIM_SCACCHIERA * 2][DIM_SCACCHIERA * 2]; // moltiplico per 2 perchè considero le linee dei muri
void ripristinaMuriPrecedenti(void);
void scoloraCelleAdiacenti(int up, int left, int right, int down);
void deselezionaCellaSelezionataPrecedentemente(int ultimaMossa);
int muroIntrappolaGiocatori(void);
uint8_t muroIntrappolaGiocatore(uint8_t g, uint8_t x, uint8_t y, uint8_t t, uint8_t s[DIM_SCACCHIERA][DIM_SCACCHIERA]);
extern unsigned char npc[];
extern unsigned char win_str[];

void disegnaScacchiera(void){
	int i = 8, j;
	int quadrato = 1;
	LCD_Clear(White);
	while(i < 232){
		LCD_DrawLine(i, 8, i, 231, Black);
		if(quadrato){
			i = i + 25;
			quadrato = 0;
		}
		else{
			i = i + 8;
			quadrato = 1;
		}
	}
	i = 8;
	quadrato = 1;
	while(i < 232){
		LCD_DrawLine(8, i, 231, i, Black);
		if(quadrato){
			i = i + 25;
			quadrato = 0;
		}
		else{
			i = i + 8;
			quadrato = 1;
		}
	}
	for(i = 34; i <= 199; i = i + 33)
		for(j = i; j < i + 7; j++)
			LCD_DrawLine(j, 8, j, 231, White);
	
	for(i = 34; i <= 199; i = i + 33)
		for(j = i; j < i + 7; j++)
			LCD_DrawLine(8, j, 231, j, White);
	
	LCD_DrawLine(9, 256, 9, 311, Black);
	LCD_DrawLine(77, 256, 77, 311, Black);
	LCD_DrawLine(9, 256, 77, 256, Black);
	LCD_DrawLine(9, 311, 77, 311, Black);
	
	LCD_DrawLine(86, 256, 86, 311, Black);
	LCD_DrawLine(154, 256, 154, 311, Black);
	LCD_DrawLine(86, 256, 154, 256, Black);
	LCD_DrawLine(86, 311, 154, 311, Black);
	
	LCD_DrawLine(163, 256, 163, 311, Black);
	LCD_DrawLine(231, 256, 231, 311, Black);
	LCD_DrawLine(163, 256, 231, 256, Black);
	LCD_DrawLine(163, 311, 231, 311, Black);
	
	schermata = SCACCHIERA;
}

void inserisciPedina(int x, int y, int colore){
	int i = 13 + x * 33;
	int j = 13 + y * 33;
	GUI_Text(i, j, pedina, colore, colore);
}

void rimuoviPedina(int x, int y){
	inserisciPedina(x, y, White);
}

void inserisciMuro(int x, int y, int colore, int orientamento){
	int i;
	if(orientamento == ORIZZONTALE){
		x = 8 + 33 * x;
		y = 34 + 33 * y;
		for(i = y; i < y + 7; i++){
			LCD_DrawLine(x, i, x + 58, i, colore);
		}
	}
	else{
		x = 34 + 33 * x;
		y = 8 + 33 * y;
		for(i = x; i < x + 7; i++){
			LCD_DrawLine(i, y, i, y + 58, colore);
		}
	}
}

void rimuoviMuro(int x, int y, int orientamento){
	inserisciMuro(x, y, White, orientamento);
}

void configurazioneInizialePartita(void){
	int i, j;
	unsigned char s1[] = "P1";
	unsigned char s2[] = "P2";
	unsigned char s4[] = "Turn of: ";
	unsigned char s5[] = " Walls";
	unsigned char s6[] = "You are: ";
	unsigned char s7[] = "Waiting for the opponent...";
	partitaIniziata = 1;
	giocatore = 0;
	modalitaMuro = 0;
	scritto_m = 1;
	posizionePedine[0][0] = 3;
	posizionePedine[0][1] = 0;
	posizionePedine[1][0] = 3;
	posizionePedine[1][1] = 6;
	inserisciPedina(posizionePedine[0][0], posizionePedine[0][1], coloriGiocatori[0]);
	inserisciPedina(posizionePedine[1][0], posizionePedine[1][1], coloriGiocatori[1]);
	GUI_Text(13, 260, s1, coloriGiocatori[0], White);
	GUI_Text(29, 260, s5, Black, White);
	PutChar(40, 280, muriRimanenti[0], Black, White);
	GUI_Text(167, 260, s2, coloriGiocatori[1], White);
	GUI_Text(183, 260, s5, Black, White);
	PutChar(194, 280, muriRimanenti[1], Black, White);
	PutChar(130, 275, 's', Black, White);
	if(mod == SINGLE_BOARD_V_HUMAN)
		GUI_Text(9, 238, s4, Black, White);
	else if(mod == SINGLE_BOARD_V_NPC){
		GUI_Text(9, 238, s6, Black, White);
		GUI_Text(81, 238, pedina, coloriGiocatori[giocatoreUtente - 1], coloriGiocatori[giocatoreUtente - 1]);
	}
	else if(mod == TWO_BOARDS_HUMAN || mod == TWO_BOARDS_NPC)
		GUI_Text(9, 238, s7, Black, Yellow);
	
	//riempio la scacchiera con tutti zeri e 1 e 2 per le posizioni delle 2 pedine
	for(i = 0; i < DIM_SCACCHIERA * 2; i++){
		for(j = 0; j < DIM_SCACCHIERA * 2; j++){
			if(i == 0 && j == 6)
				scacchiera[i][j] = 1;
			else if(i == DIM_SCACCHIERA * 2 - 1 && j == 6)
				scacchiera[i][j] = 2;
			else
				scacchiera[i][j] = 0;
		}
	}
}

void coloraCella(int x, int y, int colore){
	int i;
	x = 9 + 33 * x;
	y = 9 + 33 * y;
	for(i = y; i < y + 24; i++)
		LCD_DrawLine(x, i, x + 23, i, colore);
}

void coloraCelleAdiacenti(void){
	int x, y;
	x = posizionePedine[giocatore - 1][0];
	y = posizionePedine[giocatore - 1][1];
	if(y - 1 >= 0 && scacchiera[(y * 2) - 1][x * 2] != 4){ // up
		if(scacchiera[(y - 1) * 2][x * 2] == 0){
			scacchiera[(y - 1) * 2][x * 2] = 3;
			coloraCella(x, y - 1, Yellow);
		}
		else if(y - 2 >= 0 && scacchiera[((y - 1) * 2) - 1][x * 2] != 4){
			scacchiera[(y - 2) * 2][x * 2] = 3;
			coloraCella(x, y - 2, Yellow);
		}
		else if((y - 1 >= 0) && (x - 1 >= 0 || x + 1 < DIM_SCACCHIERA)){
			if(x - 1 >= 0 && scacchiera[((y - 1) * 2)][(x * 2) - 1] != 4){ // up-left
				scacchiera[(y - 1) * 2][(x - 1) * 2] = 3;
				coloraCella(x - 1, y - 1, Yellow);
			}
			if(x + 1 < DIM_SCACCHIERA && scacchiera[((y - 1) * 2)][(x * 2) + 1] != 4){ // up-right
				scacchiera[(y - 1) * 2][(x + 1) * 2] = 3;
				coloraCella(x + 1, y - 1, Yellow);
			}
		}
	}
	if(x - 1 >= 0 && scacchiera[y * 2][(x * 2) - 1] != 4){ // left
		if(scacchiera[y * 2][(x - 1) * 2] == 0){
			scacchiera[y * 2][(x - 1) * 2] = 3;
			coloraCella(x - 1, y, Yellow);
		}
		else if(x - 2 >= 0 && scacchiera[y * 2][((x - 1) * 2) - 1] != 4){
			scacchiera[y * 2][(x - 2) * 2] = 3;
			coloraCella(x - 2, y, Yellow);
		}
		else if((x - 1 >= 0) && (y - 1 >= 0 || y + 1 < DIM_SCACCHIERA)){
			if(y - 1 >= 0 && scacchiera[(y * 2) - 1][(x - 1) * 2] != 4){ // left-up
				scacchiera[(y - 1) * 2][(x - 1) * 2] = 3;
				coloraCella(x - 1, y - 1, Yellow);
			}
			if(y + 1 < DIM_SCACCHIERA && scacchiera[(y * 2) + 1][(x - 1) * 2] != 4){ // left-down
				scacchiera[(y + 1) * 2][(x - 1) * 2] = 3;
				coloraCella(x - 1, y + 1, Yellow);
			}
		}
	}
	if(x + 1 < DIM_SCACCHIERA && scacchiera[y * 2][(x * 2) + 1] != 4){ // right
		if(scacchiera[y * 2][(x + 1) * 2] == 0){
			scacchiera[y * 2][(x + 1) * 2] = 3;
			coloraCella(x + 1, y, Yellow);
		}
		else if(x + 2 < DIM_SCACCHIERA && scacchiera[y * 2][((x + 1) * 2) + 1] != 4){
			scacchiera[y * 2][(x + 2) * 2] = 3;
			coloraCella(x + 2, y, Yellow);
		}
		else if((x + 1 < DIM_SCACCHIERA) && (y - 1 >= 0 || y + 1 < DIM_SCACCHIERA)){
			if(y - 1 >= 0 && scacchiera[(y * 2) - 1][(x + 1) * 2] != 4){ // right-up
				scacchiera[(y - 1) * 2][(x + 1) * 2] = 3;
				coloraCella(x + 1, y - 1, Yellow);
			}
			if(y + 1 < DIM_SCACCHIERA && scacchiera[(y * 2) + 1][(x + 1) * 2] != 4){ // right-down
				scacchiera[(y + 1) * 2][(x + 1) * 2] = 3;
				coloraCella(x + 1, y + 1, Yellow);
			}
		}
	}
	if(y + 1 < DIM_SCACCHIERA && scacchiera[(y * 2) + 1][x * 2] != 4){ // down
		if(scacchiera[(y + 1) * 2][x * 2] == 0){
			scacchiera[(y + 1) * 2][x * 2] = 3;
			coloraCella(x, y + 1, Yellow);
		}
		else if(y + 2 < DIM_SCACCHIERA && scacchiera[((y + 1) * 2) + 1][x * 2] != 4){
			scacchiera[(y + 2) * 2][x * 2] = 3;
			coloraCella(x, y + 2, Yellow);
		}
		else if((y + 1 < DIM_SCACCHIERA) && (x - 1 >= 0 || x + 1 < DIM_SCACCHIERA)){
			if(x - 1 >= 0 && scacchiera[((y + 1) * 2)][(x * 2) - 1] != 4){ // down-left
				scacchiera[(y + 1) * 2][(x - 1) * 2] = 3;
				coloraCella(x - 1, y + 1, Yellow);
			}
			if(x + 1 < DIM_SCACCHIERA && scacchiera[((y + 1) * 2)][(x * 2) + 1] != 4){ // down-right
				scacchiera[(y + 1) * 2][(x + 1) * 2] = 3;
				coloraCella(x + 1, y + 1, Yellow);
			}
		}
	}
}

void scoloraCelleAdiacenti(int up, int left, int right, int down){
	int x, y;
 	x = posizionePedine[giocatore - 1][0];
	y = posizionePedine[giocatore - 1][1];
	if(up){
		if(y - 1 >= 0 && (scacchiera[(y - 1) * 2][x * 2] == 3 || scacchiera[(y - 1) * 2][x * 2] == 5)){ // up
			scacchiera[(y - 1) * 2][x * 2] = 0;
			coloraCella(x, y - 1, White);
		}
		else if(y - 2 >= 0 && (scacchiera[(y - 2) * 2][x * 2] == 3 || scacchiera[(y - 2) * 2][x * 2] == 5)){
			scacchiera[(y - 2) * 2][x * 2] = 0;
			coloraCella(x, y - 2, White);
		}
		else if((y - 1 >= 0) && (x - 1 >= 0 || x + 1 < DIM_SCACCHIERA)){
			if(x - 1 >= 0 && (scacchiera[(y - 1) * 2][(x - 1) * 2] == 3 || scacchiera[(y - 1) * 2][(x - 1) * 2] == 5)){ // up-left
				scacchiera[(y - 1) * 2][(x - 1) * 2] = 0;
				coloraCella(x - 1, y - 1, White);
			}
			if(x + 1 < DIM_SCACCHIERA && (scacchiera[(y - 1) * 2][(x + 1) * 2] == 3 || scacchiera[(y - 1) * 2][(x + 1) * 2] == 5)){ //up-right
				scacchiera[(y - 1) * 2][(x + 1) * 2] = 0;
				coloraCella(x + 1, y - 1, White);
			}
		}
	}
	if(left){
		if(x - 1 >= 0 && (scacchiera[y * 2][(x - 1) * 2] == 3 || scacchiera[y * 2][(x - 1) * 2] == 5)){ // left
			scacchiera[y * 2][(x - 1) * 2] = 0;
			coloraCella(x - 1, y, White);
		}
		else if(x - 2 >= 0 && (scacchiera[y * 2][(x - 2) * 2] == 3 || scacchiera[y * 2][(x - 2) * 2] == 5)){
			scacchiera[y * 2][(x - 2) * 2] = 0;
			coloraCella(x - 2, y, White);
		}
		else if((x - 1 >= 0) && (y - 1 >= 0 || y + 1 < DIM_SCACCHIERA)){
			if(y - 1 >= 0 && (scacchiera[(y - 1) * 2][(x - 1) * 2] == 3 || scacchiera[(y - 1) * 2][(x - 1) * 2] == 5)){ // left-up
				scacchiera[(y - 1) * 2][(x - 1) * 2] = 0;
				coloraCella(x - 1, y - 1, White);
			}
			if(y + 1 < DIM_SCACCHIERA && (scacchiera[(y + 1) * 2][(x - 1) * 2] == 3 || scacchiera[(y + 1) * 2][(x - 1) * 2] == 5)){ // left-down
				scacchiera[(y + 1) * 2][(x - 1) * 2] = 0;
				coloraCella(x - 1, y + 1, White);
			}
		}
	}
	if(right){
		if(x + 1 < DIM_SCACCHIERA && (scacchiera[y * 2][(x + 1) * 2] == 3 || scacchiera[y * 2][(x + 1) * 2] == 5)){ // right
			scacchiera[y * 2][(x + 1) * 2] = 0;
			coloraCella(x + 1, y, White);
		}
		else if(x + 2 < DIM_SCACCHIERA && (scacchiera[y * 2][(x + 2) * 2] == 3 || scacchiera[y * 2][(x + 2) * 2] == 5)){
			scacchiera[y * 2][(x + 2) * 2] = 0;
			coloraCella(x + 2, y, White);
		}
		else if((x + 1 < DIM_SCACCHIERA) && (y - 1 >= 0 || y + 1 < DIM_SCACCHIERA)){
			if(y - 1 >= 0 && (scacchiera[(y - 1) * 2][(x + 1) * 2] == 3 || scacchiera[(y - 1) * 2][(x + 1) * 2] == 5)){ // right-up
				scacchiera[(y - 1) * 2][(x + 1) * 2] = 0;
				coloraCella(x + 1, y - 1, White);
			}
			if(y + 1 < DIM_SCACCHIERA && (scacchiera[(y + 1) * 2][(x + 1) * 2] == 3 || scacchiera[(y + 1) * 2][(x + 1) * 2] == 5)){ // right-down
				scacchiera[(y + 1) * 2][(x + 1) * 2] = 0;
				coloraCella(x + 1, y + 1, White);
			}
		}
	}
	if(down){
		if(y + 1 < DIM_SCACCHIERA && (scacchiera[(y + 1) * 2][x * 2] == 3 || scacchiera[(y + 1) * 2][x * 2] == 5)){ // down
			scacchiera[(y + 1) * 2][x * 2] = 0;
			coloraCella(x, y + 1, White);
		}
		else if(y + 2 < DIM_SCACCHIERA && (scacchiera[(y + 2) * 2][x * 2] == 3 || scacchiera[(y + 2) * 2][x * 2] == 5)){
			scacchiera[(y + 2) * 2][x * 2] = 0;
			coloraCella(x, y + 2, White);
		}
		else if((y + 1 < DIM_SCACCHIERA) && (x - 1 >= 0 || x + 1 < DIM_SCACCHIERA)){
			if(x - 1 >= 0 && (scacchiera[(y + 1) * 2][(x - 1) * 2] == 3 || scacchiera[(y + 1) * 2][(x - 1) * 2] == 5)){ // down-left
				scacchiera[(y + 1) * 2][(x - 1) * 2] = 0;
				coloraCella(x - 1, y + 1, White);
			}
			if(x + 1 < DIM_SCACCHIERA && (scacchiera[(y + 1) * 2][(x + 1) * 2] == 3 || scacchiera[(y + 1) * 2][(x + 1) * 2] == 5)){ // down-right
				scacchiera[(y + 1) * 2][(x + 1) * 2] = 0;
				coloraCella(x + 1, y + 1, White);
			}
		}
	}
}

void turno(void){
	int i;
	uint32_t mossa;
	unsigned char s6[] = "You are: ";
	switch(mod){
		case SINGLE_BOARD_V_HUMAN:
			if(giocatore != 0){
				if(muroProvvisorio){
					rimuoviMuro(posizioneMuro[0], posizioneMuro[1], orientamentoMuro);
					ripristinaMuriPrecedenti();
					muroProvvisorio = 0;
					disable_timer(2);
				}
				else
					scoloraCelleAdiacenti(1, 1, 1, 1);
				if(!scritto_m){
					if(giocatore == 1)
						PutChar(40, 280, muriRimanenti[0], Black, White);
					else
						PutChar(194, 280, muriRimanenti[1], Black, White);
					scritto_m = 1;
				}
			}
			modalitaMuro = 0;
			tempo_rimanente[0] = '2';
			tempo_rimanente[1] = '0';
			giocatore++;
			if(giocatore == 3)
				giocatore = 1;
			GUI_Text(81, 238, pedina, coloriGiocatori[giocatore - 1], coloriGiocatori[giocatore - 1]);
			coloraCelleAdiacenti();
			coloreSfondoTempo = White;
			GUI_Text(110, 275, tempo_rimanente, Black, coloreSfondoTempo);
			ultimaMossa = -1;
			enable_timer(0);
			break;
		case SINGLE_BOARD_V_NPC:
			if(giocatore == giocatoreUtente && giocatore != 0){
				if(muroProvvisorio){
					rimuoviMuro(posizioneMuro[0], posizioneMuro[1], orientamentoMuro);
					ripristinaMuriPrecedenti();
					muroProvvisorio = 0;
					disable_timer(2);
				}
				else
					scoloraCelleAdiacenti(1, 1, 1, 1);
				if(!scritto_m){
					if(giocatore == 1)
						PutChar(40, 280, muriRimanenti[0], Black, White);
					else
						PutChar(194, 280, muriRimanenti[1], Black, White);
					scritto_m = 1;
				}
			}
			modalitaMuro = 0;
			tempo_rimanente[0] = '2';
			tempo_rimanente[1] = '0';
			giocatore++;
			if(giocatore == 3)
				giocatore = 1;
			if(giocatore == giocatoreUtente){
				coloraCelleAdiacenti();
				coloreSfondoTempo = White;
				GUI_Text(110, 275, tempo_rimanente, Black, coloreSfondoTempo);
				ultimaMossa = -1;
				enable_timer(0);
			}
			else{
				GUI_Text(110, 275, tempo_rimanente, White, White);
				mossa = generaMossaNPC();
				inserisciMossaNellaScacchiera(mossa);
				if(verificaVittoria()){
					GUI_Text(9, 238, npc, coloriGiocatori[giocatore - 1], Yellow);
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
			break;
		case TWO_BOARDS_HUMAN:
			if(giocatore != 0 && giocatore == mioId){
				if(muroProvvisorio){
					rimuoviMuro(posizioneMuro[0], posizioneMuro[1], orientamentoMuro);
					ripristinaMuriPrecedenti();
					muroProvvisorio = 0;
					disable_timer(2);
				}
				else
					scoloraCelleAdiacenti(1, 1, 1, 1);
				if(!scritto_m){
					if(giocatore == 1)
						PutChar(40, 280, muriRimanenti[0], Black, White);
					else
						PutChar(194, 280, muriRimanenti[1], Black, White);
					scritto_m = 1;
				}
			}
			modalitaMuro = 0;
			tempo_rimanente[0] = '2';
			tempo_rimanente[1] = '0';
			giocatore++;
			if(giocatore == 3)
				giocatore = 1;
			if(giocatore == mioId){
				for(i = 235; i < 255; i++){
					LCD_DrawLine(0, i, 240, i, White);
				}
				GUI_Text(9, 238, your_turn, Black, White);
				GUI_Text(125, 238, pedina, coloriGiocatori[giocatore - 1], coloriGiocatori[giocatore - 1]);
				coloraCelleAdiacenti();
				coloreSfondoTempo = White;
				GUI_Text(110, 275, tempo_rimanente, Black, coloreSfondoTempo);
				ultimaMossa = -1;
				enable_timer(0);
			}
			else{
				for(i = 235; i < 255; i++){
					LCD_DrawLine(0, i, 240, i, White);
				}
				GUI_Text(9, 238, opp_turn, Black, White);
				GUI_Text(181, 238, pedina, coloriGiocatori[giocatore - 1], coloriGiocatori[giocatore - 1]);
				GUI_Text(110, 275, tempo_rimanente, White, White);
			}
			break;
		case TWO_BOARDS_NPC:
			if(giocatore == 0){
				for(i = 235; i < 255; i++){
					LCD_DrawLine(0, i, 240, i, White);
				}
				GUI_Text(9, 238, s6, Black, White);
				GUI_Text(81, 238, pedina, coloriGiocatori[mioId - 1], coloriGiocatori[mioId - 1]);
				giocatoreUtente = 3 - mioId;
			}
			modalitaMuro = 0;
			tempo_rimanente[0] = '2';
			tempo_rimanente[1] = '0';
			giocatore++;
			if(giocatore == 3)
				giocatore = 1;
			if(giocatore == mioId){
				reset_timer(2);
				enable_timer(2);
			}
			break;
	}
}

void passaModalitaMuro(void){
	scoloraCelleAdiacenti(1, 1, 1, 1);
	posizioneMuro[0] = 3;
	posizioneMuro[1] = 2;
	orientamentoMuro = ORIZZONTALE;
	inserisciMuro(posizioneMuro[0], posizioneMuro[1], coloreMuroProvvisorio, ORIZZONTALE);
	modalitaMuro = 1;
	muroProvvisorio = 1;
	enable_timer(2);
}

void esciModalitaMuro(void){
	disable_timer(2);
	if(!scritto_m){
		if(giocatore == 1)
			PutChar(40, 280, muriRimanenti[0], Black, White);
		else
			PutChar(194, 280, muriRimanenti[1], Black, White);
		scritto_m = 1;
	}
	rimuoviMuro(posizioneMuro[0], posizioneMuro[1], orientamentoMuro);
	ripristinaMuriPrecedenti();
	modalitaMuro = 0;
	muroProvvisorio = 0;
	coloraCelleAdiacenti();
	ultimaMossa = -1;
}

void ruotaMuro(void){
	if(!orientamentoMuro){
		rimuoviMuro(posizioneMuro[0], posizioneMuro[1], orientamentoMuro);
		ripristinaMuriPrecedenti();
		orientamentoMuro = 1;
		inserisciMuro(posizioneMuro[0], posizioneMuro[1], coloreMuroProvvisorio, orientamentoMuro);
	}
	else{
		rimuoviMuro(posizioneMuro[0], posizioneMuro[1], orientamentoMuro);
		ripristinaMuriPrecedenti();
		orientamentoMuro = 0;
		inserisciMuro(posizioneMuro[0], posizioneMuro[1], coloreMuroProvvisorio, orientamentoMuro);
	}
}

void deselezionaCellaSelezionataPrecedentemente(int ultimaMossa){
	int x, y;
	x = posizionePedine[giocatore - 1][0];
	y = posizionePedine[giocatore - 1][1];
	switch(ultimaMossa){
		case DOWN:
			if(scacchiera[(y + 1) * 2][x * 2] == 5){
				scacchiera[(y + 1) * 2][x * 2] = 3;
				coloraCella(x, y + 1, Yellow);
			}
			else{
				scacchiera[(y + 2) * 2][x * 2] = 3;
				coloraCella(x, y + 2, Yellow);
			}
			break;
		case LEFT:
			if(scacchiera[y * 2][(x - 1) * 2] == 5){
				scacchiera[y * 2][(x - 1) * 2] = 3;
				coloraCella(x - 1, y, Yellow);
			}
			else{
				scacchiera[y * 2][(x - 2) * 2] = 3;
				coloraCella(x - 2, y, Yellow);
			}
			break;
		case RIGHT:
			if(scacchiera[y * 2][(x + 1) * 2] == 5){
				scacchiera[y * 2][(x + 1) * 2] = 3;
				coloraCella(x + 1, y, Yellow);
			}
			else{
				scacchiera[y * 2][(x + 2) * 2] = 3;
				coloraCella(x + 2, y, Yellow);
			}
			break;
		case UP:
			if(scacchiera[(y - 1) * 2][x * 2] == 5){
				scacchiera[(y - 1) * 2][x * 2] = 3;
				coloraCella(x, y - 1, Yellow);
			}
			else{
				scacchiera[(y - 2) * 2][x * 2] = 3;
				coloraCella(x, y - 2, Yellow);
			}
			break;
		case UP_LEFT:
			if(scacchiera[(y - 1) * 2][(x - 1) * 2] == 5){
				scacchiera[(y - 1) * 2][(x - 1) * 2] = 3;
				coloraCella(x - 1, y - 1, Yellow);
			}
			break;
		case UP_RIGHT:
			if(scacchiera[(y - 1) * 2][(x + 1) * 2] == 5){
				scacchiera[(y - 1) * 2][(x + 1) * 2] = 3;
				coloraCella(x + 1, y - 1, Yellow);
			}
			break;
		case DOWN_LEFT:
			if(scacchiera[(y + 1) * 2][(x - 1) * 2] == 5){
				scacchiera[(y + 1) * 2][(x - 1) * 2] = 3;
				coloraCella(x - 1, y + 1, Yellow);
			}
			break;
		case DOWN_RIGHT:
			if(scacchiera[(y + 1) * 2][(x + 1) * 2] == 5){
				scacchiera[(y + 1) * 2][(x + 1) * 2] = 3;
				coloraCella(x + 1, y + 1, Yellow);
			}
			break;
		default:
			break;
	}
}

void controllaMossa(int direzione){
	int x, y;
	switch(direzione){
		case DOWN:
			if(!modalitaMuro){
				x = posizionePedine[giocatore - 1][0];
				y = posizionePedine[giocatore - 1][1];
				if(y + 1 < DIM_SCACCHIERA && scacchiera[(y + 1) * 2][x * 2] == 3){
					deselezionaCellaSelezionataPrecedentemente(ultimaMossa);
					scacchiera[(y + 1) * 2][x * 2] = 5;
					coloraCella(x, y + 1, Green);
					ultimaMossa = DOWN;
				}
				else if(y + 2 < DIM_SCACCHIERA && scacchiera[(y + 2) * 2][x * 2] == 3){
					deselezionaCellaSelezionataPrecedentemente(ultimaMossa);
					scacchiera[(y + 2) * 2][x * 2] = 5;
					coloraCella(x, y + 2, Green);
					ultimaMossa = DOWN;
				}
			}
			else{
				x = posizioneMuro[0];
				y = posizioneMuro[1];
				if(y + 1 < DIM_SCACCHIERA - 1){
					rimuoviMuro(x, y, orientamentoMuro);
					ripristinaMuriPrecedenti();
					inserisciMuro(x, y + 1, coloreMuroProvvisorio, orientamentoMuro);
					posizioneMuro[1] = y + 1;
				}
			}
			break;
		case LEFT:
			if(!modalitaMuro){
				x = posizionePedine[giocatore - 1][0];
				y = posizionePedine[giocatore - 1][1];
				if(x - 1 >= 0 && scacchiera[y * 2][(x - 1) * 2] == 3){
					deselezionaCellaSelezionataPrecedentemente(ultimaMossa);
					scacchiera[y * 2][(x - 1) * 2] = 5;
					coloraCella(x - 1, y, Green);
					ultimaMossa = LEFT;
				}
				else if(x - 2 >= 0 && scacchiera[y * 2][(x - 2) * 2] == 3){
					deselezionaCellaSelezionataPrecedentemente(ultimaMossa);
					scacchiera[y * 2][(x - 2) * 2] = 5;
					coloraCella(x - 2, y, Green);
					ultimaMossa = LEFT;
				}
			}
			else{
				x = posizioneMuro[0];
				y = posizioneMuro[1];
				if(x - 1 >= 0){
					rimuoviMuro(x, y, orientamentoMuro);
					ripristinaMuriPrecedenti();
					inserisciMuro(x - 1, y, coloreMuroProvvisorio, orientamentoMuro);
					posizioneMuro[0] = x - 1;
				}
			}
			break;
		case RIGHT:
			if(!modalitaMuro){
				x = posizionePedine[giocatore - 1][0];
				y = posizionePedine[giocatore - 1][1];
				if(x + 1 < DIM_SCACCHIERA && scacchiera[y * 2][(x + 1) * 2] == 3){
					deselezionaCellaSelezionataPrecedentemente(ultimaMossa);
					scacchiera[y * 2][(x + 1) * 2] = 5;
					coloraCella(x + 1, y, Green);
					ultimaMossa = RIGHT;
				}
				else if(x + 2 < DIM_SCACCHIERA && scacchiera[y * 2][(x + 2) * 2] == 3){
					deselezionaCellaSelezionataPrecedentemente(ultimaMossa);
					scacchiera[y * 2][(x + 2) * 2] = 5;
					coloraCella(x + 2, y, Green);
					ultimaMossa = RIGHT;
				}
			}
			else{
				x = posizioneMuro[0];
				y = posizioneMuro[1];
				if(x + 1 < DIM_SCACCHIERA - 1){
					rimuoviMuro(x, y, orientamentoMuro);
					ripristinaMuriPrecedenti();
					inserisciMuro(x + 1, y, coloreMuroProvvisorio, orientamentoMuro);
					posizioneMuro[0] = x + 1;
				}
			}
			break;
		case UP:
			if(!modalitaMuro){
				x = posizionePedine[giocatore - 1][0];
				y = posizionePedine[giocatore - 1][1];
				if(y - 1 >= 0 && scacchiera[(y - 1) * 2][x * 2] == 3){
					deselezionaCellaSelezionataPrecedentemente(ultimaMossa);
					scacchiera[(y - 1) * 2][x * 2] = 5;
					coloraCella(x, y - 1, Green);
					ultimaMossa = UP;
				}
				else if(y - 2 >= 0 && scacchiera[(y - 2) * 2][x * 2] == 3){
					deselezionaCellaSelezionataPrecedentemente(ultimaMossa);
					scacchiera[(y - 2) * 2][x * 2] = 5;
					coloraCella(x, y - 2, Green);
					ultimaMossa = UP;
				}
			}
			else{
				x = posizioneMuro[0];
				y = posizioneMuro[1];
				if(y - 1 >= 0){
					rimuoviMuro(x, y, orientamentoMuro);
					ripristinaMuriPrecedenti();
					inserisciMuro(x, y - 1, coloreMuroProvvisorio, orientamentoMuro);
					posizioneMuro[1] = y - 1;
				}
			}	
			break;
		case UP_LEFT:
			if(!modalitaMuro){
				x = posizionePedine[giocatore - 1][0];
				y = posizionePedine[giocatore - 1][1];
				if(y - 1 >= 0 && x - 1 >= 0 && scacchiera[(y - 1) * 2][(x - 1) * 2] == 3){
					deselezionaCellaSelezionataPrecedentemente(ultimaMossa);
					scacchiera[(y - 1) * 2][(x - 1) * 2] = 5;
					coloraCella(x - 1, y - 1, Green);
					ultimaMossa = UP_LEFT;
				}
			}
			break;
		case UP_RIGHT:
			if(!modalitaMuro){
				x = posizionePedine[giocatore - 1][0];
				y = posizionePedine[giocatore - 1][1];
				if(y - 1 >= 0 && x + 1 < DIM_SCACCHIERA && scacchiera[(y - 1) * 2][(x + 1) * 2] == 3){
					deselezionaCellaSelezionataPrecedentemente(ultimaMossa);
					scacchiera[(y - 1) * 2][(x + 1) * 2] = 5;
					coloraCella(x + 1, y - 1, Green);
					ultimaMossa = UP_RIGHT;
				}
			}
			break;
		case DOWN_LEFT:
			if(!modalitaMuro){
				x = posizionePedine[giocatore - 1][0];
				y = posizionePedine[giocatore - 1][1];
				if(y + 1 < DIM_SCACCHIERA && x - 1 >= 0 && scacchiera[(y + 1) * 2][(x - 1) * 2] == 3){
					deselezionaCellaSelezionataPrecedentemente(ultimaMossa);
					scacchiera[(y + 1) * 2][(x - 1) * 2] = 5;
					coloraCella(x - 1, y + 1, Green);
					ultimaMossa = DOWN_LEFT;
				}
			}
			break;
		case DOWN_RIGHT:
			if(!modalitaMuro){
				x = posizionePedine[giocatore - 1][0];
				y = posizionePedine[giocatore - 1][1];
				if(y + 1 < DIM_SCACCHIERA && x + 1 < DIM_SCACCHIERA && scacchiera[(y + 1) * 2][(x + 1) * 2] == 3){
					deselezionaCellaSelezionataPrecedentemente(ultimaMossa);
					scacchiera[(y + 1) * 2][(x + 1) * 2] = 5;
					coloraCella(x + 1, y + 1, Green);
					ultimaMossa = DOWN_RIGHT;
				}
			}
			break;
	}
}

uint32_t compilaCodificaMossa(int valida, int g, int mm, int om, int pm[2], int pp[2]){
	uint32_t mossa = 0;
	mossa |= (g - 1)<<24;
  if(valida){
		mossa |= mm<<20;
		if(mm){
			mossa |= om<<16;
			mossa |= pm[1]<<8;
			mossa |= pm[0];
		}
		else{
			mossa &= 0xFFF0FFFF;
			mossa |= pp[1]<<8;
			mossa |= pp[0];
		}
	}
	else{
		mossa |= 0x00010000;
		mossa &= 0xFF010000; 
	}
	return mossa;
}

int confermaMossa(void){
	int x, y;
	unsigned char s[] = "NO MOVE SELECTED!";
	unsigned char s1[] = "WALL POSITION NOT VALID!";
	if(!modalitaMuro){
		x = posizionePedine[giocatore - 1][0];
		y = posizionePedine[giocatore - 1][1];
		switch(ultimaMossa){
			case DOWN:
				if(scacchiera[(y + 1) * 2][x * 2] == 5){
					scoloraCelleAdiacenti(1, 1, 1, 1);
					rimuoviPedina(x, y);
					scacchiera[y * 2][x * 2] = 0;
					inserisciPedina(x, y + 1, coloriGiocatori[giocatore - 1]);
					scacchiera[(y + 1) * 2][x * 2] = giocatore;
					posizionePedine[giocatore - 1][1] = y + 1;
				}
				else{
					scoloraCelleAdiacenti(1, 1, 1, 1);
					rimuoviPedina(x, y);
					scacchiera[y * 2][x * 2] = 0;
					inserisciPedina(x, y + 2, coloriGiocatori[giocatore - 1]);
					scacchiera[(y + 2) * 2][x * 2] = giocatore;
					posizionePedine[giocatore - 1][1] = y + 2;
				}
				break;
			case LEFT:
				if(scacchiera[y * 2][(x - 1) * 2] == 5){
					scoloraCelleAdiacenti(1, 1, 1, 1);
					rimuoviPedina(x, y);
					scacchiera[y * 2][x * 2] = 0;
					inserisciPedina(x - 1, y, coloriGiocatori[giocatore - 1]);
					scacchiera[y * 2][(x - 1) * 2] = giocatore;
					posizionePedine[giocatore - 1][0] = x - 1;
				}
				else{
					scoloraCelleAdiacenti(1, 1, 1, 1);
					rimuoviPedina(x, y);
					scacchiera[y * 2][x * 2] = 0;
					inserisciPedina(x - 2, y, coloriGiocatori[giocatore - 1]);
					scacchiera[y * 2][(x - 2) * 2] = giocatore;
					posizionePedine[giocatore - 1][0] = x - 2;
				}
				break;
			case RIGHT:
				if(scacchiera[y * 2][(x + 1) * 2] == 5){
					scoloraCelleAdiacenti(1, 1, 1, 1);
					rimuoviPedina(x, y);
					scacchiera[y * 2][x * 2] = 0;
					inserisciPedina(x + 1, y, coloriGiocatori[giocatore - 1]);
					scacchiera[y * 2][(x + 1) * 2] = giocatore;
					posizionePedine[giocatore - 1][0] = x + 1;
				}
				else{
					scoloraCelleAdiacenti(1, 1, 1, 1);
					rimuoviPedina(x, y);
					scacchiera[y * 2][x * 2] = 0;
					inserisciPedina(x + 2, y, coloriGiocatori[giocatore - 1]);
					scacchiera[y * 2][(x + 2) * 2] = giocatore;
					posizionePedine[giocatore - 1][0] = x + 2;
				}
				break;
			case UP:
				if(scacchiera[(y - 1) * 2][x * 2] == 5){
					scoloraCelleAdiacenti(1, 1, 1, 1);
					rimuoviPedina(x, y);
					scacchiera[y * 2][x * 2] = 0;
					inserisciPedina(x, y - 1, coloriGiocatori[giocatore - 1]);
					scacchiera[(y - 1) * 2][x * 2] = giocatore;
					posizionePedine[giocatore - 1][1] = y - 1;
				}
				else{
					scoloraCelleAdiacenti(1, 1, 1, 1);
					rimuoviPedina(x, y);
					scacchiera[y * 2][x * 2] = 0;
					inserisciPedina(x, y - 2, coloriGiocatori[giocatore - 1]);
					scacchiera[(y - 2) * 2][x * 2] = giocatore;
					posizionePedine[giocatore - 1][1] = y - 2;
				}
				break;
			case UP_LEFT:
				if(scacchiera[(y - 1) * 2][(x - 1) * 2] == 5){
					scoloraCelleAdiacenti(1, 1, 1, 1);
					rimuoviPedina(x, y);
					scacchiera[y * 2][x * 2] = 0;
					inserisciPedina(x - 1, y - 1, coloriGiocatori[giocatore - 1]);
					scacchiera[(y - 1) * 2][(x - 1) * 2] = giocatore;
					posizionePedine[giocatore - 1][0] = x - 1;
					posizionePedine[giocatore - 1][1] = y - 1;
				}
				break;
			case UP_RIGHT:
				if(scacchiera[(y - 1) * 2][(x + 1) * 2] == 5){
					scoloraCelleAdiacenti(1, 1, 1, 1);
					rimuoviPedina(x, y);
					scacchiera[y * 2][x * 2] = 0;
					inserisciPedina(x + 1, y - 1, coloriGiocatori[giocatore - 1]);
					scacchiera[(y - 1) * 2][(x + 1) * 2] = giocatore;
					posizionePedine[giocatore - 1][0] = x + 1;
					posizionePedine[giocatore - 1][1] = y - 1;
				}
				break;
			case DOWN_LEFT:
				if(scacchiera[(y + 1) * 2][(x - 1) * 2] == 5){
					scoloraCelleAdiacenti(1, 1, 1, 1);
					rimuoviPedina(x, y);
					scacchiera[y * 2][x * 2] = 0;
					inserisciPedina(x - 1, y + 1, coloriGiocatori[giocatore - 1]);
					scacchiera[(y + 1) * 2][(x - 1) * 2] = giocatore;
					posizionePedine[giocatore - 1][0] = x - 1;
					posizionePedine[giocatore - 1][1] = y + 1;
				}
				break;
			case DOWN_RIGHT:
				if(scacchiera[(y + 1) * 2][(x + 1) * 2] == 5){
					scoloraCelleAdiacenti(1, 1, 1, 1);
					rimuoviPedina(x, y);
					scacchiera[y * 2][x * 2] = 0;
					inserisciPedina(x + 1, y + 1, coloriGiocatori[giocatore - 1]);
					scacchiera[(y + 1) * 2][(x + 1) * 2] = giocatore;
					posizionePedine[giocatore - 1][0] = x + 1;
					posizionePedine[giocatore - 1][1] = y + 1;
				}
				break;
			default:
				GUI_Text(9, 238, s, Black, Yellow);
				enable_timer(3);
				enable_timer(1);
				return 0;
		}
	}
	else{
		x = posizioneMuro[0];
		y = posizioneMuro[1];
		if(orientamentoMuro == ORIZZONTALE){
			if(scacchiera[(y * 2) + 1][x * 2] != 4 && scacchiera[(y * 2) + 1][(x * 2) + 1] == 0 && scacchiera[(y * 2) + 1][(x * 2) + 2] != 4 && !muroIntrappolaGiocatori()){
				inserisciMuro(x, y, coloreMuro, orientamentoMuro);
				scacchiera[(y * 2) + 1][x * 2] = 4;
				scacchiera[(y * 2) + 1][(x * 2) + 1] = 7;
				scacchiera[(y * 2) + 1][(x * 2) + 2] = 4;
				disable_timer(2);
				muriRimanenti[giocatore - 1]--;
				muroProvvisorio = 0;
				if(giocatore == 1)
					PutChar(40, 280, muriRimanenti[0], Black, White);
				else
					PutChar(194, 280, muriRimanenti[1], Black, White);
			}
			else{
				GUI_Text(9, 238, s1, Black, Yellow);
				enable_timer(3);
				enable_timer(1);
				return 0;
			}
		}
		else{
			if(scacchiera[y * 2][(x * 2) + 1] != 4 && scacchiera[(y * 2) + 1][(x * 2) + 1] == 0 && scacchiera[(y * 2) + 2][(x * 2) + 1] != 4 && !muroIntrappolaGiocatori()){
				inserisciMuro(x, y, coloreMuro, orientamentoMuro);
				scacchiera[y * 2][(x * 2) + 1] = 4;
				scacchiera[(y * 2) + 1][(x * 2) + 1] = 6;
				scacchiera[(y * 2) + 2][(x * 2) + 1] = 4;
				disable_timer(2);
				muriRimanenti[giocatore - 1]--;
				muroProvvisorio = 0;
				if(giocatore == 1)
					PutChar(40, 280, muriRimanenti[0], Black, White);
				else
					PutChar(194, 280, muriRimanenti[1], Black, White);
			}
			else{
				GUI_Text(9, 238, s1, Black, Yellow);
				enable_timer(3);
				enable_timer(1);
				return 0;
			}
		}
	}
	return 1;
}

void ripristinaMuriPrecedenti(void){
	int x = posizioneMuro[0];
	int y = posizioneMuro[1];
	if(scacchiera[(y * 2) + 1][(x * 2) + 1] == 7)
			inserisciMuro(x, y, coloreMuro, ORIZZONTALE);
	else if(scacchiera[(y * 2) + 1][(x * 2) + 1] == 6)
		inserisciMuro(x, y, coloreMuro, VERTICALE);
	if(orientamentoMuro == ORIZZONTALE){
		if(scacchiera[(y * 2) + 1][((x - 1) * 2) + 1] == 7)
			inserisciMuro(x - 1, y, coloreMuro, ORIZZONTALE);
		if(scacchiera[(y * 2) + 1][((x + 1) * 2) + 1] == 7)
			inserisciMuro(x + 1, y, coloreMuro, ORIZZONTALE);
	}
	else{
		if(scacchiera[((y - 1) * 2) + 1][(x * 2) + 1] == 6)
			inserisciMuro(x, y - 1, coloreMuro, VERTICALE);
		if(scacchiera[((y + 1) * 2) + 1][(x * 2) + 1] == 6)
			inserisciMuro(x, y + 1, coloreMuro, VERTICALE);
	}
}

uint8_t muroIntrappolaGiocatore(uint8_t g, uint8_t x, uint8_t y, uint8_t t, uint8_t s[DIM_SCACCHIERA][DIM_SCACCHIERA]){
	if(g == 0 && y == DIM_SCACCHIERA - 1) // condizione finale
		return 0;
	else if(g == 1 && y == 0)
		return 0;
	s[y][x] = 1;
	// chiamate ricorsive
	if(g == 0){
		if(y + 1 < DIM_SCACCHIERA && scacchiera[(y * 2) + 1][x * 2] != 4 && t){ // down
			if(!s[y + 1][x]){
				t = muroIntrappolaGiocatore(g, x, y + 1, t, s);
			}
		}
		if(x - 1 >= 0 && scacchiera[y * 2][(x * 2) - 1] != 4 && t){ // left
			if(!s[y][x - 1]){
				t = muroIntrappolaGiocatore(g, x - 1, y, t, s);
			}
		}
		if(x + 1 < DIM_SCACCHIERA && scacchiera[y * 2][(x * 2) + 1] != 4 && t){ // right
			if(!s[y][x + 1]){
				t = muroIntrappolaGiocatore(g, x + 1, y, t, s);
			}
		}
		if(y - 1 >= 0 && scacchiera[(y * 2) - 1][x * 2] != 4 && t){ // up
			if(!s[y - 1][x]){
				t = muroIntrappolaGiocatore(g, x, y - 1, t, s);
			}
		}
	}
	else{
		if(y - 1 >= 0 && scacchiera[(y * 2) - 1][x * 2] != 4 && t){ // up
			if(!s[y - 1][x]){
				t = muroIntrappolaGiocatore(g, x, y - 1, t, s);
			}
		}
		if(x - 1 >= 0 && scacchiera[y * 2][(x * 2) - 1] != 4 && t){ // left
			if(!s[y][x - 1]){
				t = muroIntrappolaGiocatore(g, x - 1, y, t, s);
			}
		}
		if(x + 1 < DIM_SCACCHIERA && scacchiera[y * 2][(x * 2) + 1] != 4 && t){ // right
			if(!s[y][x + 1]){
				t = muroIntrappolaGiocatore(g, x + 1, y, t, s);
			}
		}
		if(y + 1 < DIM_SCACCHIERA && scacchiera[(y * 2) + 1][x * 2] != 4 && t){ // down
			if(!s[y + 1][x]){
				t = muroIntrappolaGiocatore(g, x, y + 1, t, s);
			}
		}
	}
	return t;
}

int muroIntrappolaGiocatori(void){
	uint8_t x, y, i, j, scacchieraGiaEsplorata[DIM_SCACCHIERA][DIM_SCACCHIERA];
	x = posizioneMuro[0];
	y = posizioneMuro[1];
	for(i = 0; i < DIM_SCACCHIERA; i++)
		for(j = 0; j < DIM_SCACCHIERA; j++)
			scacchieraGiaEsplorata[i][j] = 0;
	if(orientamentoMuro == ORIZZONTALE){
		scacchiera[(y * 2) + 1][x * 2] = 4;
		scacchiera[(y * 2) + 1][(x * 2) + 1] = 7;
		scacchiera[(y * 2) + 1][(x * 2) + 2] = 4;
	}
	else{
		scacchiera[y * 2][(x * 2) + 1] = 4;
		scacchiera[(y * 2) + 1][(x * 2) + 1] = 6;
		scacchiera[(y * 2) + 2][(x * 2) + 1] = 4;
	}
	if(muroIntrappolaGiocatore(0, posizionePedine[0][0], posizionePedine[0][1], 1, scacchieraGiaEsplorata)){
		if(orientamentoMuro == ORIZZONTALE){
			scacchiera[(y * 2) + 1][x * 2] = 0;
			scacchiera[(y * 2) + 1][(x * 2) + 1] = 0;
			scacchiera[(y * 2) + 1][(x * 2) + 2] = 0;
		}
		else{
			scacchiera[y * 2][(x * 2) + 1] = 0;
			scacchiera[(y * 2) + 1][(x * 2) + 1] = 0;
			scacchiera[(y * 2) + 2][(x * 2) + 1] = 0;
		}
		return 1;
	}
	for(i = 0; i < DIM_SCACCHIERA; i++)
		for(j = 0; j < DIM_SCACCHIERA; j++)
			scacchieraGiaEsplorata[i][j] = 0;
	if(muroIntrappolaGiocatore(1, posizionePedine[1][0], posizionePedine[1][1], 1, scacchieraGiaEsplorata)){
		if(orientamentoMuro == ORIZZONTALE){
			scacchiera[(y * 2) + 1][x * 2] = 0;
			scacchiera[(y * 2) + 1][(x * 2) + 1] = 0;
			scacchiera[(y * 2) + 1][(x * 2) + 2] = 0;
		}
		else{
			scacchiera[y * 2][(x * 2) + 1] = 0;
			scacchiera[(y * 2) + 1][(x * 2) + 1] = 0;
			scacchiera[(y * 2) + 2][(x * 2) + 1] = 0;
		}
		return 1;
	}
	return 0;
}

int verificaVittoria(void){
	int y;
	y = posizionePedine[giocatore - 1][1];
	if(giocatore == 1){
		if(y == DIM_SCACCHIERA - 1)
			return 1;
	}
	else{
		if(y == 0)
			return 1;
	}
	return 0;
}

int verificaOttimalita(uint8_t l, uint8_t g){
	if(l < lunghezzaPercorsoOttimo[g])
		return 1;
	return 0;
}

void aggiornaSoluzioneOttima(uint8_t p[DIM_SCACCHIERA * DIM_SCACCHIERA], uint8_t l, uint8_t g){
	int i;
	lunghezzaPercorsoOttimo[g] = l;
	for(i = 0; i < l; i++){
		mossePercorsoOttimo[g][i] = p[i];
	}
}

void ricercaPercorsoOttimo(uint8_t g, uint8_t x, uint8_t y, uint8_t s[DIM_SCACCHIERA][DIM_SCACCHIERA], uint8_t p[DIM_SCACCHIERA * DIM_SCACCHIERA], uint8_t l){
	uint8_t s_locale[DIM_SCACCHIERA][DIM_SCACCHIERA], i, j;
	uint8_t p_locale[DIM_SCACCHIERA * DIM_SCACCHIERA];
	for(i =0; i < DIM_SCACCHIERA; i++){
		for(j = 0; j < DIM_SCACCHIERA; j++)
			s_locale[i][j] = s[i][j];
	}
	for(i = 0; i < l; i++)
		p_locale[i] = p[i];
	if(g == 0 && y == DIM_SCACCHIERA - 1){ // condizione finale
		if(verificaOttimalita(l, g))
			aggiornaSoluzioneOttima(p, l, g);
			return;
	}
	else if(g == 1 && y == 0){
		if(verificaOttimalita(l, g))
			aggiornaSoluzioneOttima(p, l, g);
			return;
	}
	s_locale[y][x] = 1;
	if(l > lunghezzaPercorsoOttimo[g])
		return;
	// chiamate ricorsive
	if(g == 0){
		if(y + 1 < DIM_SCACCHIERA && scacchiera[(y * 2) + 1][x * 2] != 4){ // down
			if(!s[y + 1][x]){
				p_locale[l] = DOWN;
				ricercaPercorsoOttimo(g, x, y + 1, s_locale, p_locale, l + 1);
			}
		}
		if(x - 1 >= 0 && scacchiera[y * 2][(x * 2) - 1] != 4){ // left
			if(!s[y][x - 1]){
				p_locale[l] = LEFT;
				ricercaPercorsoOttimo(g, x - 1, y, s_locale, p_locale, l + 1);
			}
		}
		if(x + 1 < DIM_SCACCHIERA && scacchiera[y * 2][(x * 2) + 1] != 4){ // right
			if(!s[y][x + 1]){
				p_locale[l] = RIGHT;
				ricercaPercorsoOttimo(g, x + 1, y, s_locale, p_locale, l + 1);
			}
		}
		if(y - 1 >= 0 && scacchiera[(y * 2) - 1][x * 2] != 4){ // up
			if(!s[y - 1][x]){
				p_locale[l] = UP;
				ricercaPercorsoOttimo(g, x, y - 1, s_locale, p_locale, l + 1);
			}
		}
	}
	else{
		if(y - 1 >= 0 && scacchiera[(y * 2) - 1][x * 2] != 4){ // up
			if(!s[y - 1][x]){
				p_locale[l] = UP;
				ricercaPercorsoOttimo(g, x, y - 1, s_locale, p_locale, l + 1);
			}
		}
		if(x - 1 >= 0 && scacchiera[y * 2][(x * 2) - 1] != 4){ // left
			if(!s[y][x - 1]){
				p_locale[l] = LEFT;
				ricercaPercorsoOttimo(g, x - 1, y, s_locale, p_locale, l + 1);
			}
		}
		if(x + 1 < DIM_SCACCHIERA && scacchiera[y * 2][(x * 2) + 1] != 4){ // right
			if(!s[y][x + 1]){
				p_locale[l] = RIGHT;
				ricercaPercorsoOttimo(g, x + 1, y, s_locale, p_locale, l + 1);
			}
		}
		if(y + 1 < DIM_SCACCHIERA && scacchiera[(y * 2) + 1][x * 2] != 4){ // down
			if(!s[y + 1][x]){
				p_locale[l] = DOWN;
				ricercaPercorsoOttimo(g, x, y + 1, s_locale, p_locale, l + 1);
			}
		}
	}
	return;
}
