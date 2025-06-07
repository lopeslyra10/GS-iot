# SafeHeat IoT 🌡️🚨

Projeto focado em monitorar e prevenir riscos relacionados ao calor extremo em tempo real, emitindo alertas e direcionando pessoas a centros de apoio. Este sistema utiliza um microcontrolador ESP32 para coletar dados de temperatura e umidade, enviando-os para o ThingSpeak para visualização e também para uma API Java Spring Boot, que gerencia os alertas e informações de locais.

---

## 📡 Descrição

O projeto SafeHeat IoT tem como principal objetivo proteger a população de riscos de saúde causados por ondas de calor. Ele faz isso monitorando as condições climáticas e emitindo alertas de risco, além de fornecer uma plataforma para que autoridades visualizem dados e que usuários busquem centros de apoio.

O sistema integra as seguintes tecnologias:

-   **ESP32 + C++:** Para coleta de dados e comunicação.
-   **ThingSpeak:** Para visualização e monitoramento de dados em tempo real.
-   **API REST em Java com Spring Boot:** Backend para gerenciamento de usuários, locais e alertas.
-   **Wokwi:** Simulador online para desenvolvimento e testes.

---

## 📁 Estrutura do Projeto

```bash
safeheat-iot/
├── main.ino         # Código principal do ESP32
├── diagram.json     # Diagrama (se houver, para Wokwi)
├── README.md        # Este arquivo
```

---

## 🔧 Tecnologias Utilizadas

-   ✅ **ESP32 DevKit**
-   🧰 **Arduino IDE ou VSCode + PlatformIO**
-   📊 **ThingSpeak** (canal público para dashboard)
-   🌐 **WiFi** para conexão com a internet
-   ☕ **Spring Boot API** (hospedada em `onrender.com`)
-   🧪 **Wokwi** (simulador online)
-   🌡️ **Sensor DHT22** (temperatura e umidade)

---

## 📲 Funcionalidades

-   🔌 **Conexão automática do ESP32** com rede Wi-Fi.
-   📝 **Definição do ID do sensor via Monitor Serial:** Permite associar o dispositivo a um local específico cadastrado no backend.
-   📤 **Leitura de temperatura e umidade** usando o sensor DHT22.
-   🚦 **Avaliação do nível de risco** (Normal, Baixo, Médio, Crítico) com base na temperatura.
-   🚨 **Envio de alertas para a API Java:** Quando a temperatura atinge 30°C ou mais, um alerta JSON é enviado com dados de temperatura, nível de risco, mensagem e ID do local.
-   📊 **Envio de dados para o ThingSpeak:** A temperatura e umidade são enviadas para o ThingSpeak para visualização em dashboard.
-   💡 **Feedback visual via LED:** O LED no ESP32 acende quando um alerta é enviado.
-   ⏱️ **Intervalo de envio:** Após o envio de um alerta, o sistema aguarda 20 segundos antes de permitir um novo envio.

---

## 🛠️ Como usar

1.  **Clone o repositório:**
    ```bash
    git clone https://github.com/lopeslyra10/GS-iot.git
    ```
2.  **Abra no VSCode ou Arduino IDE.**

3.  **Configure suas credenciais:**
    No `main.ino`, ajuste as seguintes variáveis:
    ```cpp
    const char *SSID = "Wokwi-GUEST"; // Seu SSID da rede Wi-Fi
    const char *PASSWORD = ""; // Sua senha da rede Wi-Fi

    unsigned long channelID = 2972280; // ID do seu canal ThingSpeak
    const char *THINGSPEAK_API_KEY = "ZEX7LVYWNW05LGY6"; // Sua Write API Key do ThingSpeak

    // Endereço da API Java (já configurado para o ambiente de produção)
    const char *ALERTA_ENDPOINT = "[https://safeheat-backend-java.onrender.com/alertas](https://safeheat-backend-java.onrender.com/alertas)";
    ```

