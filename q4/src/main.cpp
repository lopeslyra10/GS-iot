#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ThingSpeak.h>
#include <HTTPClient.h>
#include <DHT.h>

// Wi-Fi
const char *SSID = "Wokwi-GUEST";
const char *PASSWORD = "";

// API endpoint
const char *ALERTA_ENDPOINT = "https://safeheat-backend-java.onrender.com/alertas";

WiFiClient client;
unsigned long channelID = 2972280;
const char *THINGSPEAK_API_KEY = "ZEX7LVYWNW05LGY6";

#define DHTPIN 15
#define DHTTYPE DHT22
const int BUTTON_PIN = 12;
const int LED_PIN = 2;

DHT dht(DHTPIN, DHTTYPE);

bool lastButtonState = HIGH;
int idSensor = 0;

void setup()
{
  Serial.begin(115200);
  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Conectando ao Wi-Fi...");
  }

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  dht.begin();
  ThingSpeak.begin(client);

  Serial.println("Sensor iniciado. Digite o ID do sensor:");
}

void loop()
{
  if (Serial.available() > 0)
  {
    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input.length() > 0 && input.toInt() > 0)
    {
      idSensor = input.toInt();
      Serial.println("ID do sensor definido: " + String(idSensor));
    }
  }

  bool buttonState = digitalRead(BUTTON_PIN);
  if (lastButtonState == HIGH && buttonState == LOW && idSensor > 0)
  {
    float temperatura = dht.readTemperature();
    float umidade = dht.readHumidity();

    if (isnan(temperatura) || isnan(umidade))
    {
      Serial.println("Erro ao ler os dados do DHT!");
      return;
    }

    String nivelRisco;
    String mensagem;

    if (temperatura >= 45)
    {
      nivelRisco = "Crítico";
      mensagem = "Risco extremo de calor nessa região";
    }
    else if (temperatura >= 37)
    {
      nivelRisco = "Médio";
      mensagem = "Risco de calor nessa região";
    }
    else if (temperatura >= 30)
    {
      nivelRisco = "Baixo";
      mensagem = "Temperatura elevada";
    }
    else
    {
      nivelRisco = "Normal";
      mensagem = "Temperatura dentro do esperado";
    }

    if (WiFi.status() == WL_CONNECTED && temperatura >= 30.0)
{
  WiFiClientSecure secureClient;
  secureClient.setInsecure(); // Ignora o certificado

  HTTPClient alertaHttp;
  alertaHttp.begin(secureClient, ALERTA_ENDPOINT);
  alertaHttp.addHeader("Content-Type", "application/json");

  String alertaJson = "{\"temperatura\":\"" + String(temperatura, 1) + "\"," +
                      "\"mensagem\":\"" + mensagem + "\"," +
                      "\"nivel_risco\":\"" + nivelRisco + "\"," +
                      "\"id_local\":" + String(idSensor) + "}";

  Serial.println("JSON enviado:");
  Serial.println(alertaJson);

  int alertaResponse = alertaHttp.POST(alertaJson);
  Serial.println("Alerta enviado. Código: " + String(alertaResponse));

  if (alertaResponse > 0) {
    String resposta = alertaHttp.getString();
    Serial.println("Resposta da API:");
    Serial.println(resposta);
  } else {
    Serial.println("Erro ao conectar com a API.");
  }

  alertaHttp.end();
  digitalWrite(LED_PIN, HIGH);
}

    else
    {
      digitalWrite(LED_PIN, LOW);
    }

    ThingSpeak.setField(1, temperatura);
    ThingSpeak.setField(2, umidade);

    int tsCode = ThingSpeak.writeFields(channelID, THINGSPEAK_API_KEY);

    if (tsCode == 200)
    {
      Serial.println("ThingSpeak: Dados enviados com sucesso!");
    }
    else
    {
      Serial.println("ThingSpeak: Erro ao enviar [" + String(tsCode) + "]");
    }

    delay(1000);
    digitalWrite(LED_PIN, LOW);
    delay(20000); // Aguarda 20 segundos
  }

  lastButtonState = buttonState;
}
