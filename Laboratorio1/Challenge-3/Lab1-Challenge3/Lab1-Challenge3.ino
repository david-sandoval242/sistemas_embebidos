#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHTesp.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int DHT_PIN = 42;
DHTesp dhtSensor;

const int pinSDA = 40;
const int pinSCL = 39;

unsigned long ultimaActualizacion;
const unsigned long intervalo = 5000;

uint32_t valorLecturaMCP9700;
uint32_t voltajeMCP9700;
float temperaturaMCP9700;

uint32_t valorLecturaLM35;
uint32_t voltajeLM35;
float temperaturaLM35;

esp_adc_cal_characteristics_t adc_chars;

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

  // Configurar resolución del ADC1 a 12 bits
  adc1_config_width(ADC_WIDTH_BIT_12);

  // Configurar el canal (ejemplo: canal ADC1_CH1 = GPIO1)
  adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11); 

  // Configurar el canal (ejemplo: canal ADC1_CH1 = GPIO2)
  adc1_config_channel_atten(ADC1_CHANNEL_1, ADC_ATTEN_DB_11);

  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
  ultimaActualizacion = 0;

  valorLecturaMCP9700 = 0;
  voltajeMCP9700 = 0;
  temperaturaMCP9700 = 0;

  valorLecturaLM35 = 0;
  voltajeLM35 = 0;
  temperaturaLM35 = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  if ((millis() - ultimaActualizacion) >= intervalo)
  {
    ultimaActualizacion = millis();
    TempAndHumidity  data = dhtSensor.getTempAndHumidity();

    //Obtener lectura cruda del ADC del sensor LM35
    valorLecturaLM35 = adc1_get_raw(ADC1_CHANNEL_0);

    // Convertir usando la calibración
    voltajeLM35 = esp_adc_cal_raw_to_voltage(valorLecturaLM35, &adc_chars);

    // --- Cálculo de la temperatura con LM35 ---
    // LM35 entrega 10 mV por cada °C
    temperaturaLM35 = voltajeLM35 / 10.0;

    //Obtener lectura cruda del ADC del sensor MCP9700
    valorLecturaMCP9700 = adc1_get_raw(ADC1_CHANNEL_1);

    // Convertir a voltaje calibrado
    voltajeMCP9700 = esp_adc_cal_raw_to_voltage(valorLecturaMCP9700, &adc_chars);

    // Calcular temperatura MCP9700
    temperaturaMCP9700 = (voltajeMCP9700 - 500.0) / 10.0;    

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("T: " + String(data.temperature, 2) + " C");

    lcd.setCursor(0, 1);
    lcd.print("H: " + String(data.humidity, 1) + " %");


    Serial.println("Temperatura DHT22: " + String(data.temperature, 2) + "°C");
    Serial.println("Humedad DHT22: " + String(data.humidity, 1) + "%");

    Serial.println("Temperatura LM35: " + String(temperaturaLM35, 2)+ " ºC");

    Serial.println("Temperatura MCP9700: " + String(temperaturaMCP9700, 2)+ " ºC");
    Serial.println("---");     
  }
}
