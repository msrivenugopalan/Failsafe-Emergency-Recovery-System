# FAILSAFE EMERGENCY RECOVERY SYSTEM

## Project Overview
The failsafe emergency recovery system is an embedded systems project designed to demonstrate real-time autonomous navigation, adaptive decision-making, and remote manual control.
The system integrates sensor-based obstacle detection, dynamic motor control, wireless communication, and data logging to create a robust and intelligent robotic platform.

---

## Key Objectives
- Design an autonomous navigation system with obstacle avoidance
- Implement a failsafe recovery mechanism for emergency scenarios
- Log operational data for analysis and debugging using SD card module
- Enable dual-mode operation: AUTO and MANUAL
- Provide real-time monitoring through OLED display
- Allow remote control via WiFi-based web interface

---

## Core Features

### Autonomous Mode (AUTO)
- Continuous distance monitoring using ultrasonic sensor
- Adaptive speed control based on obstacle proximity:
  - ≤ 10 cm → Emergency STOP
  - 10–30 cm → Reverse + directional recovery
  - > 30 cm → Normal forward motion
- Intelligent pivot-based obstacle avoidance

### Manual Mode (MANUAL)
- Mobile/web-based joystick control interface
- Real-time motor direction control via ESP32 SoftAP server
- Immediate override of autonomous behavior

### Monitoring & Feedback
- OLED display for live system status
- Real-time distance visualization
- Mode and state updates (SAFE / WARNING / STOP / MANUAL)

### Data Logging
- SD card-based data storage system
- Logs timestamped distance readings for analysis and debugging

---

## Hardware Architecture

- ESP32 Dev Module (Core Controller)
- HC-SR04 Ultrasonic Sensor (Distance Measurement)
- SH1106 OLED Display (UI Feedback)
- L298N Motor Driver (Motor Control Unit)
- DC Motors (Locomotion System)
- Micro SD Card Module (Data Logging)

---

## Connectivity, Control and features

### WiFi Configuration
The ESP32 operates in Soft Access Point mode:
SSID: ESP32_ROBOT
Password: 12345678

### Control Interface
Access the control dashboard via browser:
http://192.168.4.1


### Features:
- Touch-based joystick control
- Mode switching (AUTO / MANUAL)
- Real-time movement response

---

## System Logic Summary

| Distance Range | System Response          |
|----------------|--------------------------|
| ≤ 10 cm        | Emergency Stop           |
| 10–30 cm       | Reverse + Pivot Recovery |
| > 30 cm        | Forward Motion           |

---

## System Workflow and Circuit Diagram
1. ESP32 initializes sensors, OLED, and WiFi
2. Ultrasonic sensor continuously measures distance
3. System selects AUTO or MANUAL mode
4. Motor actions are executed based on decision logic
5. Data is logged into SD card in real time
6. OLED updates system status continuously
<img width="907" height="732" alt="Failsafe emergency recovery system" src="https://github.com/user-attachments/assets/4a856eee-9352-4883-9969-4c025af4f7a2" />
(Basic circuit without OLED and driver module)
<img width="899" height="1599" alt="image" src="https://github.com/user-attachments/assets/3657687e-c9da-4f9b-857d-cc5c7a00c250" />
(Final project output)
---

## Real-World Applications
- Autonomous mobile robots
- Smart surveillance bots
- Industrial inspection systems
- Educational robotics platforms
- IoT-based control systems

---

## Future Improvements
- RTOS-based task scheduling for better performance
- Camera integration for vision-based navigation
- Machine learning-based obstacle prediction
- Mobile app control interface (instead of web UI)
- Cloud-based logging and analytics

---

## Author
Srivenugopalan M
Embedded Systems & IoT Developer  
Focused on automation and real-time embedded systems.

---
