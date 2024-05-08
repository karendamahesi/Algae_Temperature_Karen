// Karen Cruz 2024

//Include LiquidCrystal libreries
#include <LiquidCrystal.h>
#include <Wire.h>

//Crear el objeto lcd  dirección  32 (depende del modelo) y 16 columnas x 2 filas
LiquidCrystal lcd(32,16,2);  //

// Pin definition for LCD screen
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Definition of pins and variables for the temperature sensor and the water pump
const int sensorPin = A0; // Analog pin for temperature sensor
const int bombaPin = 9;    // Digital pin for water pump
int Temperatura;           // Any data type for measured temperature variable

// Definition of the states
enum Estado { 
  SENSOR_ACTIVADO
  ANALISIS_TEMPERATURA
  REMAPEO_VALORES
  ACTIVACION
  DESACTIVACION
  ENVIO_DATOS_NUBE}; 
Estado estado=SENSOR_ACTIVADO; //Initial state


void setup() {
	// LCD screen initialization
	lcd.begin(16, 2);
  
	// Water pump pin initialization
  pinMode(bombaPin, OUTPUT);
  }


void readSensor() {
  Serial.printIn ("Reading sensor");
  Termistor=map(Temperatura, 0, 1023, -80, 150); //nuevo mapeo de temperatura
}

void loop() {

lcd.clear();
lcd.setCursor (0, 0);
lcd.print("Temperatura");
lcd.setCursor(0, 1);
lcd.print(Termistor);
delay()


}
