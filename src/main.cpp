#define USE_AVR_DEBUGGER
 
#include <Arduino.h>
 
#include <Ticker.h>
 
#if defined USE_AVR_DEBUGGER
#include "avr_debugger.h"
#include "avr8-stub.h"
#include "app_api.h"
#endif
 
#define USE_SERIAL Serial2

#define REDLIGHT_PIN 52
#define YELLOWLIGHT_PIN 51
#define GREENLIGHT_PIN 50
 
#define Display_Attached
//#define ESP8266

#define StopLineCount 30

#ifdef Display_Attached
  #include <SPI.h>
  #include <Wire.h>
  #include <Adafruit_GFX.h>
  #include <Adafruit_SSD1306.h>

  #define OLED_RESET LED_BUILTIN  //4
  Adafruit_SSD1306 display(OLED_RESET);

  #if (SSD1306_LCDHEIGHT != 64)
    #error("Height incorrect, please fix Adafruit_SSD1306.h!");
  #endif
#endif



const unsigned long PeriodBeforeShift = 3000;

unsigned long StartMillis;
unsigned long CurrentMillis;
unsigned long FunctionPointerTableCounter = 0;

static volatile bool Interrupt_Occured = false;
static volatile uint32_t Interrupt_Counter = 0;
//=======================================================================
void changeState(void) {



    digitalWrite(LED, !(digitalRead(LED)));  //Invert Current State of LED
    Interrupt_Occured = true;
    Interrupt_Counter++;
}

enum light_states_e {
    RED_LIGHT = 0,
    REDYELLOW_LIGHT = 1,
    GREEN_LIGHT = 2,
    YELLOW_LIGHT = 3,
};

typedef struct traffic_light_s {
    int redlight_period = 3;
    int redyellowlight_period = 2;
    int greenlight_period = 5;
    int yellowlight_period = 1;

    light_states_e current_state;
    int time_in_state;
} traffic_light_t;

traffic_light_t traffic_light = {
    .redlight_period = 3,
    .redyellowlight_period = 2,
    .greenlight_period = 5,
    .yellowlight_period = 1,
};

void light_state_machine(void) {

    traffic_light.time_in_state++;
    
    switch (traffic_light.current_state) {
        case RED_LIGHT:
            
            digitalWrite(REDLIGHT_PIN, 1);
            digitalWrite(YELLOWLIGHT_PIN, 0);
            digitalWrite(GREENLIGHT_PIN, 0);

            if (traffic_light.time_in_state > traffic_light.redlight_period) {
                traffic_light.time_in_state = 0;
                traffic_light.current_state = REDYELLOW_LIGHT;
            }

            break;
        case REDYELLOW_LIGHT:
            
            digitalWrite(REDLIGHT_PIN, 1);
            digitalWrite(YELLOWLIGHT_PIN, 1);
            digitalWrite(GREENLIGHT_PIN, 0);

            if (traffic_light.time_in_state > traffic_light.redyellowlight_period) {
                traffic_light.time_in_state = 0;
                traffic_light.current_state = GREEN_LIGHT;
            }

            break;
        case GREEN_LIGHT:
            
            digitalWrite(REDLIGHT_PIN, 0);
            digitalWrite(YELLOWLIGHT_PIN, 0);
            digitalWrite(GREENLIGHT_PIN, 1);

            if (traffic_light.time_in_state > traffic_light.greenlight_period) {
                traffic_light.time_in_state = 0;
                traffic_light.current_state = YELLOW_LIGHT;
            }

            break;
        
        default:
        case YELLOW_LIGHT:
            
            digitalWrite(REDLIGHT_PIN, 0);
            digitalWrite(YELLOWLIGHT_PIN, 1);
            digitalWrite(GREENLIGHT_PIN, 0);

            if (traffic_light.time_in_state > traffic_light.yellowlight_period) {
                traffic_light.time_in_state = 0;
                traffic_light.current_state = RED_LIGHT;
            }

            break;
        }
}

// Ticker Blinker(changeState, 1000);
Ticker LightTicker(light_state_machine, 1000);
 
void ClearScreen(void)
{
  display.clearDisplay();
  display.display();
  
  for (int LineCounter = 0; LineCounter < StopLineCount; LineCounter++)
  {
    printf("\n");
  }
  printf("ClearDisplay\n");
}

void WriteText(void)
{
  #ifdef Display_Attached
  display.clearDisplay();
  //display.setCursor(10,10);
  display.setTextSize(2);
  display.setCursor(0,16); 
  display.println("Embedded Styrer !!!");
  display.display();
  #endif
  printf("WriteText\n");     
}

void DraweLine(void)
{
  #ifdef Display_Attached
  display.clearDisplay();

  display.drawLine(0,0,display.width() - 1, display.height() - 1, WHITE);
  display.drawLine(display.width() - 1,0,0, display.height() - 1, WHITE);

  display.display();
  #endif
  printf("Drawline\n");
}

void DrawRectangle(void)
{
  #ifdef Display_Attached
  display.clearDisplay();

  display.drawRect(100, 10, 20, 20, WHITE);
  display.fillRect(10, 10, 45, 15, WHITE);
  display.drawRoundRect(60, 20, 35, 35, 8, WHITE);

  display.display();
  #endif
  printf("DrawRectangle\n");
}

