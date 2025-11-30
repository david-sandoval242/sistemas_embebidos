#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHTesp.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int pinLM35 = 6
const int DHT_PIN = 42;
DHTesp dhtSensor;

const int pinSDA = 40;
const int pinSCL = 39;

unsigned long ultimaActualizacion;
const unsigned long intervalo = 5000;

float valorLecturaLM35;
float temperaturaLM35;

void setup() {
  //Inicializar la comunicación por puerto serial
  Serial.begin(115200);
  //Inicializar la comunicación I2C
  Wire.begin(pinSDA, pinSCL);

  //Inicializar LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Est. Tiempo");

  Serial.println("Estación de tiempo");
  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);

  ultimaActualizacion = 0;

  valorLecturaLM35 = 0;
  temperaturaLM35 = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  if ((millis() - ultimaActualizacion) >= intervalo)
  {
    ultimaActualizacion = millis();
    TempAndHumidity  data = dhtSensor.getTempAndHumidity();

    valorLecturaLM35 = analogRead(pinLM35);
    temperaturaLM35 = (valorLecturaLM35 * 3.3 * 100.0) / 4095.0;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("T: " + String(data.temperature, 2) + " C");

    lcd.setCursor(0, 1);
    lcd.print("H: " + String(data.humidity, 1) + " %");


    Serial.println("Temperatura: " + String(data.temperature, 2) + "°C");
    Serial.println("Humedad: " + String(data.humidity, 1) + "%");

    Serial.println("Temperatura LM35: " + String(temperaturaLM35, 2)+ " ºC");
    Serial.println("---");  
  }
}
