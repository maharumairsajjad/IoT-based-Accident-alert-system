# IoT-based-Accident-alert-system

# 🚨 Real-Time Vibration & Motion Monitoring System with GPS

An ESP32-based embedded system for monitoring vibrations, motion, and location in real-time. This project utilizes the **SW420 vibration sensor**, **MPU6050 accelerometer/gyroscope**, **GPS module**, and an **OLED display** to collect and display sensor data, and uploads the results to **Firebase**.

---

## 🛠️ Features

- 📶 **WiFi Connectivity** for cloud communication  
- 🎯 **MPU6050** to track motion (acceleration and gyroscope data)  
- 📍 **GPS Module** for real-time location (latitude, longitude)  
- 🔔 **SW420 Vibration Sensor** to detect physical shock or vibration  
- 📟 **OLED Display (128x64 SSD1306)** for feedback  
- 🔊 **Buzzer Alerts** for user notifications  
- ☁️ **Firebase** integration for cloud data logging

---

## ⚙️ Hardware Used

| Component              | Details                          |
|------------------------|----------------------------------|
| ESP32 Dev Module       | WiFi + Bluetooth microcontroller |
| SW420 Vibration Sensor | Digital vibration detection      |
| MPU6050                | Accelerometer + Gyroscope        |
| GPS Module (NEO-6M)    | UART GPS receiver                |
| OLED SSD1306           | I2C 128x64 pixel display         |
| Buzzer                 | Passive buzzer for alerts        |

---

## 🔌 Pin Configuration

| Peripheral        | ESP32 Pin |
|------------------|-----------|
| SW420 Sensor     | GPIO12    |
| Buzzer           | GPIO32    |
| GPS RX (to ESP TX)| GPIO17   |
| GPS TX (to ESP RX)| GPIO16   |
| OLED Display (I2C)| SDA/SCL   |

---

## 📲 Firebase Integration

You must configure your Firebase Realtime Database:

1. Create a new Firebase project.
2. Enable Realtime Database (set to test mode for development).
3. Get the database URL and add it to the following line in your code:
   ```cpp
   const char* firebaseUrl = "https://your-database.firebaseio.com/data.json";
