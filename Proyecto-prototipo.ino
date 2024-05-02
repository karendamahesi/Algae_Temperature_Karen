// Karen Cruz 2024

//Incluir la libreria de LiquidCrystal
#include <LiquidCrystal.h>
#include <Wire.h>

//Crear el objeto lcd  dirección  32 (depende del modelo) y 16 columnas x 2 filas
LiquidCrystal lcd(32,16,2);  //

void setup() {
  // put your setup code here, to run once:
//Para iniciar la libreria y comunicación I2C con Arduino
  	Wire.begin(); 
  
	// Inicializar el LCD
	lcd.init();
  
	//Encender la luz de fondo.
	lcd.backlight();

  // Tipo de dato y variable
  float Termistor; 
  }

void loop() {
  // put your main code here, to run repeatedly:
// Nuevo mapeo de temperatura
Termistor=map(Temperatura, 0, 1023, -80, 150);

lcd.clear();
lcd.setCursor (0, 0);
lcd.print("Temperatura");
lcd.setCursor(0, 1);
lcd.print(Termistor);
delay()


}
