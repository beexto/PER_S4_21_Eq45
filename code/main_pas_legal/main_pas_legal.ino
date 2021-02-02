#include "M5Stack.h"

#define int int32_t
//Definitions pour l'ecran
enum{
	BLANC,//0
	VERT,//1
	BLEU,//2
	ROUGE//3
};
int color = BLANC;
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

bool pressed=false;
void phaseUNO(void)//bouton C est le bouton de droite
{
	if(M5.BtnC.wasPressed())//test appuis court
	{
		pressed=true;
	}
	else if(M5.BtnC.pressedFor(1000))//test appuis long
	{
		color=BLANC;
    pressed=false;
	}
	if(M5.BtnC.wasReleased() && pressed)
 {
  pressed=false;
  color++;
    if(color>ROUGE)
      color=BLANC;
 }
	/*
	Mettre la couleur sur l'ecran
	*/
	switch(color)
	{
		case BLANC : M5.Lcd.fillScreen(WHITE); break;
		case VERT : M5.Lcd.fillScreen(GREEN); break;
		case BLEU : M5.Lcd.fillScreen(BLUE); break;
		case ROUGE : M5.Lcd.fillScreen(RED); break;
		default: break;
	}
}
