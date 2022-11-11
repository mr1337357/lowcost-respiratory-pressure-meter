
///////////////////////////////////////////////////CODI TFG CLAUDIA AYMERICH/////////////////////////////////////////////////////


///////////////////////////////////////////LLIBRERIES///////////////////////////////////////////////////////////////////////////
#include <SPI.h> //libreries de la SD
#include <SD.h>

// Llibreries del tft
//#include <Adafruit_TFTLCD.h> // Core graphics library
#include <Adafruit_GFX.h>    // Hardware-specific library
#include <TouchScreen.h>
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;

////////////////////////////////////////DEFINICIONS/////////////////////////////////////////////////////////////////////////////
// we define the pins used by the LCD display
#define LCD_CS A3 
#define LCD_CD A2 
#define LCD_WR A1 
#define LCD_RD A0 
#define LCD_RESET A4 

#define SD_SCK 13
#define SD_MISO 12
#define SD_MOSI 11
#define SD_CS 5

// values found by screen calibration (feta per mi)
#define TS_MINX 129 //from calibration: x of right-lower corner
#define TS_MINY 127
#define TS_MAXX 938 //from calibration: x of left-upper corner
#define TS_MAXY 897


#if defined(__SAM3X8E__)
#undef __FlashStringHelper::F(string_literal)
#define F(string_literal) string_literal
#endif
// define the pins used to activate the touch screen
uint8_t YP = A1;  // must be an analog pin, use "An" notation!
uint8_t XM = A2;  // must be an analog pin, use "An" notation!
uint8_t YM = 7;   // can be a digital pin
uint8_t XP = 6;   // can be a digital pin
uint8_t SwapXY = 0;

uint16_t TS_LEFT = 880;
uint16_t TS_RT  = 170;
uint16_t TS_TOP = 950;
uint16_t TS_BOT = 180;
char *name = "Unknown controller";

// define color codes
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define ORANGE  0xE880

// fixed definition used by TFT LCD library
//Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

// fixed definition used by Touch Screen library
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
TSPoint tp;

///////////////////////////////////////////////////////VARIABLES/////////////////////////////////////////////////////
//Definim les variables
int FileToOpen = 1; 
char newfile[10]=" ";
byte t=0, k, a=0, indexMAX=3, indexMIN=0, b=0, dibuix=0, tipo=0, next=0, variable=1;
int px, py, counter=0, j, altura, contador=0, contacceptar=0;
float oldValueMIP=0, oldValueMEP=0, Value,maxim=0, minim=110, mitja, suma, holder;
// if we touch the red button, the program will display a message
boolean buttonEnabled1 = true; //MIP/MEP initial buttons
boolean buttonEnabled2= false; //next measurement
boolean buttonEnabled3= false; //stop
boolean buttonEnabled6= false; //Accept MIP
boolean buttonEnabled7= false; //Reject MIP
boolean buttonEnabled8= false; //Accept MEP
boolean buttonEnabled9= false; //Reject MEP
boolean buttonEnabled10= false; //change test
boolean buttonEnabled11= false; //off
int sensorPin = A15;
int batteryPin= A11;
int bat=0;
unsigned long interval=1000/70;// canvi (els llocs on posa canvi s'han de canviar si es canvia la freq)
unsigned long previousMillis=0; // millis() returns an unsigned long.
float myArray[350]; //70Hz
float mitjanes[12]={200,200,200,200,200,200,200,200,200,200,200,200}; //vector on guardarem les dades de la mitja 
//(el posem a 200 per tenir els valors obtinguts al principi del vector en ordre 
//creixent i així no fer una mitja amb els valors previs no obtinguts de l'experiment)
float pressions[12]; //vector on guardarem les dades de pressions per anar imprimint-los a mesura que s'accepten 


