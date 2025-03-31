# MediBox Project

Welcome to the **MediBox** repository! This project demonstrates an IoT-based alarm and environmental monitoring system using the Wokwi simulator. The system integrates functionalities like time zone configuration, alarm management, real-time NTP synchronization, and temperature/humidity monitoring with clear user interaction through an OLED display and push button controls.

---

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Hardware & Software Requirements](#hardware--software-requirements)
- [Project Structure](#project-structure)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Future Enhancements](#future-enhancements)
- [License](#license)

---

## Overview

The MediBox project offers an innovative solution for managing medication schedules by integrating:
- **Time Zone Configuration:** Adjust the system time using a UTC offset.
- **Alarm Management:** Set, view, and delete up to two alarms.
- **NTP Time Synchronization:** Fetch current time from an NTP server over Wi-Fi.
- **Real-Time Alerts:** Ring an alarm with visual and auditory indicators.
- **Environmental Monitoring:** Track temperature and humidity, providing warnings if values exceed healthy limits.

This project is built with a forward-thinking approach to IoT design, leveraging the Wokwi simulator for rapid prototyping and testing.

---

## Features

1. **Menu-Driven Interface:**
   - **Set Time Zone:** Input a UTC offset to configure local time.
   - **Set Alarms:** Create up to 2 alarms.
   - **View Active Alarms:** Display currently active alarm settings.
   - **Delete Alarm:** Remove a specific alarm by selection.

2. **Real-Time Clock:**
   - **NTP Integration:** Fetch time from an NTP server via Wi-Fi.
   - **OLED Display:** Show the current time alongside alarm and sensor data.

3. **Alarm Functionality:**
   - **Ring Alarm:** Trigger an audible buzzer and visual notification when the alarm time is reached.
   - **Stop/Snooze:** Use a push button to stop the alarm or snooze it for 5 minutes.

4. **Environmental Monitoring:**
   - **Temperature & Humidity:** Continuously monitor environmental conditions.
   - **Health Warnings:** Alerts for temperature outside 24°C ≤ T ≤ 32°C and humidity outside 65% ≤ H ≤ 80%.

---

## Hardware & Software Requirements

- **Wokwi Simulator:** Utilize Wokwi for development and simulation.
- **Microcontroller:** Compatible with Arduino (ESP32 recommended for Wi-Fi functionality).
- **Sensors:** 
  - Temperature and Humidity Sensor (e.g., DHT series).
- **Display:** OLED display (e.g., 128x64).
- **Input Device:** Push button for alarm control (stop/snooze).
- **Output Device:** Buzzer for alarm sound.

*Note:* When moving from simulation to hardware, ensure all sensor and display libraries (e.g., `Adafruit_SSD1306`, `DHT`) are installed and configured.

---

## Project Structure

MediBox/ ├── MediBox.ino // Main Arduino sketch ├── README.md // Project documentation └── Additional_Files/ // Libraries and configuration files (if any)


- **MediBox.ino:** Contains the complete code with functions to:
  - Configure time zone and alarms.
  - Fetch and display time from the NTP server.
  - Ring alarms and handle push button inputs.
  - Monitor and alert based on temperature and humidity thresholds.

---

## Getting Started

1. **Clone the Repository:**
   ```bash
   git clone https://github.com/yourusername/MediBox.git
   ```
2. **Open in Wokwi:**

    Import the project files into the Wokwi simulator.

3. **Install Required Libraries:**

    Ensure you have the libraries for the OLED display and the DHT sensor.

4. **Configure Wi-Fi Credentials:**

    Update the SSID and password in the code if you are using a Wi-Fi-enabled microcontroller.

5. **Set NTP Server Settings:**

    Optionally adjust the NTP server address (default is pool.ntp.org).

# Usage
1. **System Initialization:**

     - On boot, the system will connect to Wi-Fi and fetch the current time via NTP.
   
     - The OLED display shows a welcome message followed by the main menu.

2. **Menu Navigation:**

     - Set Time Zone: Input the desired UTC offset (e.g., +5.5 for IST).
   
     - Set Alarms: Enter up to two alarms using hours and minutes.

3. **View Active Alarms: Display all currently active alarms.**

     - Delete Alarm: Remove a chosen alarm from the list.
   
     - Alarm Operation:

4. **When an alarm time is reached, the buzzer sounds and a visual alert is displayed.**

     - Use the push button to stop or snooze the alarm for 5 minutes.
   
     - Environmental Monitoring:
   
     - The OLED continuously displays the current temperature and humidity.

     - Alerts are generated if the temperature goes beyond 24°C to 32°C or humidity goes beyond 65% to 80%.

# Future Enhancements
1. **Data Logging:**

     - Incorporate storage for logging temperature, humidity, and alarm events.

2. **Remote Control:**

     - Develop mobile or web applications for remote monitoring and alarm control.

3. **Voice Integration:**

     - Integrate with voice assistants like Alexa or Google Assistant.

4. **Advanced Notification System:**

     - Implement SMS or email notifications for critical alerts.

# License
This project is licensed under the MIT License. Feel free to use, modify, and distribute this project as per the terms of the license.

Thank you for exploring the MediBox project! We welcome feedback, contributions, and suggestions to further improve and innovate this IoT solution. Stay ahead, keep coding, and push the boundaries of what's possible.
