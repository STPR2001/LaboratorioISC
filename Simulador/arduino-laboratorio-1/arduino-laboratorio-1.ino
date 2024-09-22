#include <Keypad.h>
#include <OneWire.h>
#include <DallasTemperature.h>

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {2, 3, 4, 5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {8, 7, 6}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

const int temperatureSensorPin = 12;

const int redLed = 9;
const int greenLed = 10;
const int blueLed = 11;

OneWire oneWire(temperatureSensorPin);
DallasTemperature sensors(&oneWire);

String password = "12345";
String userInput = "";

unsigned long startTime = 0;
bool isHashHeld = false;

String ESTADO[] = {"MODO_PROGRAMACION", "ACTIVADO", "DESACTIVADO", "SUPERA_UMBRAL"};
String ESTADO_ACTUAL;
String ESTADO_ANTERIOR;

float umbralTemp = 35;

unsigned long previousTempTime = 0;
unsigned long tempInterval = 4000;  // Intervalo de 2 segundos para actualizar temperatura

void setup() {
  Serial.begin(9600);
  sensors.begin();
  ESTADO_ACTUAL = ESTADO[2];  // Inicia en DESACTIVADO
  ESTADO_ANTERIOR = "";       // Inicializa vacío para asegurarse de que el primer estado se imprima

  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
}

void loop() {
  char key = keypad.getKey();

  if (key) {
    if (key == '*') {
      userInput = "";
    } else if (key == '#') {
      if (ESTADO_ACTUAL != ESTADO[0]) {
        if (!isHashHeld) {
          if (userInput == password) {
            if (ESTADO_ACTUAL == ESTADO[1]) {
              ESTADO_ACTUAL = ESTADO[2];
            } else if (ESTADO_ACTUAL == ESTADO[2]) {
              ESTADO_ACTUAL = ESTADO[1];
            }
          }
          userInput = "";
        }
      } else if (ESTADO_ACTUAL == ESTADO[0]) {
        if (userInput.length() == 5) {
          password = userInput;
          userInput = "";
          ESTADO_ACTUAL = ESTADO[2];
        }
      }
    } else {
      userInput += key;
      Serial.println("Entrada: " + userInput);
    }
  }

  if (keypad.getState() == PRESSED && key == '#') {
    if (!isHashHeld) {
      startTime = millis();
      isHashHeld = true;
    }
  }

  if (isHashHeld && millis() - startTime >= 5000) {
    ESTADO_ACTUAL = ESTADO[0];
    isHashHeld = false;
    startTime = 0;
  }

  if (keypad.getState() == RELEASED && isHashHeld) {
    isHashHeld = false;
    startTime = 0;
  }

  // Verifica si el estado ha cambiado
  if (ESTADO_ACTUAL != ESTADO_ANTERIOR) {
    Serial.println(ESTADO_ACTUAL);
    ESTADO_ANTERIOR = ESTADO_ACTUAL; // Actualiza el estado anterior
  }

  // Modo programación
  if (ESTADO_ACTUAL == "MODO_PROGRAMACION") {
    setColor(0, 0, 255);
  }
  // Estado Activado
  else if (ESTADO_ACTUAL == "ACTIVADO") {
    setColor(0, 255, 0);
    
    // Ejecutar solo si ha pasado el intervalo de tiempo
    if (millis() - previousTempTime >= tempInterval) {
      sensors.requestTemperatures();
      float temp = sensors.getTempCByIndex(0);
      Serial.println("Temperatura: " + String(temp) + " °C");

      int analogValue = analogRead(A0);
      Serial.println("Luminocidad: " + String(analogValue));

      if (temp >= umbralTemp) {
        ESTADO_ACTUAL = ESTADO[3];
      }

      previousTempTime = millis(); // Actualiza el tiempo de la última lectura
    }
  }
  // Estado Desactivado
  else if (ESTADO_ACTUAL == "DESACTIVADO") {
    setColor(255, 0, 0);
  }
  // Estado Supera Umbral
  else if (ESTADO_ACTUAL == "SUPERA_UMBRAL") {
    setColor(255, 255, 0);
    
    // Ejecutar solo si ha pasado el intervalo de tiempo
    if (millis() - previousTempTime >= tempInterval) {
      sensors.requestTemperatures();
      float temp = sensors.getTempCByIndex(0);
      Serial.println("Temperatura: " + String(temp) + " °C");

      int analogValue = analogRead(A0);
      Serial.println("Luminocidad: " + String(analogValue));

      if (temp < umbralTemp) {
        ESTADO_ACTUAL = ESTADO[1];
      }

      previousTempTime = millis(); // Actualiza el tiempo de la última lectura
    }
  }
}

void setColor(int redValue, int greenValue, int blueValue) {
  analogWrite(redLed, redValue);
  analogWrite(greenLed, greenValue);
  analogWrite(blueLed, blueValue);
}
