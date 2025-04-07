# 🕹️ ESP32 Firebase Control

Este es un ejemplo de cómo conectar un ESP32 con Firebase para controlar dispositivos como un servo y un LED, así como para recolectar datos de un sensor LDR.

El proyecto ha sido desarrollado utilizando [PlatformIO](https://platformio.org/) en VS Code y se ha simulado completamente en [Wokwi](https://wokwi.com/), pero está diseñado para poder implementarse fácilmente en un entorno físico real.

---

## ⚙️ Características

- Lectura de sensor LDR y cálculo de voltaje.
- Lectura de estado de un pulsador físico.
- Control remoto de un LED desde Firebase.
- Control de la posición de un servo motor desde Firebase.
- Comunicación bidireccional con Firebase Realtime Database.

---

## 💻 Tecnologías utilizadas

- ESP32  
- PlatformIO  
- Firebase Realtime Database  
- Firebase ESP32 Client Library  
- Wokwi (simulador)

---

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

## 💡 Notas
> [!NOTE]
> Este proyecto está actualmente simulado en Wokwi, lo que permite su prueba sin hardware físico.

> [!TIP]
> Se recomienda probar el código en un entorno real para verificar su funcionamiento con hardware.
