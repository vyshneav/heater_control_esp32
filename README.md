


---

# 🔥 ESP32 Heater Control System with FreeRTOS

This project demonstrates a simple heater control system using an **ESP32**, **DHT22 temperature sensor**, **relay**, **LCD (I2C)**, **two buttons**, and **FreeRTOS**. The system adjusts heating based on the current temperature and a user-defined target temperature.

---

## 🚀 Features

* 🧠 **FreeRTOS-based Multitasking**

  * Independent tasks for temperature monitoring, LCD display, and button input
* 🌡️ **DHT22 Sensor** for precise temperature readings
* 🔘 **Two Buttons** to adjust the target temperature up/down
* 📟 **I2C LCD Display** shows real-time temperature, target temperature, and system state
* ⚙️ **State Machine** with 5 states: `IDLE`, `HEATING`, `STABILIZING`, `TARGET_REACHED`, `OVERHEAT`

---

## 📦 Components Used

| Component       | Description               |
| --------------- | ------------------------- |
| ESP32           | Microcontroller           |
| DHT22           | Temperature sensor        |
| Relay Module    | Simulates heater control  |
| LCD I2C (16x2)  | Display temperature/state |
| Buttons (x2)    | Adjust target temperature |
| Wokwi Simulator | For online testing        |

---

## 🔌 Wiring

| Component   | ESP32 Pin |
| ----------- | --------- |
| DHT22       | GPIO 2    |
| Relay IN    | GPIO 18   |
| Button Up   | GPIO 4    |
| Button Down | GPIO 5    |
| LCD SDA     | GPIO 21   |
| LCD SCL     | GPIO 22   |

---

## 🧩 System Behavior

| Temperature vs. Target | Relay | System State    |
| ---------------------- | ----- | --------------- |
| Below Target - 1°C     | ON    | HEATING         |
| Within ±0.5°C          | OFF   | TARGET\_REACHED |
| Above Target + 2°C     | OFF   | OVERHEAT        |
| Slight variations      | OFF   | STABILIZING     |

---

## 🛠️ How It Works

* **FreeRTOS Tasks**:

  * `TaskTemperature`: Reads temperature every 2s and updates system state
  * `TaskButton`: Monitors button presses to change target temperature
  * `TaskLCD`: Displays updated values with minimal flicker
* **Semaphore**:

  * Used to avoid display conflicts between tasks

---

## 🧪 Testing

Tested using the **Wokwi Simulator**. To simulate:

1. Upload code to [https://wokwi.com/](https://wokwi.com/)
2. Select ESP32 as the board
3. Wire components as per instructions

---

## 🖼️ Example LCD Display

```
Temp: 24.5°C
Set: 25.0 HEAT
```

---

## 📌 To Do

* Add Wi-Fi + MQTT for remote control and monitoring
* Add persistent storage (EEPROM) for target temperature
* Add buzzer alert for overheat state

---

