#include "M5Stack.h"
#include <Adafruit_NeoPixel.h>
#define M5STACK_FIRE_NEO_NUM_LEDS 10
#define M5STACK_FIRE_NEO_DATA_PIN 15
#define courbeSIZE 50
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(M5STACK_FIRE_NEO_NUM_LEDS, M5STACK_FIRE_NEO_DATA_PIN, NEO_GRB + NEO_KHZ800);
int lum=0;
//int freq[6]={30,30,20,15,10,125};
int freq[6]={0,2000,1000,500,250,125};// les frequences à suivre
int tfreq=0;//periode à laquelle on doit clignoter
int courbe[courbeSIZE]={0,0,0,0,0,20,40,59,78,97,114,131,147,162,176,190,202,212,222,231,238,244,248,252,254,255,254,252,248,244,238,231,222,212,202,190,176,162,147,131,114,97,78,59,40,20,0,0,0,0};//400octets-->1%delamemoiredesprogrammes
int icourbe=0;//l'emplacement de courbe[] à lire
int upd=0;//pour savoir si l'ecran doit etre rafraichi
unsigned long tattente=0;//repere dans le temps de quand on est passé au mode d'attente (couleur autre que blanche)
unsigned long lastmillis=0;//repere dans le temps du dernier changement de icourbe
bool pressed=false;//stocke si le bouton à été appuyé lors de la derniere boucle
#define int int32_t// pour etre sûr que les int sont sur 32 bits
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
	//initPhase2();//rempli le tableau courbe[] avec une sin redressé
	pixels.clear();
	pixels.show();
	
	M5.Lcd.setCursor(30, 20, 2);
	M5.Lcd.setTextColor(TFT_WHITE,TFT_BLACK);
	M5.Lcd.setTextFont(4);
	M5.Lcd.println("Blanc pas de demande");
	M5.Lcd.setTextColor(TFT_WHITE,TFT_BLACK);
	M5.Lcd.setCursor(225, 210, 4);
	M5.Lcd.println("Next");
	while(1)
	{
		if(M5.BtnC.wasPressed())
			pressed=true;
		M5.BtnC.read();	
		if(M5.BtnC.wasReleased() && pressed)
			break;
	}
	pressed=false;
	M5.Lcd.fillScreen(GREEN);
	M5.Lcd.setCursor(60, 20, 2);
	M5.Lcd.setTextColor(TFT_WHITE,TFT_BLACK);
	M5.Lcd.setTextFont(4);
	M5.Lcd.println("Vert : demande");
	M5.Lcd.setCursor(60, 45, 4);
	M5.Lcd.println("de verification");
	M5.Lcd.setTextColor(TFT_WHITE,TFT_BLACK);
	M5.Lcd.setCursor(225, 210, 4);
	M5.Lcd.println("Next");
	while(1)
	{
		if(M5.BtnC.wasPressed())
			pressed=true;
		M5.BtnC.read();	
		if(M5.BtnC.wasReleased() && pressed)
			break;
	}
	pressed=false;
	M5.Lcd.fillScreen(BLUE);
	M5.Lcd.setCursor(60, 20, 2);
	M5.Lcd.setTextColor(TFT_WHITE,TFT_BLACK);
	M5.Lcd.setTextFont(4);
	M5.Lcd.println("Bleu : demande");
	M5.Lcd.setCursor(60, 45, 4);
	M5.Lcd.println("d'explication");
	M5.Lcd.setTextColor(TFT_WHITE,TFT_BLACK);
	M5.Lcd.setCursor(225, 210, 4);
	M5.Lcd.println("Next");
	while(1)
	{
		if(M5.BtnC.wasPressed())
			pressed=true;
		M5.BtnC.read();	
		if(M5.BtnC.wasReleased() && pressed)
			break;
	}
	pressed=false;
	M5.Lcd.fillScreen(RED);
	M5.Lcd.setCursor(60, 20, 2);
	M5.Lcd.setTextColor(TFT_WHITE,TFT_BLACK);
	M5.Lcd.setTextFont(4);
	M5.Lcd.println("Rouge : demande");
	M5.Lcd.setCursor(60, 45, 4);
	M5.Lcd.println("d'accompagnement");
	M5.Lcd.setTextColor(TFT_WHITE,TFT_BLACK);
	M5.Lcd.setCursor(225, 210, 4);
	M5.Lcd.println("Next");
	while(1)
	{
		if(M5.BtnC.wasPressed())
			pressed=true;
		M5.BtnC.read();	
		if(M5.BtnC.wasReleased() && pressed)
			break;
	}	
	pressed=false;
	color.couleur=BLANC;
	M5.Lcd.fillScreen(WHITE);
}

void loop()
{
	M5.BtnC.read();//lecture de l'état des boutons
	M5.Speaker.update();
	phaseUNO();//fonction de la premiere phase client
	phase1();//fonction de la premiere phase client
	phase2();//fonction de la deuxieme phase client
}


void phaseUNO(void)//bouton C est le bouton de droite
void phase1(void)//bouton C est le bouton de droite
{
	if(M5.BtnC.wasPressed())//test appuis court
	{
		pressed=true;
   /*On mets a jour les repere*/
    tattente=millis();
    lastmillis=millis();
    tfreq=0;//on ne clignote pas
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
		if(color.couleur>ROUGE)//si couleur rouge on repasse au vert
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


void initPhase2()//test de courbe
{
	for(int i=0;i<courbeSIZE;i++)
	{
		courbe[i]=255*abs(sin((3.14/(courbeSIZE))*i));//sin redressé
	}
}

void phase2()//active toutes les fonctions de la phase 2
{
	if(color.couleur==BLANC)
	{
		Serial.println("BLANC");
		/*On mets a jour les repere*/
		tattente=millis();
		lastmillis=millis();
		tfreq=0;//on ne clignote pas
	}
	else
	{
		actionGraph();//on change icourbe si besoin
		changeFreq();//on change la frequence si besoin
	}
	
	afficherGraph();//on mets a jour les leds
}

void actionGraph()//fait clignoter les bargraph //le min cest 2ms par boucle a la phase2
{
	if((millis()-lastmillis)>(tfreq/courbeSIZE))//si il est temps de passer a la case de la courbe d'apres
	{//le temps depuis le dernier mouvement>temps sur une case du tableau
		icourbe++;
		Serial.println((String)"T="+(millis()-lastmillis)+"--tfreq/courbeSIZE="+(tfreq/courbeSIZE));
		
		lastmillis=millis();
		if(icourbe>courbeSIZE)//remise à zero de icourbe si au bout de courbe[]
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
