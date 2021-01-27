#include "M5Stack.h"



void setup()
{
M5.begin();
M5.Power.begin();
enum{
	WHITE,
	GREEN,
	BLUE,
	RED
	}
int color = WHITE;
M5.Lcd.fillScreen(WHITE);

}

void loop()
{

}

void act_couleur()//Rempli l'écran de la couleur séléctionnée
{
switch(color)
{
	case WHITE : M5.Lcd.fillScreen(WHITE); break;
	case GREEN : M5.Lcd.fillScreen(GREEN); break;
	case BLUE : M5.Lcd.fillScreen(BLUE); break;
	case RED : M5.Lcd.fillScreen(RED); break;
}
}

void trt_couleur()//Passe d'une couleur a l'autre dans l'ordre
{
	color++;
	if(color>RED){
		color=WHITE;
	}
	else;
}