# 1. Challenge-2 (Simulation)

**Measuring Weight with a Load Cell, HX711, and LCD**\
Design and Implement an embedded system to read the weight (until 50kg) from a load cell connected through\
an HX711 amplifier, display the measured value in real-time on an LCD screen (16x2), and print it to the Serial\
Monitor.

## Metodología de diseño e implementación de sistemas embebidos
### 1. Análisis de requerimientos
#### Requerimientos funcionales

- FR-01: El sistema debe permitir realizar medidas de peso hasta 50Kg
- FR-02: El sistema debe mostrar el peso por medio de una pantalla LCD en unidades de Kg
- FR-03: El sistema debe mostrar la medida del peso en todo momento
- FR-04: El sistema debe mostrar la medida del peso por medio de la salida serial
- FR-05: El sistema debe contar con cuatro celdas de carga y usar el integrado HX711

#### Requerimientos no funcionales

- NFR-01: El sistema debe iniciar automaticamente al conectarse a la energía
- NFR-02: La medición del peso debe realizarse en todo momento
- NFR-03: Las 4 celdas de carga deben ubicarse en 4 puntos distribuidos uniformemente a lo largo del área de presión
- NFR-04: La pantalla LCD debe encender la luz de fondo para visualizar mejor la medida desplegada.

#### Restricciones

- CON-01: La pantalla LCD debe tener 16 columnas y 2 filas y comunicarse por medio del protocolo I2C
- CON-02: El módulo HX711 se debe conectar a dos pines GPIO digitales de la tarjeta ESP32
- CON-03: Las celdas de carga deben conectarse al módulo HX711
- CON-05: Se dbe hacer una simulación usando [Wokwi](https://wokwi.com/)
- CON-06: Se debe hacer la simulación del sistema usando una placa de desarrollo ESP32

### 2. Análisis del sistema y diseño (arquitectura y componentes)

1. Placa de desarrollo ESP32 S3 DevKit C
2. Módulo para celdas de carga HX711
3. 4 celdas de carga
4. 4 Pantalla LCD 16x2 con protocolo I2C
5. 15 jumpers variados

### 3. Diseño de Hardware e Integración

![3. Diseño de Harware e Integración](Diseño-Hardware-Sim.png)

### 4. Diseño y desarrollo del firmware

**Pseudocódigo**
```
SET estado ← Pausa
SET ledEncendido ← 1
SET intervaloLEDS ← 1000

INICIO CICLO

  SI boton presionado ENTONCES
    SI estado = Pausa ENTONCES
      SET estado ← Ejecucion
    SINO
      SET estado ← Pausa
    FIN SI
  FIN SI

  SI (estado = Ejecucion) & (tiempoActual - ultimaActualización) > intervaloLEDS ENTONCES
    SET ultimaActualización ← tiempoActual

    PARA I DE 1 A 4
      APAGA LED No. I
    FIN PARA

    ENCENDER LED No. ledEncendido

    SET ledEncendido ← ledEncendido + 1

    SI ledEncendido >= 5 ENTONCES
      SET ledEncendido ← 1
    FIN SI
  FIN SI

FIN CICLO
```

**Código simulación**

```
// Pines de los LEDs (ajusta según tu conexión)
const int leds[4] = {16, 4, 0, 2};

// Pin del botón
const int pinBoton = 22;

// Variables de control
bool activo = false;                // Estado: encendido/apagado de la secuencia
int ledActual = 0;                  // LED actual encendido
unsigned long ultimaActualizacion = 0;        // Última actualización de LED
unsigned long tiempoBoton = 0;    // Tiempo del último cambio del botón
const unsigned long intervalo = 1000; // Intervalo entre LEDs (1 segundo)
const unsigned long tiempoRebote = 200; // Antirrebote del botón (200 ms)

// Lectura previa del botón
int ultimoEstadoBoton = HIGH;

void setup() {
  // Configuración de pines
  for (int i = 0; i < 4; i++) {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW);
  }
  pinMode(pinBoton, INPUT_PULLUP); // Botón con resistencia interna
  Serial.begin(9600);
}

void loop() {
  // --- LECTURA DEL BOTÓN CON ANTIRREBOTE ---
  int estadoBoton = digitalRead(pinBoton);

  if (estadoBoton == LOW && ultimoEstadoBoton == HIGH && (millis() - tiempoBoton > tiempoRebote)) {
    activo = !activo; // Cambia el estado (toggle)
    tiempoBoton = millis();
    Serial.print("Estado: ");
    Serial.println(activo ? "Activo" : "Parado");
  }
  ultimoEstadoBoton = estadoBoton;

  // --- CONTROL DE LA SECUENCIA ---
  if (activo && (millis() - ultimaActualizacion >= intervalo)) {
    ultimaActualizacion = millis();

    // Apagar todos los LEDs
    for (int i = 0; i < 4; i++) {
      digitalWrite(leds[i], LOW);
    }

    // Encender el LED actual
    digitalWrite(leds[ledActual], HIGH);

    // Avanzar al siguiente LED
    ledActual++;
    if (ledActual >= 4) {
      ledActual = 0; // Vuelve al primero (cíclico)
    }
  }
}
```
**Código implementación**

```
// Pines de los LEDs (ajusta según tu conexión)
const int leds[4] = {47, 48, 45, 35};

// Pin del botón
const int pinBoton = 36;

// Variables de control
bool activo = false;                // Estado: encendido/apagado de la secuencia
int ledActual = 0;                  // LED actual encendido
unsigned long ultimaActualizacion = 0;        // Última actualización de LED
unsigned long tiempoBoton = 0;    // Tiempo del último cambio del botón
const unsigned long intervalo = 1000; // Intervalo entre LEDs (1 segundo)
const unsigned long tiempoRebote = 200; // Antirrebote del botón (200 ms)

// Lectura previa del botón
int ultimoEstadoBoton = HIGH;

void setup() {
  // Configuración de pines
  for (int i = 0; i < 4; i++) {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW);
  }
  pinMode(pinBoton, INPUT_PULLUP); // Botón con resistencia interna
  Serial.begin(9600);
}

void loop() {
  // --- LECTURA DEL BOTÓN CON ANTIRREBOTE ---
  int estadoBoton = digitalRead(pinBoton);

  if (estadoBoton == LOW && ultimoEstadoBoton == HIGH && (millis() - tiempoBoton > tiempoRebote)) {
    activo = !activo; // Cambia el estado (toggle)
    tiempoBoton = millis();
    Serial.print("Estado: ");
    Serial.println(activo ? "Activo" : "Parado");
  }
  ultimoEstadoBoton = estadoBoton;

  // --- CONTROL DE LA SECUENCIA ---
  if (activo && (millis() - ultimaActualizacion >= intervalo)) {
    ultimaActualizacion = millis();

    // Apagar todos los LEDs
    for (int i = 0; i < 4; i++) {
      digitalWrite(leds[i], LOW);
    }

    // Encender el LED actual
    digitalWrite(leds[ledActual], HIGH);

    // Avanzar al siguiente LED
    ledActual++;
    if (ledActual >= 4) {
      ledActual = 0; // Vuelve al primero (cíclico)
    }
  }
}
```
### 5. Pruebas y validación

[Enlace al archivo con el código Arduino/C++](./Lab1-Challenge1/Lab1-Challenge1.ino)

[Enlace a la simulación](https://wokwi.com/projects/448158164631347201)
