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
FirebaseData fbdo;
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

        //------------ Leer desde FireBase Realtime DataBase ---------

        // -------------- Leer valor PWM desde RTDB --------------
        if (Firebase.RTDB.getInt(&fbdo, "LED/analog"))
        {
            if (fbdo.dataType() == "int")
            {
                pwmValue = fbdo.intData();
                Serial.println("Lectura exitosa desde " + fbdo.dataPath() + ": " + pwmValue + " (" + fbdo.dataType() + ")");
                ledcWrite(PWMChannel, pwmValue);
            }
        }
        else
        {
            Serial.println("Error: " + fbdo.errorReason());
        }

        // -------------- Leer estado digital para LED verde --------------
        if (Firebase.RTDB.getBool(&fbdo, "LED/digital")) // Cambié getInt por getBool para tipo booleano
        {
            if (fbdo.dataType() == "boolean")
            {
                ledStatus = fbdo.boolData();
                Serial.println("Lectura exitosa desde " + fbdo.dataPath() + ": " + ledStatus + " (" + fbdo.dataType() + ")");
                digitalWrite(LED_VERDE, ledStatus);
            }
        }
        else
        {
            Serial.println("Error: " + fbdo.errorReason());
        }
    }
}
