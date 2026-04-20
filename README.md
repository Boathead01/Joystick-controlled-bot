
#  Joystick Controlled Robot using ESP32

##  Project Overview

This project is a **wireless joystick-controlled robot** built using the **ESP32**, **L298N motor driver**, and **TT DC motors**. The robot can be controlled in real-time using a joystick module, making it a great beginner-to-intermediate project for learning embedded systems, wireless communication, and motor control.

---

##  How It Works

* A **joystick module** captures user input (X and Y axis movement).
* The **ESP32 (transmitter)** reads joystick values and sends control signals wirelessly.
* Another **ESP32 (receiver)** processes these signals.
* The **L298N motor driver** drives the **TT motors** based on received commands.
* The robot moves forward, backward, left, or right depending on joystick direction.

---

##  Components Used

* ESP32 (2 units – transmitter & receiver)
* L298N Motor Driver Module
* TT DC Motors (2 or 4 depending on chassis)
* Robot Chassis
* Joystick Module (Analog)
* Power Supply (Battery Pack)
* Jumper Wires

---

##  Circuit Connections

### Transmitter (Joystick + ESP32)

* Joystick VCC → 3.3V
* Joystick GND → GND
* VRx → GPIO (e.g., 32)
* VRy → GPIO (e.g., 33)

### Receiver (ESP32 + L298N)

* IN1, IN2, IN3, IN4 → ESP32 GPIO pins
* ENA, ENB → PWM pins (optional for speed control)
* Motors → L298N output terminals
* External power → L298N (for motors)

---

##  Communication

* Uses **ESP-NOW / Wi-Fi (depending on your implementation)** for fast, low-latency communication.
* No router required (if using ESP-NOW).

---

##  Controls

| Joystick Movement | Robot Action |
| ----------------- | ------------ |
| Up                | Forward      |
| Down              | Backward     |
| Left              | Turn Left    |
| Right             | Turn Right   |
| Center            | Stop         |

---

## Code

* Written in **Arduino IDE (C/C++)**
* Includes:

  * Joystick reading (ADC)
  * Wireless transmission
  * Motor control logic



##  Future Improvements

* Add speed control using PWM
* Add obstacle detection (Ultrasonic sensor)
* Add camera module for FPV control
* Mobile app control instead of joystick

---

##  What You Learn

* ESP32 wireless communication
* Motor driver interfacing
* Analog signal processing
* Embedded system design basics

---

##  Contributing

Feel free to fork this repo, improve the design, and submit a pull request!

---

##  License

This project is open-source and available under the MIT License.

---

##  Support

If you found this project useful, consider giving it a ⭐ on GitHub!
