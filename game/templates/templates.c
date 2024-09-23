#include "templates.h"

extern int schermata;
extern int selezione;
extern int coloriGiocatori[2];
extern int mioId;

void templateIniziale(void){
	unsigned char s1[] = "QUORIDOR";
	unsigned char s2[] = "  ";
	unsigned char s3[] = "Press INT0 to";
	unsigned char s4[] = "start playing";
	
	LCD_Clear(White);
	
	GUI_Text(85, 40, s1, Black, White);
	GUI_Text(100, 55, s2, Red, Red);
	GUI_Text(120, 55, s2, Blue, Blue);
	
	GUI_Text(70, 167, s3, Black, Yellow);
	GUI_Text(70, 180, s4, Black, Yellow);
	
	schermata = TEMPL_INIZ;
}

void templateSelezionaGameMode(void){
	int i;
	unsigned char s1[] = "Select the";
	unsigned char s2[] = "GAME MODE";
	unsigned char s3[] = "Single Board";
	unsigned char s4[] = "Two Boards";
	
	for(i = 110; i < 200; i++)
		LCD_DrawLine(0, i, 240, i, White);
	
	GUI_Text(75, 120, s1, Black, White);
	GUI_Text(80, 135, s2, Black, White);
	
	LCD_DrawLine(60, 160, 180, 160, Red);
	LCD_DrawLine(60, 190, 180, 190, Red);
	LCD_DrawLine(60, 160, 60, 190, Red);
	LCD_DrawLine(180, 160, 180, 190, Red);
	for(i = 161; i < 190; i++)
		LCD_DrawLine(61, i, 179, i, Yellow);
	GUI_Text(72, 167, s3, Black, Yellow);
	selezione = 0;
	
	LCD_DrawLine(60, 210, 180, 210, Red);
	LCD_DrawLine(60, 240, 180, 240, Red);
	LCD_DrawLine(60, 210, 60, 240, Red);
	LCD_DrawLine(180, 210, 180, 240, Red);
	GUI_Text(80, 217, s4, Black, White);
	
	schermata = SEL_GAME_MODE;
}

void templateSingleBoard(void){
	int i;
	unsigned char s1[] = "Single Board: select";
	unsigned char s2[] = "the opposite player";
	unsigned char s3[] = "Human";
	unsigned char s4[] = "NPC";
	
	for(i = 110; i < 315; i++)
		LCD_DrawLine(0, i, 240, i, White);
	
	
	
	GUI_Text(40, 120, s1, Black, White);
	GUI_Text(44, 135, s2, Black, White);
	
	LCD_DrawLine(60, 160, 180, 160, Red);
	LCD_DrawLine(60, 190, 180, 190, Red);
	LCD_DrawLine(60, 160, 60, 190, Red);
	LCD_DrawLine(180, 160, 180, 190, Red);
	for(i = 161; i < 190; i++)
		LCD_DrawLine(61, i, 179, i, Yellow);
	GUI_Text(100, 167, s3, Black, Yellow);
	selezione = 0;
	
	LCD_DrawLine(60, 210, 180, 210, Red);
	LCD_DrawLine(60, 240, 180, 240, Red);
	LCD_DrawLine(60, 210, 60, 240, Red);
	LCD_DrawLine(180, 210, 180, 240, Red);
	GUI_Text(107, 217, s4, Black, White);
	
	schermata = SINGLE_BOARD;
}

void templateTwoBoard(void){
	int i;
	unsigned char s1[] = "Two Boards: select";
	unsigned char s2[] = "your player";
	unsigned char s3[] = "Human";
	unsigned char s4[] = "NPC";
	unsigned char s5[] = "Connection succesful!";
	unsigned char s6[] = "You are Player 1";
	unsigned char s7[] = "You are Player 2";
	unsigned char p[] = "  ";
	
	for(i = 110; i < 315; i++)
		LCD_DrawLine(0, i, 240, i, White);
	
	GUI_Text(45, 120, s1, Black, White);
	GUI_Text(75, 135, s2, Black, White);
	
	LCD_DrawLine(60, 160, 180, 160, Red);
	LCD_DrawLine(60, 190, 180, 190, Red);
	LCD_DrawLine(60, 160, 60, 190, Red);
	LCD_DrawLine(180, 160, 180, 190, Red);
	for(i = 161; i < 190; i++)
		LCD_DrawLine(61, i, 179, i, Yellow);
	GUI_Text(100, 167, s3, Black, Yellow);
	selezione = 0;
	
	LCD_DrawLine(60, 210, 180, 210, Red);
	LCD_DrawLine(60, 240, 180, 240, Red);
	LCD_DrawLine(60, 210, 60, 240, Red);
	LCD_DrawLine(180, 210, 180, 240, Red);
	GUI_Text(107, 217, s4, Black, White);
	
	GUI_Text(35, 265, s5, Black, Yellow);
	if(mioId == 1){
		GUI_Text(55, 280, s6, Black, Yellow);
		GUI_Text(112, 300, p, coloriGiocatori[0], coloriGiocatori[0]);
	}
	else{
		GUI_Text(55, 280, s7, Black, Yellow);
		GUI_Text(112, 300, p, coloriGiocatori[1], coloriGiocatori[1]);
	}
	
	schermata = TWO_BOARDS;
}

void templateSelezionaGiocatoreUtente(void){
	int i;
	unsigned char s1[] = "Single Board vs NPC:";
	unsigned char s2[] = "select your player";
	unsigned char p1[] = "Player 1";
	unsigned char p2[] = "Player 2";
	unsigned char p[] = "  ";
	
	for(i = 110; i < 250; i++)
		LCD_DrawLine(0, i, 240, i, White);
	
	GUI_Text(40, 120, s1, Black, White);
	GUI_Text(48, 135, s2, Black, White);
	
	LCD_DrawLine(60, 160, 180, 160, Red);
	LCD_DrawLine(60, 190, 180, 190, Red);
	LCD_DrawLine(60, 160, 60, 190, Red);
	LCD_DrawLine(180, 160, 180, 190, Red);
	for(i = 161; i < 190; i++)
		LCD_DrawLine(61, i, 179, i, Yellow);
	GUI_Text(80, 167, p1, Black, Yellow);
	GUI_Text(150, 167, p, coloriGiocatori[0], coloriGiocatori[0]);
	selezione = 0;
	
	LCD_DrawLine(60, 210, 180, 210, Red);
	LCD_DrawLine(60, 240, 180, 240, Red);
	LCD_DrawLine(60, 210, 60, 240, Red);
	LCD_DrawLine(180, 210, 180, 240, Red);
	GUI_Text(80, 217, p2, Black, White);
	GUI_Text(150, 217, p, coloriGiocatori[1], coloriGiocatori[1]);
	
	schermata = SEL_GIOC_UTENTE;
}
