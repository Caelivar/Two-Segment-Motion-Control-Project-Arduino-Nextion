# ⚙️ Mechatronics Semester Project 1 – D/E25

## 🎯 Project Overview
The goal of this project is to integrate multiple hardware and software modules on the **ATmega328P (Arduino Nano)** platform to control and monitor a DC motor.  
Communication with the **Nextion LCD Display** is performed over UART, and all measurements and controls are combined into a single working system.

---

<img width="3840" height="2160" alt="Page 1" src="https://github.com/user-attachments/assets/377337f6-da7f-4a39-b74d-b6a2a059b7d9" />



## 👩‍💻 Individual Tasks

### **Ario**
- Display **two numbers** on the Nextion Display via UART.
- One number must be an **integer**, and the other a **floating-point value** with decimal places.
- Start testing with the **Nextion Debugger**, then verify on the **actual display**.

### **Natalia**
- Read an **integer input** from the Nextion Display via UART.
- Start with the **Nextion Debugger**, then finalize on the **physical display**.

### **Maks**
- Measure **time between two holes** on the encoder wheel using the **optocoupler**.
- Output the measured **time period** to the serial terminal.

### **Andrii**
- Configure and read voltage from **ADC0 (PC0)** pin.
- Acceptable input range: **0–5 V**.
- Display the measured voltage value in the **serial terminal** (for now).

### **Milena**
- Configure **PWM output** on **PD5 or PD6**.
- Set the PWM **frequency** and **duty cycle to 20%**.
- Connect output to **L298 motor driver** and observe **motor speed change**.

### **Amanda**
- Implement **button press detection** from the Nextion display (e.g. a “Start” button).
- When pressed, begin system operation (e.g. measure and display motor speed).

### **Haseeb**
- Integrate all modules to:
  - **Measure** and **calculate motor speed**.
  - **Display** the speed value on the **Nextion LCD**.

---

## 🔌 Hardware Connections

| Component          | Arduino Pin | Description |
|--------------------|--------------|-------------|
| Optocoupler (ICP)  | **PB0 (D8)** | Input from encoder wheel |
| Motor driver (L298) | **PD5 / PD6** | PWM control output |
| Voltage sensor     | **PC0 (ADC0)** | Analog input (0–5 V) |
| Nextion Display    | **PD0 / PD1** | UART TX/RX |

---

## 🧠 Programming Tasks

1. **Enable UART Communication** with Nextion LCD  
   - Test with Nextion Debugger before connecting physical display.

2. **Implement PWM Output** to control motor speed  
   - Set initial duty cycle to **20%**.

3. **Enable Optocoupler Timing**  
   - Measure time between encoder pulses to determine RPM.

4. **Enable ADC Reading** from **PC0**  
   - Read and later display battery voltage.

---

## 🚀 System Integration (Overall Goal)

- Get the **motor running** and control its **speed programmatically**.  
- **Measure rotational speed** using the **optocoupler + encoder wheel**.  
- **Display** both **motor speed (RPM)** and **voltage level** on the **Nextion LCD**.  
- System should respond to the **“Start” button** on the Nextion screen.

