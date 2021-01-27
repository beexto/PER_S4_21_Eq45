#include "M5Stack.h"

#define int int32_t

void setup()
{
M5.begin(true, false, true);
M5.Power.begin();
}

void loop()
{
	M5.update();
}

int ack_bouton(void)//bouton C est le bouton de droite
{
	if(M5.BtnC.pressedFor(1000))
		return 1;//si le bouton est appuyé pendant 1 sec
	else if(M5.BtnC.wasPressed())
		return 0;//si un appuis court
	else return -1;//si pas d'appuis
}

