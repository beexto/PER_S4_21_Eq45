#include "M5Stack.h"

#define int int32_t
//Definitions pour l'ecran
enum{
	WHITE,
	GREEN,
	BLUE,
	RED
};
int color = WHITE;
//==============
void setup()
{
M5.begin();
M5.Power.begin();
M5.Lcd.fillScreen(WHITE);
}

void loop()
{
	M5.update();
	phaseUNO();
}

void phaseUNO(void)//bouton C est le bouton de droite
{
	if(M5.BtnC.pressedFor(1000))
	{
		color++;
		if(color>RED){
			color=WHITE;
	}
	else if(M5.BtnC.wasPressed())
	{
		color=WHITE
	}
	M5.Lcd.fillScreen(color);
}