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
  IDLE
  DIRECTING
  GET_INTERNET_TIME
  READ_SENSOR
  EVALUAR_TEMP
  ACTIVACION
  DESACTIVACION
  STORE_SD
  SEND_CLOUD
}; 

Estado estado=READ_SENSOR; //Initial state

unsigned long tiempoUltimaLectura = 0; // Variable to record the time of the last sensor reading
const unsigned long intervaloLectura = 30 * 60 * 1000; // Reading interval: 30 minutes in milliseconds

// Variables para Arduino IoT Cloud
float temperature;
String currentTime;

// Definición de tu red WiFi y credenciales de Arduino IoT Cloud
const char SSID[]     = "your_SSID";    // Tu SSID
const char PASS[]     = "your_PASSWORD"; // Tu contraseña de red
const char DEVICE_ID[]  = "your_DEVICE_ID";  // Tu Device ID de Arduino IoT Cloud
const char DEVICE_SECRET[] = "your_DEVICE_SECRET"; // Tu Device Secret de Arduino IoT Cloud

// Inicialización de la conexión WiFi y Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  ArduinoCloud.addProperty(temperature, READWRITE, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(currentTime, READWRITE, ON_CHANGE, NULL);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  // Conectar a WiFi
  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  } 
  

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

  // Dequeue (ir tomando, uno por uno, los estados del queue)

  // Call the function corresponding to the current state
  switch (estado) {
    case IDLE:
      idle();
      break;
    case DIRECTING:
      directing();
      break;
    case GET_INTERNET_TIME:
      get_internet_time();
      break;
    case READ_SENSOR:
      read_sensor();
      break;
    case EVALUAR_TEMP:
      evaluar_temp();
      break;
    case ACTIVACION:
      activacion();
      break;
    case DESACTIVACION:
      desactivacion();
      break;
    case STORE_SD:
      store_sd();
      break;
    case SEND_CLOUD:
      send_cloud();
      break;
  }

// Update Arduino Cloud Status
  ArduinoCloud.update();
}


void directing() {
  // Obtener el tiempo actual
  time_t now = now();
  currentTime = String(hour(now)) + ":" + String(minute(now)) + ":" + String(second(now));
  if (tiempoActual % 30000 == 0) {
    estado = READ_SENSOR;
  }
  if (currentHour == 0 && currentMinute == 0 && currentSecond == 0) {
    estado = GET_INTERNET_TIME; 
  }
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
  estado = EVALUAR_TEMP;
}

evaluar_temp() {
    // Check if the recorded temperature is >26°C
  if (temperatura > 26) {
    estado = ACTIVACION;
  }
  else {
    estado = DESACTIVACION;
  }
}

void activacion() {

    // Turn on the relay that activates the water pump
    digitalWrite(relePin, HIGH);

    estado = IDLE;


}


void desactivacion() {

    // Turn on the relay that activates the water pump
    digitalWrite(relePin, LOW);

    estado = IDLE;

}