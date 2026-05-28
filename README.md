# Smart Glasses — Obstacle Detection System
### Assistive Technology for Visually Impaired Users

## Overview

Smart Glasses is an embedded assistive technology project 
designed to help visually impaired individuals navigate 
their environment safely and independently.

Using an **ESP32 microcontroller** and an **HC-SR04 
ultrasonic sensor**, the system continuously measures 
the distance to obstacles ahead and alerts the user 
through **real-time buzzer feedback** and **voice alerts** 
via a WiFi-connected web interface.


##  Features

-  **Real-time obstacle detection**
   continuously measures distance up to 400 cm
   
- **Smart buzzer alerts**
  beeping frequency increases as obstacles get closer
  
-  **WiFi Web Interface**
  radar-style dashboard accessible from any device

- **Voice Alerts**
 spoken warnings using Web Speech API

- **Live distance display**
 updates every 500ms

- **3-zone detection system:**
  - 🔴 Danger: 0–20 cm → Stop immediately
  - 🟠 Caution: 20–60 cm → Slow down
  - 🟢 Clear: > 60 cm → Path is safe


##  Hardware Requirements

| Component | Description |
|-----------|-------------|
| ESP32 | Main microcontroller |
| HC-SR04 | Ultrasonic distance sensor |
| Buzzer | Audio feedback component |
| Power Bank | Portable power supply |
| Glasses Frame | Wearable mount |


## Pin Configuration

| Pin  | GPIO  |
|----- |------ |
| TRIG |   14  |
| ECHO |   12  |
| BUZZER | 13  |



## Tech Stack

| Layer | Technology |
|-------|-----------|
| Firmware | C++ / Arduino IDE |
| Microcontroller | ESP32 |
| Web UI | HTML · CSS · JavaScript |
| Communication | WiFi / WebServer |
| Voice | Web Speech API |
| Sensor | HC-SR04 Ultrasonic |


##  How It Works

1. ESP32 connects to WiFi network on startup
2. Ultrasonic sensor measures distance every 300ms
3. Buzzer beeps faster as obstacle gets closer
4. Web dashboard accessible via browser on same WiFi
5. Voice alerts announce danger zones automatically


##  How To Use

1. Flash the code to your ESP32
2. Power on the ESP32 — it connects to WiFi automatically
3. Open Serial Monitor (115200 baud) to get the IP address
4. Connect your phone to the SAME WiFi network as ESP32
5. Open your phone browser and type:
   http://192.168.x.x  (your ESP32 IP)
6. Press "Start Voice Alerts" on the dashboard
7. Wear the glasses and navigate safely 

> ⚠️ Note: The web interface is accessible only on 
> devices connected to the same WiFi network as the ESP32.


## Project Purpose

This project was built with a deep passion for 
**assistive technology** and **accessibility innovation**
The goal is to empower visually impaired individuals 
with an affordable, lightweight, and easy-to-use 
navigation aid that can be worn as glasses.



## Author

**Mariem Ben Salem**
Embedded Systems & IoT Student
📧 mariembensalemx@gmail.com
🔗 linkedin.com/in/mariem-bensalem
📍 Bizerte, Tunisia


This project is open source under the MIT License.
Feel free to use, modify, and improve it! 🙌
