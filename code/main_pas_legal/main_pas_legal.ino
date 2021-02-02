#include "M5Stack.h"


#include <Adafruit_NeoPixel.h>
#define M5STACK_FIRE_NEO_NUM_LEDS 10
#define M5STACK_FIRE_NEO_DATA_PIN 15
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(M5STACK_FIRE_NEO_NUM_LEDS, M5STACK_FIRE_NEO_DATA_PIN, NEO_GRB + NEO_KHZ800);

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
M5.begin();//initialisation de l'objet M5stack -- celui qui contient les boutons
M5.Power.begin();//allumage des peripheriques -- les leds sur les bords
M5.Lcd.fillScreen(WHITE);//on remplis initialement en en blanc l'écran
}

void loop()
{
	M5.update();//lecture de l'état des boutons
	phaseUNO();//fonction de la premiere phase client
}

bool pressed=false;//stocke si le bouton à été appuyé lors de la derniere boucle
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
    if(color>ROUGE)//si couleur rouge on repasse au blanc
      color=VERT;
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
