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

void act_couleur(int couleur)
{
switch(couleur)
{
	case WHITE : M5.Lcd.fillScreen(WHITE); break;
	case GREEN : M5.Lcd.fillScreen(GREEN); break;
	case BLUE : M5.Lcd.fillScreen(BLUE); break;
	case RED : M5.Lcd.fillScreen(RED); break;
}
}