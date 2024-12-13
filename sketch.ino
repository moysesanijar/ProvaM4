#include <WiFi.h>       // Biblioteca para conexão WiFi
#include <HTTPClient.h> // Biblioteca para realizar requisições HTTP

// Definição dos pinos dos LEDs
#define led_verde 2
#define led_vermelho 40
#define led_amarelo 9

// Definição do pino do botão
const int buttonPin = 18;  
int buttonState = LOW;        // Estado atual do botão (estável após debounce)
int lastButtonReading = LOW;  // Última leitura bruta do botão (antes do debounce)
unsigned long lastDebounceTime = 0; // Momento da última mudança de estado do botão
unsigned long debounceDelay = 50;   // Tempo de debounce em ms

// Definição do pino do LDR 
const int ldrPin = 4;       
int threshold = 600; // Limite para diferenciar claro/escuro

void setup() {
  // Configura os pinos dos LEDs como saída
  pinMode(led_verde, OUTPUT);
  pinMode(led_vermelho, OUTPUT);
  pinMode(led_amarelo, OUTPUT);

  // Configura o pino do botão como entrada
  pinMode(buttonPin, INPUT);

  // Garante que todos os LEDs estejam apagados na inicialização
  digitalWrite(led_verde, LOW);
  digitalWrite(led_vermelho, LOW);
  digitalWrite(led_amarelo, LOW);

  // Inicializa a comunicação serial para depuração
  Serial.begin(9600);

  // Inicia a conexão WiFi 
  WiFi.begin("Wokwi-GUEST", "");

  // Aguarda a conexão com a rede WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("\nConectado ao WiFi com sucesso!");

  // Leitura inicial do estado do botão 
  lastButtonReading = digitalRead(buttonPin);

  // Caso esteja conectado à Internet, realiza uma requisição HTTP inicial (teste)
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String serverPath = "http://www.google.com.br/";

    // Inicia a conexão HTTP
    http.begin(serverPath.c_str());
    int httpResponseCode = http.GET(); // Executa a requisição GET

    // Verifica o código de resposta HTTP
    if (httpResponseCode > 0) {
      Serial.print("Código de resposta HTTP: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
    } else {
      Serial.print("Código de erro: ");
      Serial.println(httpResponseCode);
    }

    // Finaliza a conexão HTTP
    http.end();

  } else {
    // Caso não conectado ao WiFi, imprime no Serial
    Serial.println("WiFi Desconectado");
  }
}

void loop() {
  // Leitura bruta do estado do botão
  int reading = digitalRead(buttonPin);

  // Implementação do debounce do botão
  if (reading != lastButtonReading) {
    lastDebounceTime = millis();    // Reinicia o contador de debounce
    lastButtonReading = reading;
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // Após o tempo de debounce, confirma o estado do botão
    buttonState = lastButtonReading;
  }

  // Leitura do valor do LDR
  int ldrstatus = analogRead(ldrPin);

  // Verifica o valor lido e escolhe o modo
  if (ldrstatus <= threshold) {
    // Está escuro: modo noturno -> pisca o LED amarelo a cada segundo
    Serial.print("Está escuro (modo noturno ativo). Valor do LDR: ");
    Serial.println(ldrstatus);

    // Apaga os LEDs de verde e vermelho
    digitalWrite(led_verde, LOW);
    digitalWrite(led_vermelho, LOW);

    // Pisca o LED amarelo: acende 1s, apaga 1s
    digitalWrite(led_amarelo, HIGH);
    delay(1000);
    digitalWrite(led_amarelo, LOW);
    delay(1000);

  } else {
    // Está claro: modo convencional
    Serial.print("Está claro (modo convencional ativo). Valor do LDR: ");
    Serial.println(ldrstatus);

    // Sequência: Verde (3s), Amarelo (2s), Vermelho (5s)
    digitalWrite(led_amarelo, LOW);
    digitalWrite(led_verde, LOW);
    digitalWrite(led_vermelho, LOW);

    // Verde por 3 segundos
    digitalWrite(led_verde, HIGH);
    delay(3000);
    digitalWrite(led_verde, LOW);

    // Amarelo por 2 segundos
    digitalWrite(led_amarelo, HIGH);
    delay(2000);
    digitalWrite(led_amarelo, LOW);

    // Vermelho por 5 segundos
    digitalWrite(led_vermelho, HIGH);

    // Durante o vermelho, verificar se o botão é pressionado.
    unsigned long startTimeRed = millis();
    unsigned long redDuration = 5000; // 5 segundos no vermelho
    unsigned long endTimeRed = startTimeRed + redDuration;

    int pressCount = 0;             // Contador de pressões do botão durante o vermelho
    bool firstPressDetected = false;
    unsigned long firstPressTime = 0; 
    bool greenOpened = false;
    bool requestSent = false;

    // Loop para monitorar o botão durante o vermelho
    while (millis() < endTimeRed) {
      // Leitura do botão com debounce durante o vermelho
      int currentReading = digitalRead(buttonPin);
      if (currentReading != lastButtonReading) {
        lastDebounceTime = millis();
        lastButtonReading = currentReading;
      }

      if ((millis() - lastDebounceTime) > debounceDelay) {
        buttonState = lastButtonReading;
      }

      // Caso o botão seja pressionado (estado HIGH)
      if (buttonState == HIGH) {
        pressCount++;
        Serial.print("Botão pressionado durante o vermelho. Contagem de pressões: ");
        Serial.println(pressCount);

        // Se for a primeira pressão, inicia contagem de 1 segundo para abrir o semáforo
        if (!firstPressDetected) {
          firstPressDetected = true;
          firstPressTime = millis();
        }

        if (pressCount == 3 && !requestSent) {
          Serial.println("3 pressões detectadas. Enviando requisição HTTP de alerta...");

          if (WiFi.status() == WL_CONNECTED) {
            HTTPClient http;
            String serverPath = "http://www.google.com.br/";
            http.begin(serverPath.c_str());
            int httpResponseCode = http.GET();
            if (httpResponseCode > 0) {
              Serial.print("Código de resposta HTTP (alerta): ");
              Serial.println(httpResponseCode);
              String payload = http.getString();
              Serial.println(payload);
            } else {
              Serial.print("Código de erro na requisição de alerta: ");
              Serial.println(httpResponseCode);
            }
            http.end();
          } else {
            Serial.println("WiFi Desconectado. Não foi possível enviar alerta.");
          }

          requestSent = true;
        }

        // Aguarda até que o botão seja solto para não contar múltiplas vezes a mesma pressão
        while (digitalRead(buttonPin) == HIGH) {
          delay(10); 
        }
      }

      // Verifica se 1 segundo se passou desde a primeira pressão para abrir o semáforo
      if (firstPressDetected && !greenOpened && (millis() - firstPressTime >= 1000)) {
        // "Abre" o semáforo (verde)
        digitalWrite(led_vermelho, LOW);
        digitalWrite(led_verde, HIGH);
        Serial.println("Abrindo o semáforo (verde) após 1 segundo do primeiro pressionamento.");
        // Segura o verde por 3s
        delay(3000);
        digitalWrite(led_verde, LOW);
        greenOpened = true;
        break;
      }

      delay(10); // Pequeno atraso para não travar o loop completamente
    }
    if (!greenOpened) {
      digitalWrite(led_vermelho, LOW);
      Serial.println("Ciclo concluído sem abertura antecipada. Semáforo agora liberado.");
    }
  }
}