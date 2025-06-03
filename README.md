# IoT RFID Attendance System

A microcontroller-based attendance system using RFID and simple input/output components.

## 🔧 Components Used
- 16x2 I2C LCD Display
- Buzzer
- RFID Reader (RC522)
- Push Button
- ESP8266 / Other microcontroller (e.g., NodeMCU) *(no Arduino board used)*

## 📋 Features
- Detects RFID card presence
- Displays user status on 16x2 I2C LCD
- Sends attendance data to a server via Wi-Fi (if connected)
- Buzzer for feedback (success/fail)
- Button used to reset/clear or for mode selection

## 🖼️ System Overview
1. User taps card on RFID reader
2. Data processed and shown on LCD
3. Buzzer gives feedback
4. Data optionally sent to server or logged locally
