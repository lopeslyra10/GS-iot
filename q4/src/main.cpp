#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ThingSpeak.h>
#include <HTTPClient.h>
#include <DHT.h>

// --- Configuração Wi-Fi ---
const char* SSID = "Wokwi-GUEST";
const char* PASSWORD = "";

// --- API LOCAL (Java Spring Boot) ---
const char* LEITURA_ENDPOINT = "http://192.168.0.105:8080/leituras";
const char* ALERTA_ENDPOINT = "http://192.168.0.105:8080/alertas";

const char* THINGSPEAK_API_KEY = "ZEX7LVYWNW05LGY6";
unsigned long channelID = 2972280;

// --- Pinos ---
#define DHTPIN 15
#define DHTTYPE DHT22
const int BUTTON_PIN = 12;
const int LED_PIN = 2;

// --- Sensor DHT ---
DHT dht(DHTPIN, DHTTYPE);

// --- Estado do botão ---
bool lastButtonState = HIGH;

// --- ID do sensor (definido via Serial) ---
int idSensor = 0;

void setup() {
  Serial.begin(115200);
  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao Wi-Fi...");
  }

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  dht.begin();
  Serial.println("Sensor iniciado. Digite o ID do sensor:");
}

void loop() {
  // --- Leitura do ID via Serial ---
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input.length() > 0 && input.toInt() > 0) {
      idSensor = input.toInt();
      Serial.println("ID do sensor definido: " + String(idSensor));
    }
  }

  // --- Verifica botão pressionado ---
  bool buttonState = digitalRead(BUTTON_PIN);
  if (lastButtonState == HIGH && buttonState == LOW && idSensor > 0) {
    float temperatura = dht.readTemperature();
    float umidade = dht.readHumidity();

    if (isnan(temperatura) || isnan(umidade)) {
      Serial.println("Erro ao ler os dados do DHT!");
      return;
    }

    int nivelRisco = random(60, 110); // Simulação de risco

    // --- Envia leitura para a API Java ---
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;

      http.begin(LEITURA_ENDPOINT);
      http.addHeader("Content-Type", "application/json");

      String leituraJson = "{\"idSensor\":" + String(idSensor) +
                           ",\"nivelRisco\":" + String(nivelRisco) +
                           ",\"temperatura\":" + String(temperatura, 1) +
                           ",\"umidade\":" + String(umidade, 1) + "}";

      int leituraResponse = http.POST(leituraJson);
      Serial.println("Leitura enviada. Código: " + String(leituraResponse));
      http.end();

      // --- Envia alerta se necessário ---
      if (temperatura >= 30.0) {
        HTTPClient alertaHttp;
        alertaHttp.begin(ALERTA_ENDPOINT);
        alertaHttp.addHeader("Content-Type", "application/json");

        String alertaJson = "{\"descricao\":\"Temperatura crítica: " + String(temperatura, 1) + "°C\","
                            "\"grauRisco\":\"ALTO\","
                            "\"dataHora\":\"2025-06-03T15:00:00\","
                            "\"localId\":1}";

        int alertaResponse = alertaHttp.POST(alertaJson);
        Serial.println("Alerta enviado. Código: " + String(alertaResponse));
        alertaHttp.end();

        digitalWrite(LED_PIN, HIGH);
      } else {
        digitalWrite(LED_PIN, LOW);
      }

      // --- Envia dados ao ThingSpeak ---
      HTTPClient tsClient;
      String tsURL = String(THINGSPEAK_URL) +
                     "?api_key=" + THINGSPEAK_API_KEY +
                     "&field1=" + String(temperatura, 1) +
                     "&field2=" + String(umidade, 1) +
                     "&field3=" + String(nivelRisco);

      tsClient.begin(tsURL);
      int tsCode = tsClient.GET();

      if (tsCode > 0) {
        Serial.println("ThingSpeak: Sucesso [" + String(tsCode) + "]");
      } else {
        Serial.println("ThingSpeak: Erro [" + String(tsCode) + "]");
      }

      tsClient.end();
    }

    delay(1000);
    digitalWrite(LED_PIN, LOW);
    delay(20000); // Aguarda 20 segundos
  }

  lastButtonState = buttonState;
}
