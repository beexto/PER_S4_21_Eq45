#include <WiFi.h>
#include <HTTPClient.h>
const char* ssid     = "BusyLight";
const char* password = "imsobusy";

const char* host = "192.168.60.1";
const int httpPort = 80;
#define RFID 0
#if RFID
#include <Wire.h>
#include "MFRC522_I2C.h"
MFRC522 mfrc522(0x28);   // Create MFRC522 instance.
#endif

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
bool sent=false;//stocke si la requete http a été envoyé
bool envoiblanc=false;//condition pour ne pas spammer les requetes http si l'ecran est blanc
//bool rfid=false;//condition pour ne pas passer tant que la carte n'est pas lu 
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
		/*RFID*/
		Serial.println("init pcd");
	#if RFID	
	mfrc522.PCD_Init();             // Init MFRC522
	Serial.println("init rfid");
	ShowReaderDetails();  
	#endif	// Show details of PCD - MFRC522 Card Reader details
	//initPhase2();//rempli le tableau courbe[] avec une sin redressé
	pixels.clear();
	pixels.show();
	tuto();
	Serial.println();
	#if RFID
	phaseRFID(); //aquisition et traitement du RFID
	M5.Lcd.fillScreen(WHITE);
	M5.Lcd.setCursor(1, 70, 2);
	M5.Lcd.setTextColor(TFT_BLACK,TFT_WHITE);
	M5.Lcd.setTextFont(4);	
	M5.Lcd.println("Votre carte est lu");
	delay(200);
	#endif
    connectWifi();
	
	

	/*remise à zero des variables*/
	pressed=false;
	color.couleur=BLANC;
	
}

void loop()
{
	//Serial.println("loop");
	M5.BtnC.read();//lecture de l'état des boutons
	M5.Speaker.update();
	phase1();//fonction de la premiere phase client
	phase2();//fonction de la deuxieme phase client
	phase3();
	
}


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
		envoiblanc=true;
	}
	/*
	Mettre la couleur sur l'ecran
	*/
	if(upd==1){
		sent=false;
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
		//Serial.println("BLANC");
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
		//Serial.println((String)"T="+(millis()-lastmillis)+"--tfreq/courbeSIZE="+(tfreq/courbeSIZE));
		
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
		  pixels.setPixelColor(i, color.r, color.g, color.b);//change la couleur du pixel d'indice i
		  pixels.setBrightness(courbe[icourbe]);//change la luminosité des barGraph
		}
	}
	pixels.show();//envoi aux barGraph les infos de couleur et de luminositée
}

void changeFreq()//change tfreq pour savoir à quelle frequenece clignoter
{
	int a=(int)((millis()-tattente)/60000);
	
	if(a>5)
		a=5;
	tfreq=freq[a];
}

void tuto()
{
	blanc:
	M5.Lcd.fillScreen(WHITE);
	M5.Lcd.setCursor(30, 20, 2);
	M5.Lcd.setTextColor(TFT_BLACK,TFT_WHITE);
	M5.Lcd.setTextFont(4);
	M5.Lcd.println("Blanc pas de demande");
	M5.Lcd.setTextColor(TFT_BLACK,TFT_WHITE);
	M5.Lcd.setCursor(225, 210, 4);
	M5.Lcd.println("Next");
	M5.Lcd.setCursor(130, 210, 4);
	M5.Lcd.println("Skip");
	M5.Lcd.setCursor(35, 210, 4);
	M5.Lcd.println("Prev");
	while(1)
	{
		if(M5.BtnC.wasPressed())
			pressed=true;
		M5.BtnC.read();	
		if(M5.BtnC.wasReleased() && pressed)
			break;//si bouton droite alors tuto suivant
		
		if(M5.BtnA.wasPressed())
			pressed=true;
		M5.BtnA.read();	
		if(M5.BtnA.wasReleased() && pressed)
			goto blanc;//si bouton gauche aors tuto precedent
		
		if(M5.BtnB.wasPressed())
			pressed=true;
		M5.BtnB.read();	
		if(M5.BtnB.wasReleased() && pressed)
			return;//si bouton milieu alors skip tuto
	}
	vert:
	pressed=false;
	M5.Lcd.fillScreen(GREEN);
	M5.Lcd.setCursor(60, 20, 2);
	M5.Lcd.setTextColor(TFT_BLACK,TFT_GREEN);
	M5.Lcd.setTextFont(4);
	M5.Lcd.println("Vert : demande");
	M5.Lcd.setCursor(60, 45, 4);
	M5.Lcd.println("de verification");
	M5.Lcd.setTextColor(TFT_BLACK,TFT_GREEN);
	M5.Lcd.setCursor(225, 210, 4);
	M5.Lcd.println("Next");
	M5.Lcd.setCursor(130, 210, 4);
	M5.Lcd.println("Skip");
	M5.Lcd.setCursor(35, 210, 4);
	M5.Lcd.println("Prev");
	while(1)
	{
		if(M5.BtnC.wasPressed())
			pressed=true;
		M5.BtnC.read();	
		if(M5.BtnC.wasReleased() && pressed)
			break;
		
		if(M5.BtnA.wasPressed())
			pressed=true;
		M5.BtnA.read();	
		if(M5.BtnA.wasReleased() && pressed)
			goto blanc;
		
		if(M5.BtnB.wasPressed())
			pressed=true;
		M5.BtnB.read();	
		if(M5.BtnB.wasReleased() && pressed)
			return;
	}
	bleu:
	pressed=false;
	M5.Lcd.fillScreen(BLUE);
	M5.Lcd.setCursor(60, 20, 2);
	M5.Lcd.setTextColor(TFT_WHITE,TFT_BLUE);
	M5.Lcd.setTextFont(4);
	M5.Lcd.println("Bleu : demande");
	M5.Lcd.setCursor(60, 45, 4);
	M5.Lcd.println("d'explication");
	M5.Lcd.setTextColor(TFT_WHITE,TFT_BLUE);
	M5.Lcd.setCursor(225, 210, 4);
	M5.Lcd.println("Next");
	M5.Lcd.setCursor(130, 210, 4);
	M5.Lcd.println("Skip");
	M5.Lcd.setCursor(35, 210, 4);
	M5.Lcd.println("Prev");
	while(1)
	{
		if(M5.BtnC.wasPressed())
			pressed=true;
		M5.BtnC.read();	
		if(M5.BtnC.wasReleased() && pressed)
			break;
		
		if(M5.BtnA.wasPressed())
			pressed=true;
		M5.BtnA.read();	
		if(M5.BtnA.wasReleased() && pressed)
			goto vert;
		
		if(M5.BtnB.wasPressed())
			pressed=true;
		M5.BtnB.read();	
		if(M5.BtnB.wasReleased() && pressed)
			return;
	}
	rouge:
	pressed=false;
	M5.Lcd.fillScreen(RED);
	M5.Lcd.setCursor(60, 20, 2);
	M5.Lcd.setTextColor(TFT_WHITE,TFT_RED);
	M5.Lcd.setTextFont(4);
	M5.Lcd.println("Rouge : demande");
	M5.Lcd.setCursor(60, 45, 4);
	M5.Lcd.println("d'accompagnement");
	M5.Lcd.setTextColor(TFT_WHITE,TFT_RED);
	M5.Lcd.setCursor(225, 210, 4);
	M5.Lcd.println("Next");
	M5.Lcd.setCursor(130, 210, 4);
	M5.Lcd.println("Skip");
	M5.Lcd.setCursor(35, 210, 4);
	M5.Lcd.println("Prev");
	while(1)
	{
		if(M5.BtnC.wasPressed())
			pressed=true;
		M5.BtnC.read();	
		if(M5.BtnC.wasReleased() && pressed)
			break;
		
		if(M5.BtnA.wasPressed())
			pressed=true;
		M5.BtnA.read();	
		if(M5.BtnA.wasReleased() && pressed)
			goto bleu;
		
		if(M5.BtnB.wasPressed())
			pressed=true;
		M5.BtnB.read();	
		if(M5.BtnB.wasReleased() && pressed)
			return;
	}	
	
	
}

