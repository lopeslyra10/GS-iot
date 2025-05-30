#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <DHTesp.h>

// --- Configuração Wi-Fi ---
const char* SSID = "Wokwi-GUEST";
const char* PASSWORD = "";

// --- API HTTPS ---
const char* API_ENDPOINT = "https://SEU_NGROK/tarefas/sensor/leitura"; // Substitua pela sua URL HTTPS real

// --- Pinos ---
const int DHT_PIN = 15;
const int BUTTON_PIN = 12;
const int LED_PIN = 2;

// --- Sensor DHT ---
DHTesp dht;

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

  dht.setup(DHT_PIN, DHTesp::DHT22);

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
    TempAndHumidity data = dht.getTempAndHumidity();
    float temperatura = data.temperature;
    float umidade = data.humidity;
    int nivelAgua = random(60, 110); // Simulação de nível

    // --- Envio via POST HTTPS para API local ---
    WiFiClientSecure secureClient;
    secureClient.setInsecure(); // Modo protótipo

    HTTPClient https;
    if (https.begin(secureClient, API_ENDPOINT)) {
      https.addHeader("Content-Type", "application/json");

      String jsonBody = "{\"idSensor\":" + String(idSensor) +
                        ",\"nivelAgua\":" + String(nivelAgua) +
                        ",\"temperatura\":" + String(temperatura, 1) +
                        ",\"umidade\":" + String(umidade, 1) + "}";

      int httpResponseCode = https.POST(jsonBody);

      if (httpResponseCode > 0) {
        Serial.println("POST enviado com sucesso. Código: " + String(httpResponseCode));
        digitalWrite(LED_PIN, HIGH);
      } else {
        Serial.println("Erro no POST. Código: " + String(httpResponseCode));
        digitalWrite(LED_PIN, LOW);
      }

      https.end();
    } else {
      Serial.println("Falha ao iniciar conexão HTTPS.");
    }

    delay(1000);
    digitalWrite(LED_PIN, LOW);
    delay(20000); // Aguardar para nova leitura
  }

  lastButtonState = buttonState;
}
