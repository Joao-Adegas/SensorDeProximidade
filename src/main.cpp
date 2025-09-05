#include <LiquidCrystal.h>
#include <WiFi.h>
#include <HTTPClient.h>

// LCD: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(23, 22, 21, 19, 17, 16);

// Sensor ultrassônico
const int trigPin = 5;
const int echoPin = 18;

// LED
const int ledPin = 4;

// Wi-Fi
const char* ssid = "Wokwi-GUEST";
const char* password = ""; // senha correta para Wokwi

// URL do App Script
const char* scriptURL = "https://script.google.com/macros/s/AKfycby4ybj_IOyT2Bik4bN3SnxESbx-ekpwiTdhco04jE4XTNl-i9kflNfFfRC5be3Qn6uK/exec";

void setup() {
  Serial.begin(115200);  // inicializa a Serial
  lcd.begin(16, 2);
  lcd.print("Sistema iniciado");

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);

  WiFi.begin(ssid, password);
  lcd.setCursor(0, 1);
  lcd.print("Conectando...");

  // aguarda conexão
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    lcd.print(".");
  }

  // conexão estabelecida
  lcd.clear();
  lcd.print("Wi-Fi conectado");
  Serial.println("Wi-Fi conectado!"); // mensagem no terminal

  delay(1000);
  lcd.clear();
}

void loop() {
  // Pulso do sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Timeout de 30ms (máx ~5 metros)
  long duration = pulseIn(echoPin, HIGH, 30000);
  float distance = duration * 0.034 / 2;

  if (duration == 0 || distance < 2 || distance > 400) {
    distance = 999; // valor fictício
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Distancia:");
  lcd.setCursor(0, 1);
  if (distance == 999) {
    lcd.print("Sem leitura");
  } else {
    lcd.print(distance);
    lcd.print(" cm");
  }

  if (distance > 0 && distance < 20) {
    digitalWrite(ledPin, HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Voce esta perto");
    lcd.setCursor(0, 1);
    lcd.print(distance);
    lcd.print(" cm");

    // Envia para Google Sheets
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      String url = String(scriptURL) + "?distancia=" + String(distance);
      http.begin(url);
      http.GET();
      http.end();
      Serial.println("Dados enviados para o Google Sheets"); // mensagem no terminal
    }

    delay(3000); // tempo para visualizar
  } else {
    digitalWrite(ledPin, LOW);
  }

  delay(1000);
}