void DrawCircle(void)
{
  #ifdef Display_Attached
  display.clearDisplay();
  display.drawCircle(60, 30, 30, WHITE);
  display.fillCircle(50, 20, 5, WHITE);
  display.fillCircle(70, 20, 5, WHITE);
  display.display();
  #endif
  printf("DrawCircle\n");
}

void DrawTriangle()
{
  #ifdef Display_Attached
  display.clearDisplay();
  display.drawTriangle(24, 1, 3, 55, 45, 55, WHITE);
  display.fillTriangle(104, 62, 125, 9, 83, 9, WHITE);
  display.display();
  #endif
  printf("DrawTriangle\n");
}

void WriteTextTeam()
{
  #ifdef Display_Attached
  display.clearDisplay();
  //display.setCursor(10,10);
  display.setTextSize(2);
  display.setCursor(0,16); 
  display.println("h2pd041121 Styrer !!!");
  display.display();
  #endif
  printf("WriteTexth3pd041121\n");
}

// Erklæring af en prototype for en funktion pointer. Funktions pointeren kan bruges til funktioner, der okke tager imod nogle parametre og ikke returnerer 
// en parameter. 
// Skal man drage en parallel til en funktions pointer i C indenfor C# verdenen, vil der være tale om en Delegate. 
typedef void (*DisplayFunctionPointer)();

// En tabel af funktions pointere af typen beskerevet lige herover. 
DisplayFunctionPointer DisplayFunctionPointerTable [] = 
{
  &ClearScreen,
  &WriteText,
  &DraweLine,
  &DrawRectangle,
  &DrawCircle,
  &DrawTriangle,
  &WriteTextTeam
};

void setup(void) {
    USE_SERIAL.begin(9600);

    #ifdef Display_Attached
   display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
   display.clearDisplay();
   display.setTextColor(WHITE);
   #endif
   
   StartMillis = millis();  //initial start time

   (*DisplayFunctionPointerTable[0])();  // Start med den første funktion i vores Funktion Pointer Tabel så vi ikke starter med et tomt Display, hvis vi har et Display tilkoblet !!!

    // put your setup code here, to run once:
    pinMode(LED, OUTPUT);
#if defined USE_AVR_DEBUGGER
    debug_init();
#endif
    delay(3000);
    USE_SERIAL.begin(115200);
 
    LightTicker.start();

    // initialize digital pin LED_BUILTIN as an output.
    pinMode(50, OUTPUT);
    pinMode(51, OUTPUT);
    pinMode(52, OUTPUT);
}

bool isNumeric(const char* s, int len){
    while(*s && len--){
        if(*s < '0' || *s > '9')
            return false;
        ++s;
    }
    return true;
}

void handle_light_period(void) {
  int num_chars = USE_SERIAL.available();
  char buf[4] = {0};
  buf[4] = '\0';
  if(num_chars < 3) {
    return;
  }

  for (size_t i = 0; i < 3; i++) {
    buf[i] = USE_SERIAL.read();  // read one byte from serial buffer;
  }

  if (!isNumeric(&buf[1], 2)) {
    return;
  }

  switch (buf[0]) {
    case '0':
      traffic_light.redlight_period = atoi(&buf[1]);
      break;
    case '1':
      traffic_light.redyellowlight_period = atoi(&buf[1]);
      break;
    case '2':
      traffic_light.greenlight_period = atoi(&buf[1]);
      break;
    case '3':
      traffic_light.yellowlight_period = atoi(&buf[1]);
      break;
    
    default:
      break;
  }
}

void loop(void) {

    handle_light_period();

    CurrentMillis = millis();  //get the current time
  
    if (CurrentMillis - StartMillis >= PeriodBeforeShift) {  //test whether the period has elapsed
    
        FunctionPointerTableCounter = ++FunctionPointerTableCounter % (sizeof(DisplayFunctionPointerTable)/sizeof(DisplayFunctionPointerTable[0]));
        // Kald af den "udpegede" funktion sker ved brug af funktions pointeren herunder. Linjen herunder er faktisk "blot" et kald af en funktion.
        // Dette kan godt være lidt svært at se, hvis ikke man har anvendt syntaksen her før. Men det turde være åbenlys, hvilke fordele man som
        // programmør opnår ved at bruge funktions pointere og tabeller af samme. Ens kode bliver meget let at vedligeholde og udvide. 
        // Ønsker man at lave en ny funktion, der skal kaldes på linje med de allerede eksisterende funktioner, skal man blot implementere funktionen ét sted
        // i koden og så indsætte adressen på denne funktion i fuknktions pointer tabellen. 
        // Eneste "begrænsning" ved den nye funktion er, at denne ikke tager imod nogle parametre og heller ikke returnerer en parameter.
        // Dette ser vi nærmere på i næste program !!!
        (*DisplayFunctionPointerTable[FunctionPointerTableCounter])();
                
        StartMillis = CurrentMillis;
    }

    LightTicker.update();
    if (Interrupt_Occured) {
    
        Interrupt_Occured = false;
        USE_SERIAL.print("Interrupt Counter : ");
        USE_SERIAL.println(Interrupt_Counter);
    }
}