4.  **Crie um Usuário e um Local na API:**
    Antes de testar o hardware, é necessário criar um usuário e um local associado a ele no backend. Você pode fazer isso usando o Postman ou outra ferramenta similar, enviando requisições POST para os endpoints apropriados da API.

    Exemplo de criação de usuário:
    `POST https://safeheat-backend-java.onrender.com/usuarios`
    ```json
    {
        "nome": "Seu Nome",
        "email": "seu.email@exemplo.com",
        "senha": "sua_senha"
    }
    ```

    Exemplo de criação de local (com `id_usuario` do usuário recém-criado):
    `POST https://safeheat-backend-java.onrender.com/locais`
    ```json
    {
        "nome": "Nome do Local",
        "rua": "Rua Exemplo",
        "numero": "123",
        "complemento": "Apto 1",
        "bairro": "Bairro Exemplo",
        "cidade": "Cidade Exemplo",
        "estado": "SP",
        "cep": "00000000",
        "id_usuario": [ID_DO_SEU_USUARIO]
    }
    ```

5.  **Faça o upload para o ESP32**
    Conecte seu ESP32 ao computador e use a IDE (Arduino IDE ou VSCode + PlatformIO) para carregar o código. Após o upload, abra o **Monitor Serial**.

6.  **Interaja via Monitor Serial:**
    Quando o "Sensor iniciado. Digite o ID do sensor:" aparecer, digite o **ID do local** que você criou na API e pressione **Enter**. Por exemplo, se o ID do seu local é `42`, digite `42`.

7.  **Acione o Alerta:**
    Pressione o botão físico conectado ao **GPIO 12** do seu ESP32. Isso irá disparar a leitura dos sensores, o envio de dados para o ThingSpeak e, se a temperatura estiver acima de 30°C, um alerta para a API.

---

## 🌐 Painel ThingSpeak

Acompanhe os dados de temperatura e umidade em tempo real acessando o canal público do ThingSpeak:
[https://thingspeak.com/channels/2972280](https://thingspeak.com/channels/2972280)

---

## Resultados de Teste

Abaixo, um exemplo da interação no Monitor Serial e da resposta da API após o envio de um alerta:

```
rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:2
load:0x3fff0030,len:1156
load:0x40078000,len:11456
ho 0 tail 12 room 4
load:0x40080400,len:2972
entry 0x400805dc
Conectando ao Wi-Fi...
Sensor iniciado. Digite o ID do sensor:
ID do sensor definido: 42
JSON enviado:
{"temperatura":"44.2","mensagem":"Risco de calor nessa região","nivel_risco":"Médio","id_local":42}
Alerta enviado. Código: 200
Resposta da API:
{"id_alerta":43,"temperatura":"44.2","nivel_risco":"Médio","mensagem":"Risco de calor nessa região","data_alerta":"2025-06-07T17:20:51.53569396","id_local":42}
ThingSpeak: Dados enviados com sucesso!
```

---

## Exemplo de Resposta da API (Detalhe do Local com Alerta)

```json
{
  "id_usuario": 45,
  "nome": "Augusto",
  "email": "augustolopes@gmail.com",
  "senha": "augusto123",
  "locais": [
    {
      "id_local": 42,
      "nome": "Casa Augusto",
      "rua": "Rua Sirim",
      "numero": "55",
      "complemento": "Casa 1",
      "bairro": "Jardim Planalto",
      "cidade": "São Paulo",
      "estado": "SP",
      "cep": "03984090",
      "id_usuario": 45,
      "alertas": [
        {
          "id_alerta": 43,
          "temperatura": "44.2",
          "nivel_risco": "Médio",
          "mensagem": "Risco de calor nessa região",
          "data_alerta": "2025-06-07T17:20:51.535694",
          "id_local": 42
        }
      ]
    }
  ]
}
```

---

## ✍️ Autores

**Grupo SafeHeat - FIAP**

**Participantes:**

-   Augusto Lopes Lyra (RM: 558209)
-   Felipe Ulson Sora (RM: 555462)
-   Vinícius Ribeiro Nery Costa (RM: 559165)

---

## 📅 Status do Projeto

🟢 **Sprint 1:** Protótipo com monitoramento de temperatura/umidade, envio de alertas para API e ThingSpeak funcionando.

---

## 📌 Licença

Este projeto é de uso educacional, desenvolvido para fins acadêmicos.