/////////////////////////////////////////////DIBUIX DE LES PANTALLES////////////////////////////////////////////////////
//Dibuix de la pantalla inicial
void startScreen(){
  tft.setCursor(75,50); //set cursor, default 0,0.Is placed in the top left corner: per dir on vols posar el text
  tft.setTextColor(WHITE); //set text color
  tft.setTextSize(2); //set text size (default is 1, also 2 and 3)
  tft.println("MIP and MEP measuring\n\n              test"); //print like in serial or println

  //drawing of MIP button
  tft.fillRect(70,140, 120, 40, BLUE); //rectangle of a certain width and height (x0,y0,w,h,color)
  tft.drawRect(70,140,120,40,BLUE); //frame
  tft.setCursor(80,150);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print("MIP test");

  //drawing of MEP button
  tft.fillRect(210,140, 120, 40, RED); 
  tft.drawRect(210,140,120,40,RED);
  tft.setCursor(220,150);
  tft.print("MEP test");
}

//Dibuix dels botons de tot el programa
void NextMeasure(){ 
  tft.fillRect(30,85,110,50,GREEN);
  tft.drawRect(30,85,110,50,GREEN); //frame
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.setCursor(60,20);
  tft.print("MIP AND MEP MEASURING TEST"); 
  tft.drawRect(54,13,168,21,WHITE);
  tft.setTextSize(2);
  tft.setCursor(40,50);
  tft.print("Options:");
  tft.drawRect(175,54,2,166,WHITE);
  tft.setCursor(60,91);
  tft.print("Next");
  tft.setCursor(45,111);
  tft.print("measure");
}

void Stop(){
  tft.fillRect(30,160,110,50,RED);
  tft.drawRect(30,160,110,50,RED);
  tft.setCursor(37,175);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print("End test");
}

void MEPvalue(){
  tft.fillScreen(BLACK);
  tft.setCursor(50,50);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.print("Maximal expiratory pressure"); 
  tft.setCursor(170,70);
  tft.print("(MEP)");   
}

void MIPvalue(){
  tft.fillScreen(BLACK);
  tft.setCursor(40,50);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.print("Maximal inspiratory pressure"); 
  tft.setCursor(170,70);
  tft.print("(MIP)");   
}


void AcceptMIP(){
  tft.fillRect(220,200, 80, 30, GREEN);
  tft.drawRect(220,200,80,30,GREEN); //frame
  tft.setCursor(225,205);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print("Accept");
  tft.drawRect(215,162,180,74,WHITE);  
  
}

void RejectMIP(){
  tft.fillRect(310,200, 80, 30, RED);
  tft.drawRect(310,200,80,30,RED); //frame
  tft.setCursor(315,205);
  tft.print("Reject");  
  
}

void AcceptMEP(){
  tft.fillRect(220,50, 80, 30, GREEN);
  tft.drawRect(220,50,80,30,GREEN); //frame
  tft.setCursor(225,55);
  tft.setTextColor(WHITE);
  tft.print("Accept");  
  tft.drawRect(215,10,180,80,WHITE);
}

void RejectMEP(){
  tft.fillRect(310,50, 80, 30, RED);
  tft.drawRect(310,50,80,30,RED); //frame
  tft.setCursor(315,55);
  tft.print("Reject");  
}

void ChangeTest(){
  tft.fillRect(270,70,80,30,MAGENTA);
  tft.drawRect(270,70,80,30,MAGENTA); //frame
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.setCursor(25,98);
  tft.print("The test will automatically begin."); 
  tft.setCursor(60,20);
  tft.print("MIP AND MEP MEASURING TEST"); 
  tft.drawRect(54,13,168,21,WHITE);
  tft.setCursor(275,75);
  tft.setTextSize(2);
  tft.print("Change"); 
  tft.setCursor(25,75);
  tft.print("Click to change test"); 
}

void Off(){
  tft.fillRect(270,160,80,30,RED); 
  tft.drawRect(270,160,80,30,RED); //frame 
  tft.setCursor(25,165);
  tft.setTextColor(WHITE);
  tft.print("Click to switch off"); 
  tft.setCursor(285,165);
  tft.print("Off");
}

void FinalScreen(){
  tft.setTextColor(WHITE);
  tft.setCursor(140,110);
  tft.print("Thank you");
}