void phase3()
{
	if(((millis()-tattente>2000) && !sent)||((color.couleur==BLANC) && !sent && envoiblanc))
	{
		HTTPClient http;
		http.begin((String)"http://192.168.60.1/entry.php?"+"cID="+"eq45"+"&lvl="+color.couleur ); //envoi une requete http au serveur avec les infos d'identification et de couleur
		int httpCode = http.GET();                                        //envoie la requete et recupere la code http du serveur
 
    if (httpCode > 0) { //Check for the returning code // si <0 alors bug dans la librarie
 
        String payload = http.getString();
        Serial.println(httpCode);//devrait etre =200
        Serial.println(payload);
		sent=true;
		envoiblanc=false;
      }
 
    else {
      Serial.println("Error on HTTP request");
    }
 
    http.end(); //Free the resources
	}
}

void connectWifi()
{
	Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
		M5.Lcd.fillScreen(WHITE);
		M5.Lcd.setCursor(1, 70, 2);
		M5.Lcd.setTextColor(TFT_BLACK,TFT_WHITE);
		M5.Lcd.setTextFont(4);
		M5.Lcd.print("Connecting");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

	M5.Lcd.fillScreen(WHITE);
	M5.Lcd.setCursor(1, 70, 2);
	M5.Lcd.setTextColor(TFT_BLACK,TFT_WHITE);
	M5.Lcd.setTextFont(4);
	M5.Lcd.println((String)"SSID:"+ssid);
	M5.Lcd.print("IP:");
	M5.Lcd.println(WiFi.localIP());
}
#if RFID
void ShowReaderDetails() {
  // Get the MFRC522 software version
  byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  Serial.print(F("MFRC522 Software Version: 0x"));
  Serial.print(v, HEX);
  if (v == 0x91)
    Serial.print(F(" = v1.0"));
  else if (v == 0x92)
    Serial.print(F(" = v2.0"));
  else
    Serial.print(F(" (unknown)"));
  Serial.println("");
  // When 0x00 or 0xFF is returned, communication probably failed
  if ((v == 0x00) || (v == 0xFF)) {
    Serial.println(F("WARNING: Communication failure, is the MFRC522 properly connected?"));
  }
}


void phaseRFID(){
	M5.Lcd.fillScreen(WHITE);
	M5.Lcd.setCursor(1, 70, 2);
	M5.Lcd.setTextColor(TFT_BLACK,TFT_WHITE);
	M5.Lcd.setTextFont(4);
	M5.Lcd.println("Mettre votre carte etudiante");
	M5.Lcd.setCursor(1, 110, 2);
	M5.Lcd.setTextColor(TFT_BLACK,TFT_WHITE);
	M5.Lcd.setTextFont(4);
	M5.Lcd.println("sur le lecteur RFID");
	while(1){
	if (  mfrc522.PICC_IsNewCardPresent() ) {
	mfrc522.PICC_ReadCardSerial();
	Serial.println("RFID:");
	for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
		}
	break;
	}
		
	}
}
#endif