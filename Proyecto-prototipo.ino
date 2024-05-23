// Karen Cruz 2024

//Include libreries
#include <ArduinoIoTCloud.h>            // Librerias IoT
#include <Arduino_ConnectionHandler.h>
#include <thingProperties.h>
#include <WiFi.h>                       // Librerias conexion WiFi
#include <WiFiUdp.h>
#include <LiquidCrystal.h>              // Librerias pantalla lcd
#include <Wire.h>
#include <SD.h>                         // Librerias memoria SD
#include <SPI.h>
#include <NTPClient.h>                  // Librerias para obtener la hora por WiFi
#include <Time.h>
#include <TimeLib.h>
#include <Timezone.h>


// Pin definition for LCD screen
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


// Definition of pins and variables for the temperature sensor and the water pump
const int sensorPin = A0; // Analog pin for temperature sensor
const int bombaPin = 9;    // Digital pin for water pump
const int relePin = 8;     // Digital pin to control the relay
int Temperatura;           // Any data type for measured temperature variable
const int chipSelect = 10; // Pin para el chip select de la tarjeta SD


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


// Variables para Arduino IoT Cloud
float temperature;
String currentTime;


// Definición de tu red WiFi y credenciales de Arduino IoT Cloud
const char SSID[]     = COQUIS;    // Tu SSID
const char PASS[]     = espejocarretillaprincesa; // Tu contraseña de red
const char DEVICE_ID[]  = 630d9091-cb70-48b1-9f55-27ff4ab9f2ff;  // Tu Device ID de Arduino IoT Cloud
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
    Serial.println("Conectando a WiFi...");
  } 


// NTP Client para obtener la hora de internet
WiFiUDP ntpUDP;
const long utcOffsetInSeconds = -6 * 3600; // Offset de UTC para Ciudad de México (UTC-6)
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds, 60000); // Actualiza cada 60 segundos


unsigned long tiempoUltimaLectura = 0; // Variable para registrar el tiempo de la última lectura del sensor
const unsigned long intervaloLectura = 30 * 60 * 1000; // Intervalo de lectura: 30 minutos en milisegundos
unsigned long tiempoInicioIdle = 0; // Variable para registrar el inicio del estado IDLE
const unsigned long duracionIdle = 30 * 60 * 1000; // Duración del estado IDLE: 30 minutos en milisegundos


// Inicialización del cliente NTP
  timeClient.begin();


void setup() {
	// LCD screen initialization
	lcd.begin(16, 2);
  

	// Water pump and relay pin initialization
  pinMode(bombaPin, OUTPUT);
  pinMode(relePin, OUTPUT);
  
 
// Inicialización de la conexión WiFi y Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  ArduinoCloud.addProperty(temperature, READWRITE, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(currentTime, READWRITE, ON_CHANGE, NULL);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();


  // Inicialización de la tarjeta SD
  if (!SD.begin(chipSelect)) {
    Serial.println("Error inicializando la tarjeta SD!");
    return;
  }
  Serial.println("Tarjeta SD inicializada correctamente.");
  }


void loop() {
  // Definición de los estados de la queue
  enum QueueState { GET_INTERNET_TIME, READ_SENSOR };
  QueueState queue[2] = { GET_INTERNET_TIME, READ_SENSOR };
  int queueIndex = 0;

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

// Iniciar la cola de tareas
  queueIndex = 0;
  estado = READ_SENSOR;

// Procesar la cola de tareas
  processQueue();

// Update Arduino Cloud Status
  ArduinoCloud.update();
}


void processQueue() {
  switch (queue[queueIndex]) {
    case GET_INTERNET_TIME:
      getInternetTime();
      queueIndex++;
      break;
    case READ_SENSOR:
      readSensor();
      queueIndex++;
      break;
  }
}


void directing() {
    if (tiempoActual % 30000 == 0) {
    estado = READ_SENSOR;
  }
  // Obtener la hora actual para verificar si es medianoche
  timeClient.update();
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  int currentSecond = timeClient.getSeconds();
  //Cambiar al estado GET_INTERNET_TIME a media noche
  if (currentHour == 0 && currentMinute == 0 && currentSecond == 0) {
    estado = GET_INTERNET_TIME; 
  }
}


void get_internet_time(){
  // Obtener el tiempo actual
  time_t now = now();
  currentTime = String(hour(now)) + ":" + String(minute(now)) + ":" + String(second(now));
}


void read_sensor() {
  // Show message on LCD screen
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sensor activado");
  
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

 // Obtener el tiempo actual
  timeClient.update();
  currentTime = timeClient.getFormattedTime();

  // Update temperature value in Arduino Cloud
  ArduinoCloud.updateTemperature(temperatura);
  
  // Switch to next state after a while
  delay(3000); // Wait 3 seconds
  estado = EVALUAR_TEMP;
}


void evaluar_temp() {
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


void send_cloud(){
  // Update temperature value in Arduino Cloud
  ArduinoCloud.updateTemperature(temperatura);

  estado = STORE_SD
}


void store_sd(){
  // Guardar los datos en la tarjeta SD
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  if (dataFile) {
    dataFile.print("Hora: ");
    dataFile.print(currentTime);
    dataFile.print(", Temperatura: ");
    dataFile.println(temperature);
    dataFile.close();
    Serial.println("Datos guardados en la tarjeta SD.");
  } else {
    Serial.println("Error abriendo el archivo datalog.txt");
  }

  estado = IDLE
}


void idle() {
  // Mantener el estado IDLE durante 30 minutos
  if (millis() - tiempoInicioIdle >= duracionIdle) {
    // Cambiar al siguiente estado después del periodo de inactividad
    estado = SENSOR_ACTIVADO;
    tiempoInicioIdle = millis(); // Reiniciar el temporizador de inactividad
  }
}