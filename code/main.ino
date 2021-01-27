#include "M5Stack.h"

#define int int32_t

enum{
	BLANC,//0
	VERT,//1
	BLEU,//2
	ROUGE//3
};
int color = BLANC;

void setup()
{
M5.begin();
M5.Power.begin();
M5.Lcd.fillScreen(WHITE);
}

void loop()
{
	M5.update();
	trt_bouton();
}

int ack_bouton(void)//bouton C est le bouton de droite
{
	if(M5.BtnC.pressedFor(1000))
		return 1;//si le bouton est appuyé pendant 1 sec
	else if(M5.BtnC.wasPressed())
		return 0;//si un appuis court
	else return -1;//si pas d'appuis
}


void trt_bouton(void)
{
	trt_couleur(ack_bouton());
	
	act_couleur();
}

void act_couleur()//Rempli l'écran de la couleur séléctionnée
{
switch(color)
{
	case BLANC : M5.Lcd.fillScreen(WHITE); break;
	case VERT : M5.Lcd.fillScreen(GREEN); break;
	case BLEU : M5.Lcd.fillScreen(BLUE); break;
	case ROUGE : M5.Lcd.fillScreen(RED); break;
	default: break;
}
}

void trt_couleur(int BP)//Passe d'une couleur a l'autre dans l'ordre
{
	if(BP==0){
		color++;
		if(color>ROUGE){
			color=BLANC;
	}
		else;
	}
	else{color=BLANC;}
}
