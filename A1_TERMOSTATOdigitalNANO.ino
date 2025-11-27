// logica del nuevo termostato:
//Si baja la temperatura seteada, entonces:
//                                        1_Abrir Electrovalvula
//                                        2_Bobina CHISPA hasta detectar llama (Tiempo MAX 5seg - 4 intentos)
//Si llega a temperatura seteada, entonces:
//                                        1_Cerrar Electrovalvula
//                                                                               



#include <PCD8544.h>
#define PIN_DetectorFLAMA 4
float tempC=0;
int tempPin=0;//el pin de arduino que obtiene voltajes

static const byte sensorPin = 0;
static const byte RELAY_ELECTROVALVULA = 7;
static const byte Relay_CHISPA = 2;
static const byte Relay_BLOQUEO = 3;




const int  SetTempUp          = 6;        // pin 6 de Arduino con boton de incremento de temperatura
const int  SetTempDown        = 5;        // pin 7 de Arduino con boton de decremento de temperatura
int TemButtonCounter          = 19;       // contador de veces que se incrementa o decrementa el ajuste de temperatura (valor inicial es de 19 grados)
bool TempButtonUpState        = 0;  // estado actual del boton de incremento de temperatura
bool TempButtonDownState      = 0;  // estado actual del boton de decremento de temperatura
bool lastTempButtonUpState    = 0;  // ultimo estado (si se incremento o decremento la temperatura)
bool lastTempButtonDownState  = 0;

// The dimensions of the LCD (in pixels)...
static const byte LCD_WIDTH = 84;
static const byte LCD_HEIGHT = 48;

// The number of lines for the temperature chart...
static const byte CHART_HEIGHT = 5;

// A custom "degrees" symbol...
static const byte DEGREES_CHAR = 1;
static const byte degrees_glyph[] = { 0x00, 0x07, 0x05, 0x07, 0x00 };

// A bitmap graphic (10x2) of a thermometer...
static const byte THERMO_WIDTH = 10;
static const byte THERMO_HEIGHT = 2;
static const byte thermometer[] = { 0x00, 0x00, 0x48, 0xfe, 0x01, 0xfe, 0x00, 0x02, 0x05, 0x02,
                                    0x00, 0x00, 0x62, 0xff, 0xfe, 0xff, 0x60, 0x00, 0x00, 0x00};
static PCD8544 lcd;


void setup() {
  lcd.begin(LCD_WIDTH, LCD_HEIGHT);
  
  // Register the custom symbol...
  lcd.createChar(DEGREES_CHAR, degrees_glyph);
  
  pinMode(RELAY_ELECTROVALVULA, OUTPUT);
  pinMode(SetTempUp, INPUT);
  pinMode(SetTempDown, INPUT);
  pinMode(PIN_DetectorFLAMA,INPUT_PULLUP);
  
  // The internal 1.1V reference provides for better
  // resolution from the LM35, and is also more stable
  // when powered from either a battery or USB...
  analogReference(INTERNAL);

  lcd.setCursor(12, 1);
  lcd.print("TERMOSTATO");
 lcd.setCursor(21, 2);
  lcd.print("DIGITAL");


  // Draw the thermometer bitmap at the bottom left corner...
  lcd.setCursor(0, LCD_HEIGHT/8 - THERMO_HEIGHT);
  lcd.drawBitmap(thermometer, THERMO_WIDTH, THERMO_HEIGHT);
    lcd.setCursor(9, 0);
   lcd.print("-----------");
     lcd.setCursor(9, 9);
  lcd.print("-----------");
   lcd.setCursor(30, 10);
  lcd.print("ETI");
    lcd.setCursor(12, 11);
  lcd.print("PATAGONIA"); 

  Serial.begin(9600);

  Serial.println("-------------------------");
  Serial.println("      ETI-Patagonia      ");
  Serial.println("Educacion Tecnica Informal");
  Serial.println("prof.martintorres@educ.ar");
  Serial.println("-------------------------");
delay(2000); 
}