//////////////////////////////////////DIBUIX EIXOS GRAFIQUES///////////////////////////////////////
//Dibuix dels eixos de MEP
void drawGraphMEP(){
  tft.fillScreen(BLACK);
  tft.setTextSize(2);
  tft.setTextColor(RED);
  tft.setCursor(90,20);
  tft.print("MEP TEST");
  tft.drawLine(24, 220, 382, 220, WHITE); //Eix x (starting and ending point) afegim 78 punts
  tft.drawLine(28, 41, 28, 224, WHITE); //Eix y
  tft.setCursor(5,25);
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.print(F("P (cmH2O)"));
  tft.setCursor(373, 205);
  tft.print("time");

//Ticks en eix y: nombre
  j=175;
  for (int i=45; i <=206; i=i+25){ //cada pixel és un punt
    tft.drawLine(26,i,30,i,WHITE); //x: dos abans i dos després, i igual pq mateixa altura 
    tft.setCursor(2, i+2); //i+2 pq el pal no es mengi el numero
    tft.print(j);
    j=j-25; 
  }

  //Ticks en eix x
  k=1;
  for (int m=98; m <=379; m=m+70){ //98 és el segon 1 és on comença la linia gran
    tft.drawLine(m,218,m,222,WHITE); //x: dos abans i dos després, i igual pq mateixa altura 
    tft.setCursor(m, 230); //m+2 pq el pal no es mengi el numero
    tft.print(k);
    k=k+1; 
  }
  //Tick origen
  tft.setCursor(10,222);
  tft.print("0"); //origen   
}

//Dibuix dels eixos de MIP
void drawGraphMIP(){
  tft.fillScreen(BLACK);
  tft.setTextSize(2);
  tft.setTextColor(BLUE);
  tft.setCursor(90,205);
  tft.print("MIP TEST");
  tft.drawLine(24, 20, 382, 20, WHITE); //Eix x 
  tft.drawLine(28, 16, 28, 199, WHITE); //Eix y
  tft.setCursor(5,215);
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.print(F("P (cmH2O)"));
  tft.setCursor(373, 35);
  tft.print("time");

//Ticks en eix y
  j=-25;
  for (int i=45; i <=196; i=i+25){ //12 és on comença la linia gran
    tft.drawLine(26,i,30,i,WHITE); //x: dos abans i dos després, i igual pq mateixa altura 
    tft.setCursor(2, i+2); //i+2 pq el pal no es mengi el numero
    tft.print(j);
    j=j-25; 
  }

//Ticks en eix x
k=1;
  for (int m=98; m <=379; m=m+70){ //12 és on comença la linia gran
    tft.drawLine(m,18,m,22,WHITE); //x: dos abans i dos després, i igual pq mateixa altura 
    tft.setCursor(m, 6); //m+2 pq el pal no es mengi el numero
    tft.print(k);
    k=k+1; 
  }

//Tick origen
  tft.setCursor(10,22);
  tft.print("0");   
}

////////////////////////////////////DIBUIX ESTATS BATERIA/////////////////////////////////////
//Poses void en una funció quan no ha de retornar res
void LowBattery(){
  //tft.fillRect(150,110,50,10,BLACK);
  //tft.fillRect(320,15,80,14,BLACK);
  tft.setTextColor(WHITE);
  tft.fillRect(340,15,5,14,RED);
  tft.drawRect(340,15,30,14,RED);
  tft.fillRect(370,21,4,2, RED);
}

void OrangeBattery(){
  //tft.fillRect(320,15,80,14,BLACK);
  tft.setTextColor(WHITE);
  tft.fillRect(340,15,15,14,YELLOW);
  tft.drawRect(340,15,30,14,YELLOW);
  tft.fillRect(370,21,4,2, YELLOW);
}

void GreenBattery(){
  //tft.fillRect(320,15,80,14,BLACK);
  tft.setTextColor(BLACK);
  tft.fillRect(340,15,30,14,GREEN);
  tft.drawRect(340,15,30,14,GREEN);
  tft.fillRect(370,21,4,2, GREEN);
}

void AllBattery(){
  //tft.fillRect(320,15,80,14,BLACK);
  tft.setTextColor(GREEN);
  tft.setCursor(340,15);
  tft.print("100%");
}

void Charge(){
  //tft.fillRect(320,15,80,14,BLACK);
  tft.setTextColor(RED);
  tft.setCursor(320,15);
  tft.print("CHARGE");
}



