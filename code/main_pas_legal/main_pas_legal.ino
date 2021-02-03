#include "M5Stack.h"
#include <Adafruit_NeoPixel.h>
#define M5STACK_FIRE_NEO_NUM_LEDS 10
#define M5STACK_FIRE_NEO_DATA_PIN 15
#define courbeSIZE 30
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(M5STACK_FIRE_NEO_NUM_LEDS, M5STACK_FIRE_NEO_DATA_PIN, NEO_GRB + NEO_KHZ800);
int lum=0;
//int freq[6]={30,30,20,15,10,125};
int freq[6]={0,2000,1000,500,250,125};
int tfreq=0;//periode à laquelle on doit clignoter
int courbe[courbeSIZE]={0,0,0,20,53,85,114,142,167,190,209,225,238,247,253,255,253,247,238,225,209,190,167,142,114,85,53,20,0,0};//400octets-->1%delamemoiredesprogrammes
int icourbe=0;
int upd=0;
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
	
	void updateRGB()
	{
		switch(couleur)
		{
			case BLANC :
			r=255;
			g=255;
			b=255;
			break;
			
			case VERT :
			r=0;
			g=255;
			b=0;
			break;
			
			case BLEU :
			r=0;
			g=0;
			b=255;
			break;
			
			case ROUGE :
			r=255;
			g=0;
			b=0;
			break;
		}
	}
	
};
COLOR color;
//==============
void setup()
{
	
	M5.begin();//initialisation de l'objet M5stack -- celui qui contient les boutons
	M5.Power.begin();//allumage des peripheriques -- les leds sur les bords
	M5.Lcd.fillScreen(WHITE);//on remplis initialement en en blanc l'écran
	//Serial.begin(115200);
	//initPhase2();
}

void loop()
{
	M5.BtnC.read();//lecture de l'état des boutons
	Speaker.update();
	phaseUNO();//fonction de la premiere phase client
	phase2();
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
		color.couleur=BLANC;
		upd=1;
		pressed=false;
	}
	if(M5.BtnC.wasReleased() && pressed)
	{
		pressed=false;
		color.couleur++;
		if(color.couleur>ROUGE)//si couleur rouge on repasse au blanc
			color.couleur=VERT;
		upd=1;
	}
	/*
	Mettre la couleur sur l'ecran
	*/
	if(upd==1){
		switch(color.couleur)
		{
			case BLANC : M5.Lcd.fillScreen(WHITE); break;
			case VERT : M5.Lcd.fillScreen(GREEN); break;
			case BLEU : M5.Lcd.fillScreen(BLUE); break;
			case ROUGE : M5.Lcd.fillScreen(RED); break;
			default: break;
		}
	 upd=0;
	}
}


void initPhase2()
{
	for(int i=0;i<courbeSIZE;i++)
	{
		courbe[i]=255*abs(sin((3.14/(courbeSIZE))*i));
	}
}

void phase2()//active toutes les fonctions de la phase 2
{
	if(color.couleur==BLANC)
	{
		Serial.println("BLANC");
		tattente=millis();
		lastmillis=millis();
		tfreq=0;
	}
	else
	{
		actionGraph();
		changeFreq();
	}
	
	afficherGraph();
}

void actionGraph()//fait clignoter les bargraph //le min cest 2ms par boucle a la phase2
{
	if((millis()-lastmillis)>(tfreq/courbeSIZE))//si il est temps de passer a la case de la courbe d'apres
	{//le temps depuis le dernier mouvement>temps sur une case du tableau
		icourbe++;
		Serial.println((String)"T="+(millis()-lastmillis)+"--tfreq/courbeSIZE="+(tfreq/courbeSIZE));
		
		lastmillis=millis();
		if(icourbe>courbeSIZE)
		icourbe=0;
	}
}

void afficherGraph()/*On change la couleur et la luminosité de chaque led*/
{
	
	color.updateRGB();
	for(int i=0;i<M5STACK_FIRE_NEO_NUM_LEDS;i++)
	{
		if(tfreq==0)
		{
			//Serial.println((String)"tfreq=0");
			pixels.clear();
		}
		else
		{
			//Serial.println((String)"tfreq="+tfreq);
		  pixels.setPixelColor(i, color.r, color.g, color.b);
		  pixels.setBrightness(courbe[icourbe]);
		}
	}
	pixels.show();
}

void changeFreq()//change tfreq pour savoir à quelle frequenece clignoter
{
	int a=(int)((millis()-tattente)/60000);
	
	if(a>5)
		a=5;
	tfreq=freq[a];
}
