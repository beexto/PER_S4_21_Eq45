#include "M5Stack.h"

#define int int32_t

enum{
	WHITE,
	GREEN,
	BLUE,
	RED
};
int color = WHITE;

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
	case WHITE : M5.Lcd.fillScreen(WHITE); break;
	case GREEN : M5.Lcd.fillScreen(GREEN); break;
	case BLUE : M5.Lcd.fillScreen(BLUE); break;
	case RED : M5.Lcd.fillScreen(RED); break;
	default: break;
}
}

void trt_couleur(int BP)//Passe d'une couleur a l'autre dans l'ordre
{
	if(BP==0){
		color++;
		if(color>RED){
			color=WHITE;
	}
		else;
	}
	else{color=WHITE;}
}
