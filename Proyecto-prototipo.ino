// Karen Cruz 2024

//Include LiquidCrystal libreries
#include <LiquidCrystal.h>
#include <Wire.h>
#include "thingProperties.h"


// Pin definition for LCD screen
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


// Definition of pins and variables for the temperature sensor and the water pump
const int sensorPin = A0; // Analog pin for temperature sensor
const int bombaPin = 9;    // Digital pin for water pump
const int relePin = 8;     // Digital pin to control the relay
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

unsigned long tiempoUltimaLectura = 0; // Variable to record the time of the last sensor reading
const unsigned long intervaloLectura = 30 * 60 * 1000; // Reading interval: 30 minutes in milliseconds


void setup() {
	// LCD screen initialization
	lcd.begin(16, 2);
  
	// Water pump and relay pin initialization
  pinMode(bombaPin, OUTPUT);
  pinMode(relePin, OUTPUT);
  
  // Arduino Cloud Initialization
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
  }


void loop() {
  // Check if the time has passed to take a new sensor reading
  unsigned long tiempoActual = millis();
  if (tiempoActual - tiempoUltimaLectura >= intervaloLectura) {
    // Update last reading time
    tiempoUltimaLectura = tiempoActual;
    
    // Change the state to SENSOR_ACTIVADO to start a new sensor reading
    estado = SENSOR_ACTIVADO;
  }
}


void loop() {
  // Call the function corresponding to the current state
  switch (estado) {
    case SENSOR_ACTIVADO:
      sensorActivado();
      break;
    case ANALISIS_TEMPERATURA:
      analisisTemperatura();
      break;
    case REMAPEO_VALORES:
      remapeoValores();
      envioDatosNube(); // Sending data to the cloud 
      break;
    case ACTIVACION:
      activacion();
      break;
    case DESACTIVACION:
      desactivacion();
      break;
    case ENVIO_DATOS_NUBE:
      envioDatosNube();
      break;
  }

// Update Arduino Cloud Status
  ArduinoCloud.update();
}


void sensorActivado() {
  // Show message on LCD screen
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sensor activado");
  
  // Switch to next state after a while
  delay(2000); // Wait 2 seconds
  estado = ANALISIS_TEMPERATURA;
}


void analisisTemperatura() {
    
  // Switch to next state after a while
  delay(2000); // Wait 2 seconds
  estado = REMAPEO_VALORES;
}


void remapeoValores() {
  // Temperature sensor reading
  int lecturaSensor = analogRead(sensorPin);
  
  // Remapping values to temperature
  float temperatura = map(lecturaSensor, 0, 1023, -80, 150);

// Show temperature on LCD screen
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temperatura:");
  lcd.setCursor(0, 1);
  lcd.print(temperatura);
  lcd.print(" C");
  
// Update temperature value in Arduino Cloud
  ArduinoCloud.updateTemperature(temperatura);

    // Switch to next state
  estado = ACTIVACION;
}


void activacion() {
  // Current temperature reading
  int lecturaSensor = analogRead(sensorPin);
  float temperatura = map(lecturaSensor, 0, 1023, -80, 150);

  // Check if the recorded temperature is >26°C
  if (temperatura > 26) {
    // Turn on the relay that activates the water pump
    digitalWrite(relePin, HIGH);
    // Turn on the water pump for 30 minutes
    digitalWrite(bombaPin, HIGH);
    delay(30 * 60 * 1000); // Wait 30 minutes
    // Turn off the water pump
    digitalWrite(bombaPin, LOW);
    // Turn off the relay
    digitalWrite(relePin, LOW);
  }

  // Switch to next state
  estado = DESACTIVACION;
}


void desactivacion() {
  // Switch to next state
  estado = ENVIO_DATOS_NUBE;
}