![Platform](https://img.shields.io/badge/platform-Arduino-blue)
![MCU](https://img.shields.io/badge/MCU-ATmega328P-green)
![Display](https://img.shields.io/badge/UI-Nextion-orange)
![License](https://img.shields.io/github/license/Caelivar/Two-Segment-Motion-Control-Project-Arduino-Nextion)

# Two-Segment Motion Control (Arduino + Nextion)
**BEng Course Project - time-constrained motion control with seamless two-segment transition**

> Goal: drive a small vehicle through **two consecutive segments** defined by distance and time, **without stopping between segments**.
<img width="2560" height="1919" alt="image" src="https://github.com/user-attachments/assets/9d13c399-ccd6-46f6-bc62-13073abca2e9" />

## Table of Contents
- [Project Overview](#project-overview)
- [What this project does](#what-this-project-does)
- [Hardware](#hardware)
- [Core Idea](#core-idea)
- [Calibration Mode](#calibration-mode)
- [Notes](#notes)

---

## Project Overview
This project demonstrates a **time-constrained motion control system** using:
- Arduino (Nano / ATmega328P)
- DC motor + L298N driver
- Optical encoder feedback
- Nextion HMI for input + live telemetry

The system executes **two segments back-to-back**:

| Segment | Distance | Time |
|--------:|----------|------|
| 1 | `D1` | `T1` |
| 2 | `D2` | `T2` |

✅ Key requirement: **reach the target distance within the given time** and **transition to segment 2 without pausing**.

---

## What this project does
- Reads motion parameters from a **Nextion touchscreen**
- Drives a **DC motor using PWM**
- Measures real speed & distance via **optical encoder**
- Compensates for **non-linear motor behaviour**
- Displays live status:
  - progress
  - speed (current / target)
  - distance
  - time
  - voltage
- Smoothly transitions between two motion segments

---

## Hardware
Current setup:

- **Microcontroller:** Arduino Nano (ATmega328P)  
- **Motor driver:** L298N H-bridge  
- **Motor:** DC motor (12–24 W, unknown manufacturer)  
- **Encoder:** Optical wheel (10 holes)  
- **Wheel diameter:** 6.1 cm  
- **Display:** Nextion NX3224K024  
- **Voltage sensing:** ADC on A0 (voltage divider)

---


## Core Idea
<img width="3840" height="2160" alt="Page 1" src="https://github.com/user-attachments/assets/377337f6-da7f-4a39-b74d-b6a2a059b7d9" />

Instead of directly calculating the “correct PWM” from distance and time, the algorithm:
### 1) Measures real motion
- **Distance** from encoder pulses  
- **Speed** from **Timer1 Input Capture** (hardware timing)

### 2) Continuously recalculates required speed
At every update step:

```c
required_speed = remaining_distance / remaining_time; 
```
### 3) Compensates for non-linear motor behaviour
The DC motor does **not** respond linearly to PWM duty.

Small changes in duty cycle often produce **no change in speed**, while larger jumps suddenly switch the motor to another stable speed level.

To handle this, a **calibration table** is used:

- PWM duty → measured real speed
- Values are obtained experimentally using encoder feedback
- The table represents how the motor actually behaves, not how it *should* behave

This allows the control logic to work in the **speed domain**, instead of blindly adjusting PWM.
<img width="3840" height="2160" alt="Page 2" src="https://github.com/user-attachments/assets/d43c4625-fbf2-4df3-a121-d452f8a069f7" />
### 4) Uses duty mixing
Even with calibration, many target speeds lie **between** two achievable motor speeds.

Instead of forcing the motor to one of them, the system uses **duty mixing**:

- Two neighbouring PWM duties are selected from the calibration table  
- The motor is driven with these two duties in a defined ratio  
- The **average speed** converges to the desired target speed

This approach:
- Avoids oscillations
- Reduces sensitivity to motor dead zones
- Provides smoother motion without complex control theory (PID)

The result is **quasi-analog speed control** on top of a highly non-linear motor.

---
## Calibration Mode
A dedicated firmware mode was created to experimentally identify real motor behaviour.

Can be found on this branch: https://github.com/Caelivar/Two-Segment-Motion-Control-Project-Arduino-Nextion/tree/Data-Collection

Calibration procedure:
- Sweep PWM duty values across the usable range  
- Measure real speed using the encoder and Timer1  
- Display measured speed and duty on the Nextion screen  

The resulting data is manually transferred into the **motor calibration table** used by the main control firmware.

This ensures the control algorithm is based on **measured reality**, not assumptions.

---

## Notes
- This project is intended for **educational purposes**
- Emphasis is placed on **robustness and observability**, not optimal control theory
- The approach demonstrates how real-world hardware often violates ideal assumptions

You are free to:
- reuse the code  
- modify the algorithms  
- adapt the system for other motion-control tasks  

If you have more questions, feel free to discord me: 
```c
.caelivar
```
---

