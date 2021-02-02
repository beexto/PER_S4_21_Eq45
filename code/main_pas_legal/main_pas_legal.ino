#include "M5Stack.h"


#include <Adafruit_NeoPixel.h>
#define M5STACK_FIRE_NEO_NUM_LEDS 10
#define M5STACK_FIRE_NEO_DATA_PIN 15
#define courbeSIZE 100
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(M5STACK_FIRE_NEO_NUM_LEDS, M5STACK_FIRE_NEO_DATA_PIN, NEO_GRB + NEO_KHZ800);
int lum=0;
int tfreq=0;
int courbe[100];
int icourbe=0;
unsigned long tattente=0;
unsigned long lastmillis=0;
#define int int32_t
//Definitions pour l'ecran
enum{
	BLANC,//0
	VERT,//1
	BLEU,//2
	ROUGE//3
};
class COLOR{
	public :
	int couleur= BLANC;
	int r=0;
	int g=0;
	int b=0;
};
COLOR color;
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
		colorcouleur=BLANC;
    pressed=false;
	}
	if(M5.BtnC.wasReleased() && pressed)
 {
  pressed=false;
  color.couleur++;
    if(color.couleur>ROUGE)//si couleur rouge on repasse au blanc
      color.couleur=VERT;
 }
	/*
	Mettre la couleur sur l'ecran
	*/
	switch(color.couleur)
	{
		case BLANC : M5.Lcd.fillScreen(WHITE); break;
		case VERT : M5.Lcd.fillScreen(GREEN); break;
		case BLEU : M5.Lcd.fillScreen(BLUE); break;
		case ROUGE : M5.Lcd.fillScreen(RED); break;
		default: break;
	}
}


void initPhase2()
{
	for(int i=0;i<courbeSIZE;i++)
	{
		courbe[i]=255*abs(sin((1/(courbeSIZE*2))*i));
	}
}

void phaseDeux()//active toutes les fonctions de la phase 2
{
	if(color==BLANC)
	{
		tattente=0;
		lastmillis=0;
	}
	else
	{
		tattente=millis();
		actionGraph();
		changeFreq();
	}
}

void actionGraph()//fait clignoter les bargraph
{
	if(lastmillis==0)//si premier passage on initialise lastmillis au temps actuel
		lastmillis=tattente;	
		
	if((millis()-lastmillis)>(tfreq/courbeSIZE))//si il est temps de passer a la case de la courbe d'apres
	{	
		icourbe++;
		if(icourbe>courbeSIZE)
		icourbe=0;
	}
	
	/*On change la couleur et la luminosité de chaque led*/
	for(int i=0;i<M5STACK_FIRE_NEO_NUM_LEDS;i++)
	pixels.setPixelColor(pixelNumber, color.r, color.g, color.b,courbe[icourbe];  
	
	pixels.show();
}

void changeFreq()//change tfreq pour savoir à quelle frequenece clignoter
{
	if(tattente>60000)
	{
		tfreq=2000;		
	}
}

void translateColor()//mets en rgb la couelur actuelle
{
	
}