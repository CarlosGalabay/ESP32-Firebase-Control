#include <WiFi.h>
#include <ESP32Servo.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
#include "secrets.h"  // Contiene credenciales sensibles (SSID, contraseña, API key, URL)

// ---------------------- Definición de pines ----------------------
#define SERVO_PIN 12
#define LED_VERDE 14
#define LDR_PIN 34
#define SWITCH_PIN 13

// ------------------ Objetos y configuraciones Firebase ------------------
FirebaseData fbdo, fbdo_s1, fbdo_s2;
FirebaseAuth auth;
FirebaseConfig config;

// ---------------------- Objeto para controlar el servo ----------------------
Servo miServo;
int servo_angle = 90;  // Valor inicial del servo (90° = posición neutra)

// ---------------------- Variables generales ----------------------
bool signupOK = false;
unsigned long sendDataPreviousMillis = 0;
int ldrData = 0;
float voltage = 0.0;
bool ledStatus = false;
bool switchStatus = false;

// ---------------------- Rutas de la base de datos en Firebase ----------------------
const char* LDR_PATH = "/sensor/ldr_data";
const char* VOLTAGE_PATH = "/sensor/voltage";
const char* SWITCH_PATH = "/sensor/switch";

void setup()
{
  Serial.begin(115200);  // Inicializa la comunicación serial
  miServo.attach(SERVO_PIN);  // Asocia el pin al servo
  miServo.write(90);  // Pone el servo en la posición inicial

  pinMode(LED_VERDE, OUTPUT);     // Configura el pin del LED como salida
  pinMode(SWITCH_PIN, INPUT);     // Configura el pin del interruptor como entrada

  // ---------------------- Conexión a Wi-Fi ----------------------
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, 6);  // Conecta a la red Wi-Fi
  Serial.print("Conectando a Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(300);
    Serial.print(".");
  }
  Serial.println("\nConectado a Wi-Fi");
  Serial.println("IP: " + WiFi.localIP().toString());

  // ---------------------- Configuración de Firebase ----------------------
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  // Registro anónimo en Firebase
  if (Firebase.signUp(&config, &auth, "", ""))
  {
    Serial.println("Registro en Firebase exitoso");
    signupOK = true;
  }
  else
  {
    Serial.printf("Error de registro: %s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);  // Reintenta conexión si se pierde

  // Inicia escucha de cambios en la base de datos (stream)
  if (!Firebase.RTDB.beginStream(&fbdo_s1, "/Servo"))
    Serial.printf("Stream 1 error: %s\n", fbdo_s1.errorReason().c_str());

  if (!Firebase.RTDB.beginStream(&fbdo_s2, "/LED/digital"))
    Serial.printf("Stream 2 error: %s\n", fbdo_s2.errorReason().c_str());

  Serial.println("Iniciando....");
}

void loop()
{
  // Solo ejecutar si Firebase está listo y se registró correctamente
  if (Firebase.ready() && signupOK)
  {
    // Envía datos cada 5 segundos
    if (millis() - sendDataPreviousMillis > 5000 || sendDataPreviousMillis == 0)
    {
      sendDataPreviousMillis = millis();

      // Lectura del sensor LDR y del interruptor
      ldrData = analogRead(LDR_PIN);
      voltage = (float)analogReadMilliVolts(LDR_PIN) / 1000.0;
      switchStatus = digitalRead(SWITCH_PIN);

      // Envío de los datos al Realtime Database
      if (Firebase.RTDB.setInt(&fbdo, LDR_PATH, ldrData))
        Serial.println("Enviado: " + String(ldrData) + " -> " + LDR_PATH);
      else
        Serial.println("Error al enviar ldr_data: " + fbdo.errorReason());

      if (Firebase.RTDB.setFloat(&fbdo, VOLTAGE_PATH, voltage))
        Serial.println("Enviado: " + String(voltage, 3) + " V -> " + VOLTAGE_PATH);
      else
        Serial.println("Error al enviar voltage: " + fbdo.errorReason());

      if (Firebase.RTDB.setBool(&fbdo, SWITCH_PATH, switchStatus))
        Serial.println("Enviado: " + String(switchStatus) + " -> " + SWITCH_PATH);
      else
        Serial.println("Error al enviar switch: " + fbdo.errorReason());
    }

    // ------------------ Lectura del valor PWM del servo ------------------
    if (!Firebase.RTDB.readStream(&fbdo_s1))
      Serial.printf("Stream 1 error: %s\n", fbdo_s1.errorReason().c_str());

    if (fbdo_s1.streamAvailable() && fbdo_s1.dataType() == "int")
    {
      servo_angle = fbdo_s1.intData();
      Serial.println("Valor Servo desde Firebase: " + String(servo_angle));
      miServo.write(servo_angle);
    }

    // ------------------ Lectura del estado del LED ------------------
    if (!Firebase.RTDB.readStream(&fbdo_s2))
      Serial.printf("Stream 2 error: %s\n", fbdo_s2.errorReason().c_str());

    if (fbdo_s2.streamAvailable() && fbdo_s2.dataType() == "boolean")
    {
      ledStatus = fbdo_s2.boolData();
      Serial.println("LED Verde desde Firebase: " + String(ledStatus ? "ON" : "OFF"));
      digitalWrite(LED_VERDE, ledStatus);
    }
  }
}
