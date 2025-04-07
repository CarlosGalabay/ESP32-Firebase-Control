# 🕹️ ESP32 Firebase Control

Este es un ejemplo de cómo conectar un ESP32 con Firebase para controlar dispositivos como un servo y un LED, así como para recolectar datos de un sensor LDR.

El proyecto ha sido desarrollado utilizando [PlatformIO](https://platformio.org/) en VS Code y se ha simulado completamente en [Wokwi](https://wokwi.com/), pero está diseñado para poder implementarse fácilmente en un entorno físico real.

## ⚙️ Características

- Lectura de sensor LDR y cálculo de voltaje.
- Lectura de estado de un pulsador físico.
- Control remoto de un LED desde Firebase.
- Control de la posición de un servo motor desde Firebase.
- Comunicación bidireccional con Firebase Realtime Database.

## 💻 Tecnologías utilizadas

- ESP32  
- PlatformIO  
- Firebase Realtime Database  
- Firebase ESP32 Client Library  
- Wokwi (simulador)

## 📁 Estructura del proyecto

```
ESP32-FireBase-Control/
│
├── IOT-FireBase-Test/         
│   ├── include/
│   │   └── secrets.h          
│   ├── src/
│   │   └── main.cpp           
│   ├── .gitignore             
│   └── platformio.ini         
│
└── LICENSE
```

## 🚀 Instrucciones de Instalación

Sigue estos pasos para instalar y ejecutar el proyecto:

1. **Clona el repositorio**:
    ```bash
    git clone https://github.com/CarlosGalabay/ESP32-Firebase-Control.git
    ```

2. **Abre el proyecto usando la extensión de PlatformIO en VS Code**
    
4. **Configura las credenciales de Firebase**:
    - Crea un archivo `secrets.h` en el directorio `include/` con las credenciales de tu proyecto Firebase.
    - Asegúrate de incluir las siguientes variables en `secrets.h`:
      
    ```cpp
    #ifndef SECRETS_H
    #define SECRETS_H
    
    #define WIFI_SSID "your-SSID"
    #define WIFI_PASSWORD "your-WIFI-password"
    #define API_KEY "your-Firebase-api-key"
    #define DATABASE_URL "your-Firebase-database-url"

    #endif
    ```

5. **Compila**:
    - Usa el comando de PlatformIO para compilar el código:
    ```bash
    pio run --target upload
    ```

6. **Prueba el proyecto**:
    - Una vez cargado el código, el ESP32 se conectará a Wi-Fi y Firebase.
    - Verifica que puedas controlar el LED y el servo desde Firebase y leer los datos del sensor LDR.

> [!IMPORTANT]
> Es necesario tener instalado las extensiones de PlatformIO y Wokwi para poder compilar el proyecto y ejecutar la simulación.

## 💡 Notas
> [!NOTE]
> Este proyecto está actualmente simulado en Wokwi, lo que permite su prueba sin hardware físico.

> [!TIP]
> Se recomienda probar el código en un entorno real para verificar su funcionamiento con hardware.