void loop() {
  // Start beyond the edge of the screen...
  //static byte xChart = LCD_WIDTH;

  
// Lectura de la temperatura en grados celsius
  float temp = (1.1 * analogRead(sensorPin) * 100.0) / 1024.0;

// Imprimimos la temperatura
  int FLAMA = digitalRead(PIN_DetectorFLAMA);
  
  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.print(temp, 1);
  lcd.print(" \001C ");
  lcd.setCursor(0, 2);
  lcd.print("SetT: ");
  lcd.print(TemButtonCounter, 1);
  lcd.print(" \001C ");

  // Draw the thermometer bitmap at the bottom left corner...
  lcd.setCursor(0, LCD_HEIGHT/8 - THERMO_HEIGHT);
  lcd.drawBitmap(thermometer, THERMO_WIDTH, THERMO_HEIGHT);
    lcd.setCursor(9, 0);
   lcd.print("-----------");
   if(FLAMA==HIGH)
   {
     lcd.setCursor(9, 9);
  lcd.print("-----------");
    }
    else
     {
     lcd.setCursor(9, 9);
  lcd.print("-Sin-LLAMA-");
    }
  
   lcd.setCursor(30, 10);
  lcd.print("ETI");
    lcd.setCursor(12, 11);
  lcd.print("PATAGONIA"); 
  
  
//lectura de pulsadores para el set de temperatura
TempButtonUpState = digitalRead(SetTempUp);     // lectura del boton de incremento de temperatura
  if (TempButtonUpState != lastTempButtonUpState) 
   { // comparar TempButtonState con su estado previo
      if (TempButtonUpState == 1)    
      {              // si el ultimo estado cambio, incrementar el contador
          TemButtonCounter ++;
          delay(50);
      }
  }
   // guardar el estado actual como ultimo estado,para la proxima vez que se ejecute el lazo 

   lastTempButtonUpState = TempButtonUpState;
   
  // lectura del boton de decremento de temperatura
   
  TempButtonDownState = digitalRead(SetTempDown);
  if (TempButtonDownState != lastTempButtonDownState) 
  {    // compara el estado del boton (incremento o decremento) con el ultimo estado
      if (TempButtonDownState == 1) 
      { // si el ultimo estado cambio, decrementar el contador
          TemButtonCounter--;
          delay(50);
      }
  }
  // guardar el estado actual como ultimo estado,
  // para la proxima vez que se ejecute el lazo
  lastTempButtonDownState = TempButtonDownState; 
   
 if (temp < TemButtonCounter) {
      digitalWrite(RELAY_ELECTROVALVULA, HIGH);
  }
  else  {
      digitalWrite(RELAY_ELECTROVALVULA, LOW);
  }


/////////////////////COMUNICACION///////////////////////////////////////
Serial.begin(9600);

  Serial.println("Temp: ");
  Serial.print(temp, 1);
  Serial.println(" \001C ");
  Serial.println("SetT: ");
  Serial.print(TemButtonCounter, 1);
  Serial.println(" \001C ");
 if(FLAMA==HIGH)
   {
  Serial.println("-----------");
    }
    else
     {
  
  Serial.println("-Sin-LLAMA-");
  Serial.println("-----------");
    }

  delay(2000); 


/////////////////////GRAFICO///////////////////////////////////////////
// Wrap the chart's current position...
//  if (xChart >= LCD_WIDTH) {
//    xChart = THERMO_WIDTH + 2;
//  }
// Update the temperature chart...  
//  lcd.setCursor(xChart, 1);
//  lcd.drawColumn(CHART_HEIGHT, map(temp, 0, 45, 0, CHART_HEIGHT*8));  // ...clipped to the 0-45C range.
//  lcd.drawColumn(CHART_HEIGHT, 0);         // ...with a clear marker to see the current chart position.
//  xChart++;
//  digitalWrite(RELAY_ELECTROVALVULA, LOW);  
//  delay(500);
////////////////////////////////////////

}















  
  
