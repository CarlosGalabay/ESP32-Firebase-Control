#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// Token y helper para Firebase
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

// Credenciales Wi-Fi
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""

// Credenciales Firebase
#define API_KEY "AIzaSyBgXpwUOlBNtuqUUbYFFSUSFLcwQr_LyEI"
#define DATABASE_URL "https://iot-firebase-test-12083-default-rtdb.firebaseio.com/" // Reemplazar con tu URL real

// Pines
#define LED_ROJO 12
#define LED_VERDE 14
#define LDR_PIN 34
#define SWITCH_PIN 13
#define PWMChannel 0

// PWM
const int freq = 5000;
const int resolution = 8;

// Objetos Firebase
FirebaseData fbdo, fbdo_s1, fbdo_s2;
FirebaseAuth auth;
FirebaseConfig config;

bool signupOK = false;
unsigned long sendDataPreviousMillis = 0;
int ldrData = 0;
float voltage = 0.0;
int pwmValue = 0;
bool ledStatus = false;
bool switchStatus = false;

void setup()
{
  pinMode(LED_ROJO, OUTPUT);
  ledcSetup(PWMChannel, freq, resolution);
  ledcAttachPin(LED_ROJO, PWMChannel);

  pinMode(LED_VERDE, OUTPUT);
  pinMode(SWITCH_PIN, INPUT);
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, 6); // Canal 6 acelera conexión en Wokwi

  Serial.print("Conectando a Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(300);
    Serial.print(".");
  }
  Serial.println("\nConectado a Wi-Fi");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // Configuración de Firebase
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

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
  Firebase.reconnectWiFi(true);

  // Stream
  if (Firebase.RTDB.beginStream(&fbdo_s1, "/LED/analog")){

    Serial.printf("Stream 1 error, %s\n\n", fbdo_s1.errorReason().c_str());
  }
  if (Firebase.RTDB.beginStream(&fbdo_s2, "/LED/digital")){

    Serial.printf("Stream 2 error, %s\n\n", fbdo_s2.errorReason().c_str());
  }
}

void loop()
{
  if (Firebase.ready() && signupOK && (millis() - sendDataPreviousMillis > 5000 || sendDataPreviousMillis == 0))
  {
    // Actualizar el tiempo de envío
    sendDataPreviousMillis = millis();

    // Leer datos del sensor
    ldrData = analogRead(LDR_PIN);
    voltage = (float)analogReadMilliVolts(LDR_PIN) / 1000.0; // Convertir a voltios
    switchStatus = digitalRead(SWITCH_PIN);

    // Enviar a FireBase Realtime DataBase

    // --------------------- LDR --------------------------------
    if (Firebase.RTDB.setInt(&fbdo, "/sensor/ldr_data", ldrData))
    {
      Serial.println();
      Serial.print(ldrData);
      Serial.print(" - LDR data guardado en Firebase: " + fbdo.dataPath());
      Serial.println(" (" + fbdo.dataType() + ")");
    }
    else
    {
      Serial.print("Error al guardar LDR data: " + fbdo.errorReason());
    }

    // --------------------- Voltaje --------------------------------
    if (Firebase.RTDB.setFloat(&fbdo, "/sensor/voltage", voltage))
    {
      Serial.print(voltage);
      Serial.print(" - Voltaje guardado en Firebase: " + fbdo.dataPath());
      Serial.println(" (" + fbdo.dataType() + ")");
    }
    else
    {
      Serial.print("Error al guardar voltaje: " + fbdo.errorReason());
    }

    // --------------------- Switch --------------------------------
    if (Firebase.RTDB.setBool(&fbdo, "/sensor/switch", switchStatus))
    {
      Serial.print(switchStatus);
      Serial.print(" - Estado del switch guardado en Firebase: " + fbdo.dataPath());
      Serial.println(" (" + fbdo.dataType() + ")");
    }
    else
    {
      Serial.print("Error al guardar estado del switch: " + fbdo.errorReason());
    }
  }

  //------------ Leer desde FireBase Realtime DataBase (OnDataChange) ---------

  // -------------- Leer valor PWM desde RTDB --------------
  if (Firebase.ready() && signupOK)
  {
    if (!Firebase.RTDB.readStream(&fbdo_s1))
      Serial.printf("Stream 1 error, %s\n\n", fbdo_s1.errorReason().c_str());
    if (fbdo_s1.streamAvailable())
    {
      if (fbdo_s1.dataType() == "int")
      {
        pwmValue = fbdo_s1.intData();
        Serial.println("Lectura exitosa desde " + fbdo_s1.dataPath() + ": " + pwmValue + " (" + fbdo_s1.dataType() + ")");
        ledcWrite(PWMChannel, pwmValue);
      }
    }
  }

  // -------------- Leer estado digital para LED verde --------------
  if (!Firebase.RTDB.readStream(&fbdo_s2))
      Serial.printf("Stream 2 error, %s\n\n", fbdo_s2.errorReason().c_str());
  if (fbdo_s2.streamAvailable()) // Cambié getInt por getBool para tipo booleano
  {
    if (fbdo_s2.dataType() == "boolean")
    {
      ledStatus = fbdo_s2.boolData();
      Serial.println("Lectura exitosa desde " + fbdo_s2.dataPath() + ": " + ledStatus + " (" + fbdo_s2.dataType() + ")");
      digitalWrite(LED_VERDE, ledStatus);
    }
  }
}