///////////////////////////////////////SETUP//////////////////////////////////////////////////////////////////////
void setup(void) {
  Serial.begin(9600);
  tft.reset(); // for starting communication of the TFT LCD Touch Screen
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  tft.setRotation(3); // rotating the display: done once in the setup, parameters: 0,1,2,3 (0,90,180 or 270 deg)
  tft.fillScreen(BLACK); //filling color of screen 
  startScreen(); //cridem pantalla principal (només es pot fer un cop)
  //BatteryState(batteryPin);   
}

////////////////////////////////////////////LOOP///////////////////////////////////////////////////////////
void loop() {  
  // put your main code here, to run repeatedly:
  tp = ts.getPoint(); //if we touch the red button, the program will display a message
  pinMode(XM, OUTPUT); //pin mode required for the libray
  pinMode(YP, OUTPUT);
  pinMode(XP, OUTPUT);
  pinMode(YM, OUTPUT);

  if (bat==0){
    //tft.setCursor(320,15);
    //tft.setTextColor(RED);
    //tft.print("hola");
    BatteryState(batteryPin);}
  
  

  //aquesta part involucra la part tàctil i els botons: s'han d'activar i desactivar quan toca
  if (tp.z > ts.pressureThreshhold) { //he de mirar quin és aquest threshold
    
   //tp.x = map(tp.x, TS_MINX, TS_MAXX, 0, 240); //map:convert the values got from the touch screen library in pixels
   //tp.y = map(tp.y, TS_MINY, TS_MAXY, 0, 400); //is the resolution of the screen we use in pixels
   //px = tp.y; //fem això per adequar tàctil amb posició de la pantalla que s'ha triat
   //py = tp.x;
   tp.x = map(tp.x, TS_LEFT, TS_RT,0,240);
   tp.y = map(tp.y, TS_TOP, TS_BOT,400,0 );
   px = tp.y; //fem això per adequar tàctil amb posició de la pantalla que s'ha triat
   py = tp.x;
   //Click butons de la pantalla incial
   
   //MEP button    
   if(px>210 && px<330 && py>140 && py<180 && buttonEnabled1){ //now we know the x,y values of the touch point
    //in pixels to male clickable buttons: if we touch the screen within these coordinates, button activated
    buttonEnabled1 = false; //we deactivate the button coordinates for the next page
    t=1;
    bat=1;
    drawGraphMEP();  
    }
   
       //MIP button    
   if(px>70 && px<190 && py>140 && py<180 && buttonEnabled1){ //now we know the x,y values of the touch point
    //in pixels to male clickable buttons: if we touch the screen within these coordinates, button activated
    buttonEnabled1 = false; //we deactivate the button coordinates for the next page
    //Aquí hauria de canviar una variable que et fes portar a la següent pantalla.
    t=2;
    bat=1;
    drawGraphMIP();
   }

   //quan acaba la mesura de MEP surten opcions acceptar i rebutjar, que porten a un altra pantalla
   if (next==1){ 
    contador=0, a=0, indexMAX=3, indexMIN=0, b=0, dibuix=0;
    counter=0; pressions[0]=0;
    oldValueMIP=0, oldValueMEP=0, Value,maxim=0, minim=110; 
    
    if(px>220 && px<300 && py>50 && py<80 && buttonEnabled8){  //cas d'acceptar: guardarem a sd. Parlar quines dades volen guardar exactament
      buttonEnabled8 = false; //acceptar
      buttonEnabled9=false; //rebutjar

      
      //discrepar max 10%
      contacceptar++;
      mitjanes[contacceptar] = mitja;
      pressions[contacceptar]=mitja;


      //ordenem en ordre creixent els valors obtinguts (en funció d'anar-los obtenint)
      for(byte x = 0; x < sizeof(mitjanes)/sizeof(float); x++)
        for(byte y = 0; y < sizeof(mitjanes)/sizeof(float)-1; y++)
          if(mitjanes[y] > mitjanes[y+1]  ) {
          holder = mitjanes[y+1];
          mitjanes[y+1] = mitjanes[y];
          mitjanes[y] = holder;
     }
      
     Serial.println("contador acceptar");
     Serial.println(contacceptar);
     if (contacceptar<3){ 
      variable=1; //aparegui la opció de next measurement
     }
     if (contacceptar>2){ //s'inicia el càlcul de la mitja de tres valors un cop tens tres valors (per evitar promitjar amb el valor 200 predefinit al array)
      for (int z=1; z <contacceptar-1; z++){
        float xvalue=mitjanes[z];
        Serial.print("xvalue");
        Serial.println(xvalue);
        Serial.println(xvalue-0.1*xvalue);
        Serial.println(xvalue+0.1*xvalue);
        if ((xvalue-0.1*xvalue)<mitjanes[z-1] && mitjanes[z-1]<(xvalue+0.1*xvalue)&& (xvalue-0.1*xvalue)<mitjanes[z+1] && mitjanes[z+1] <(xvalue+0.1*xvalue)){ 
          //el valor anterior i el posterior han d'estar compresos en l'interval corresponent (si no és el cas amb el segon valor es mira pel tercer, el quart fins comprovar per tots els packs de valors del vector)
          float valor1=mitjanes[z-1];
          float valor2=xvalue;
          float valor3=mitjanes[z+1];
          float MEP=valor3; //màxim dels tres valors
          //float MEP=(mitjanes[z-1]+xvalue+mitjanes[z+1])/3; //mitja dels tres valors 
          Serial.println("MEP final");
          Serial.println(MEP);

          z=contacceptar-1; //si trobem un valor per fer el MEP (3 valors q discrepen menys del 10%) parem el bucle per no seguir buscant més
          variable=2; //eliminem la opció next measurement
          MEPvalue();
          tft.setTextColor(WHITE);
          tft.setCursor(135,110);
          tft.print(MEP); 
          tft.setCursor(200,110);
          tft.print("cmH20");
          
          
          //print de les tres mesures utilitzades per calcular el MEP
          tft.setTextSize(1);
          tft.setCursor(250,150);
          tft.print(valor1);
          tft.setCursor(315,150);
          tft.print("cmH20");
          tft.setCursor(250,170);
          tft.print(valor2);
          tft.setCursor(315,170);
          tft.print("cmH20");
          tft.setCursor(250,190);
          tft.print(valor3);
          tft.setCursor(315,190);
          tft.print("cmH20");

          
          Stop();
          buttonEnabled2=false; //eliminem la opció next measurement
          buttonEnabled3 = true; //l'opció end test segueix present
          
        }
        else{
          Serial.println("Fes una altra mesura");
          Serial.println(z);
          variable=1; // si no hi ha tres valors q discrepen menys del 10% surt opció next measurement
    }
      }
     }
     
      if (variable==1){
      tft.fillScreen(BLACK);
      bat=0;
      NextMeasure();
      Stop();
      
      buttonEnabled2=true;
      buttonEnabled3 = true;
      tft.setTextColor(WHITE);
      tft.setTextSize(2);

      //Dibuix pressions a la pantalla next measurement cas acceptar MEP
  altura=50;
  for (int i=1; i <=contacceptar; i=i+1){ 
      tft.setCursor(220,altura); 
      tft.print(F("P="));
      tft.setCursor(330,altura);
      tft.print(F("cmH2O")); 
      tft.setCursor(245,altura);
      tft.print(pressions[i]);
      tft.setCursor(200,altura);
      tft.print(i);
      altura=altura+20;
      }
      }

      //això serveix per anar mirant com evoluciona el vector mitjanes en funció de l'obtenció de valors
      Serial.println ("valors");
      for(int i = 0; i < sizeof(mitjanes)/sizeof(float); i++)
      {
      Serial.println(mitjanes[i]);
      }
    }
    
    if(px>310 && px<390 && py>50 && py<80 && buttonEnabled9){ //cas de rebutjar: no guardarem a sd
      buttonEnabled8 = false;
      buttonEnabled9 = false;
      //nosdprint
      Serial.print("Reject");
      tft.fillScreen(BLACK);
      bat=0;
      NextMeasure();
      Stop();
       
      buttonEnabled2=true;
      buttonEnabled3 = true;
      tft.setTextColor(WHITE);
      tft.setTextSize(2);
       //Dibuix pressions a la pantalla next measurement cas rebutjar MEP
  altura=50;
  for (int i=1; i <=contacceptar; i=i+1){ 
      tft.setCursor(220,altura); 
      tft.print(F("P="));
      tft.setCursor(330,altura);
      tft.print(F("cmH2O")); 
      tft.setCursor(245,altura);
      tft.print(pressions[i]);
      tft.setCursor(200,altura);
      tft.print(i);
      altura=altura+20;
      }
  
    }   
   }
   

   //mateix que anterior pel cas del MIP
   if (next==2){ 
    contador=0, a=0, indexMAX=3, indexMIN=0, b=0, dibuix=0;
    counter=0; pressions[0]=0;
    oldValueMIP=0, oldValueMEP=0, Value,maxim=0, minim=110; 

    if(px>220 && px<300 && py>200 && py<230 && buttonEnabled6){ //cas d'acceptar: guardarem a sd  
      buttonEnabled6 = false;
      buttonEnabled7=false;

      
      contacceptar++;
      mitjanes[contacceptar] = mitja;
      pressions[contacceptar]=mitja;

      for(byte x = 0; x < sizeof(mitjanes)/sizeof(float); x++)
        for(byte y = 0; y < sizeof(mitjanes)/sizeof(float)-1; y++)
          if(mitjanes[y] > mitjanes[y+1]  ) {
          holder = mitjanes[y+1];
          mitjanes[y+1] = mitjanes[y];
          mitjanes[y] = holder;
     }

     Serial.println("contador acceptar");
     Serial.println(contacceptar);
     if (contacceptar<3){
      variable=1;
     }
     if (contacceptar>2){ //////////////////////////////////////////////////////////////////////////////////////////////////////////
      for (int z=1; z <contacceptar-1; z++){
        float xvalue=mitjanes[z];
        Serial.print("xvalue");
        Serial.println(xvalue);
        Serial.println(xvalue-0.1*xvalue);
        Serial.println(xvalue+0.1*xvalue);
        if ((xvalue-0.1*xvalue)>mitjanes[z-1] && mitjanes[z-1]>(xvalue+0.1*xvalue)&& (xvalue-0.1*xvalue)>mitjanes[z+1] && mitjanes[z+1] >(xvalue+0.1*xvalue)){
          float valor1=mitjanes[z-1];
          float valor2=xvalue;
          float valor3=mitjanes[z+1];
          float MIP=valor3;
          Serial.println("MIP final");
          Serial.println(MIP);
          z=contacceptar-1;
          variable=2;
          MIPvalue();
          tft.setTextColor(WHITE);
          tft.setCursor(130,110);
          tft.print(MIP); //desconnectar boto next measure i tornar a connectar quan toqui
          tft.setCursor(210,110);
          tft.print("cmH20");

          //print de les tres mesures utilitzades per calcular el MIP
          tft.setTextSize(1);
          tft.setCursor(250,150);
          tft.print(valor1);
          tft.setCursor(315,150);
          tft.print("cmH20");
          tft.setCursor(250,170);
          tft.print(valor2);
          tft.setCursor(315,170);
          tft.print("cmH20");
          tft.setCursor(250,190);
          tft.print(valor3);
          tft.setCursor(315,190);
          tft.print("cmH20");




          
          Stop();
          buttonEnabled2=false;
          buttonEnabled3 = true;
          
        }
        else{
          Serial.println("Fes una altra mesura");
          Serial.println(z);
          variable=1;
    }
      }
     }
     
      if (variable==1){
      tft.fillScreen(BLACK);
      bat=0;
      NextMeasure();
      Stop();
        /////////////////////////////
      
      buttonEnabled2=true;
      buttonEnabled3 = true;
      tft.setTextColor(WHITE);
      tft.setTextSize(2);


            //Dibuix pressions a la pantalla next measurement cas acceptar MIP
  altura=50;
  for (int i=1; i <=contacceptar; i=i+1){ 
      tft.setCursor(220,altura); 
      tft.print(F("P="));
      tft.setCursor(330,altura);
      tft.print(F("cmH2O")); 
      tft.setCursor(245,altura);
      tft.print(pressions[i]);
      tft.setCursor(200,altura);
      tft.print(i);
      altura=altura+20;
      }
      }

      Serial.println ("valors");
      for(int i = 0; i < sizeof(mitjanes)/sizeof(float); i++)
      {
      Serial.println(mitjanes[i]);
      }
    }
    
    if(px>310 && px<390 && py>200 && py<230 && buttonEnabled7){ //cas de rebutjar: no guardarem a sd
      buttonEnabled6 = false;
      buttonEnabled7 = false;
      //nosdprint
      Serial.print("Reject");
      tft.fillScreen(BLACK);
      bat=0;
      NextMeasure();
      Stop();
        /////////////////////////////
      
      buttonEnabled2=true;
      buttonEnabled3 = true;
      tft.setTextColor(WHITE);
      tft.setTextSize(2);

     //Dibuix pressions a la pantalla next measurement cas rebutjar MIP
  altura=50;
  for (int i=1; i <=contacceptar; i=i+1){ 
      tft.setCursor(220,altura); 
      tft.print(F("P="));
      tft.setCursor(330,altura);
      tft.print(F("cmH2O")); 
      tft.setCursor(245,altura);
      tft.print(pressions[i]);
      tft.setCursor(200,altura);
      tft.print(i);
      altura=altura+20;
      }
   
    }   
   }
   
   //Si piquem al botó Next measure fem una altra mesura (adequat a la prova corresponent)
    if(px>30 && px<140 && py>85 && py<135 && buttonEnabled2){ 
      buttonEnabled2 = false;
      buttonEnabled3=false;    
      if (tipo==1){ //si feiem MEP seguirem amb MEP
        previousMillis=0; 
        bat=1;
        drawGraphMEP();
        t=1;
      }
      if (tipo==2){ //si feiem MIP seguirem amb MIP
        previousMillis=0; 
        bat=1;
        drawGraphMIP();
        t=2;
      }
      tipo=0;   
   }
   
   //Si piquem al botó End measurement, ens porta a següent pantalla: opcions canvi test i apagar
     if(px>30 && px<140 && py>160 && py<210 && buttonEnabled3){ 
      buttonEnabled3 = false;
      buttonEnabled2=false;
      tft.fillScreen(BLACK);
      bat=0;
      ChangeTest();
      Off();
        /////////////////////////////
     
      buttonEnabled10 = true;
      buttonEnabled11=true;
     }
     
     //cas en que canviem de test
     if(px>270 && px<350 && py>70 && py<100 && buttonEnabled10){ 
      contador=0, a=0, indexMAX=3, indexMIN=0, b=0, dibuix=0, contacceptar=0;
      counter=0;
      //memset(mitjanes, 0, sizeof(mitjanes)); //clear array
       for(int i = 0; i < sizeof(mitjanes)/sizeof(float); i++){
        mitjanes[i]=200;
      }
      oldValueMIP=0, oldValueMEP=0, Value,maxim=0, minim=110, FileToOpen=1; //inicialitzem nom fitxer
      previousMillis=0; 
      buttonEnabled10 = false;
      buttonEnabled11=false;
      if (tipo==1){ //si feiem MEP, ara farem MIP (invers al q hi ha més amunt)
        bat=1;
        drawGraphMIP();
        t=2;
      }
      if (tipo==2){ //si feiem MIP, ara farem MEP 
        bat=1;
        drawGraphMEP();
        t=1;
      }
      tipo=0;
     }

     //Botó d'apagar porta a pantalla final (gràcies)
     if(px>270 && px<350 && py>160 && py<190 && buttonEnabled11){
      buttonEnabled10 = false;
      buttonEnabled11=false;
      tft.fillScreen(BLACK);
      bat=0;
      FinalScreen();
       
     }    
  }

  //Gravació de les dades analògiques MEP
   if (t==1){

      if ((millis() - previousMillis) >= interval && contador<350) { // check if "interval" time has passed (1000/70 milliseconds (70*5)--> 70 Hz= 70 punts cada segon (1000 milisegons), max 350 punts a pintar)
        previousMillis = millis();
        int sensorValue(analogRead(sensorPin)); // read the value from the sensor (punts de 0 a 1023)

        float voltage=0.5+sensorValue*4.00/1023;  //valors de voltatge de 0.5 a 4.5V
        float pressure=(voltage-0.1*5)*326.0/(0.8*5)-163.0; //valors de pressió reals a partir de l'eq del sensor
        // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
    
        tft.drawLine(counter+28,220-oldValueMEP,counter+29,220-pressure,WHITE);
        oldValueMEP=pressure;
        myArray[contador] = pressure;

        if (maxim<myArray[contador]){
        maxim=myArray[contador];
        indexMAX=contador; //anem actualitzant el valor màxim cada cop per trobar-lo
        }
        counter=counter+1; //canvi
        contador=contador+1;
        if (contador==350){ //canvi
        a=1;
        } 
        }
        if (a==1){
            tipo=1;
            buttonEnabled8 = true;
            buttonEnabled9=true;
            t=0;
            suma=myArray[indexMAX];
            for (int i=indexMAX +1; i<indexMAX+70; i++){ //mitja d'un segon (70 punts) a partir de max
              suma=suma+myArray[i];
            }
            mitja=suma/70;
            
            tft.setTextColor(WHITE);
            tft.setTextSize(2);
            tft.setCursor(220,20);
            tft.print(F("P="));
            tft.setCursor(330,20);
            tft.print(F("cmH2O")); 
            tft.setCursor(245,20);
            tft.print(mitja);
            AcceptMEP();
            RejectMEP();
            next=1;
            a=2;
           }
} 

 if (t==2){ //MIP

      if ((millis() - previousMillis) >= interval && contador<350) { //canvi contador
        previousMillis = millis();
        // read the value from the sensor:
        int sensorValue(analogRead(sensorPin));

        float voltage=0.5+sensorValue*4.00/1023; //valors de voltatge 0.5-4.5V
        float pressure=(voltage-0.1*5)*326.0/(0.8*5)-163.0; //valors de pressió reals, quan estiguin bé els eixos farem servir aquests
        //Value = (-150+sensorValue*(150.0 /511.5)); //valors aniran de 0-150 pq valor eix y //chapuza 511.5 pq no hi arriba
        tft.drawLine(counter+28,200-(180+oldValueMIP),counter+29,200-(180+pressure),WHITE); //canvi x2
        oldValueMIP=pressure;
        myArray[contador] = pressure;

        if (minim>myArray[contador]){
        minim=myArray[contador];
        indexMIN=contador;
        }
        counter=counter+1; //canvi
        contador=contador+1;
        if (contador==350){ //canvi
        a=1;
        }
       
        }
        if (a==1){
            tipo=2;
            buttonEnabled6 = true;
            buttonEnabled7=true;
            t=0;
            suma=myArray[indexMIN];
            for (int i=indexMIN +1; i<indexMIN+70; i++){
              suma=suma+myArray[i];
            }
            mitja=suma/70;
            tft.setTextColor(WHITE);
            tft.setTextSize(2);
            tft.setCursor(220,170);
            tft.print(F("P="));
            tft.setCursor(310,170);
            tft.print(F("  cmH2O"));
            tft.setCursor(245,170);
            tft.print(mitja); 
            AcceptMIP();
            RejectMIP();
            next=2;
            a=2;
           }
     
  }
  
}

/////////////////////////////////////////////FUNCIO ESTAT BATERIA//////////////////////////////////////////////////

//Ho he posat en una funció per alleugerir codi

float BatteryState(int valor_bat){   
  int battery_bits=analogRead(valor_bat);
  //int battery_voltage_value=map(battery_bits,0,1023,0,950); ////////////////////////////////////////////////////////////////////
  float battery_voltage_value=battery_bits*4.8/1023;
  //float battery_voltage=battery_voltage_value/100;
  float battery_voltage=battery_voltage_value*2.2;
  tft.setCursor(260,15);

  if (battery_voltage<=7.50){ 
    //tft.print(battery_voltage);
    Charge();
  }
  
  if (battery_voltage>7.50 && battery_voltage<=8.00){ 
    //tft.print(battery_voltage);
    LowBattery();
    
  }
  if (battery_voltage>8.00 && battery_voltage<=8.50){ 
    //tft.print(battery_voltage);
    OrangeBattery();
    
  }
  if (battery_voltage>8.50 && battery_voltage<=9.50){ 
    //tft.print(battery_voltage);
    GreenBattery();
  }
  if (battery_voltage>9.50){ 
    //tft.print(battery_voltage);
    AllBattery();
  } 
}
