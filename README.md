# ESP32 Neo Flappy Bird Arcade 🦅🕹️

[![Arduino](https://img.shields.io/badge/Platform-Arduino-blue.svg)](https://www.arduino.cc/)
[![ESP32](https://img.shields.io/badge/Hardware-ESP32-red.svg)](https://www.espressif.com/)
[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](https://opensource.org/licenses/MIT)

A retro arcade-style Flappy Bird game engineered for the **ESP32** microcontroller and a **0.96" SSD1306 OLED Display (I2C)**. Featuring animated dual-frame sprites, 3D shaded pipe rendering, parallax environments, impact particle burst effects, piezo sound feedback, and non-volatile high-score saving.

## Demo Video Link👇
## YouTube👉 https://youtube.com/shorts/ahKLhcGkmJU?si=jI6uiLMJCGdq4imR
## Facebook👉 https://www.facebook.com/share/r/1DhQRuNj9M/
<img width="497" height="572" alt="image" src="https://github.com/user-attachments/assets/ecf5512d-6da3-4c80-aac1-5617d2779405" />

## Connect with TechTadka360💝👇

- YouTube: [@techtadka360official](https://youtube.com/@techtadka360official?si=GdlIntZKv30kPgBk)
- Instagram: [@techtadka360official](https://www.instagram.com/techtadka360official?igsh=cWR4bnhjdWw1MHdh)
- Facebook: [TechTadka360](https://www.facebook.com/share/1EkKAJNLdB/)

---

## 🌟 Features

*   **Arcade-Grade Visuals:** Built with custom-shaded 3D pipe caps, moving cloud layers, and scrolling checkered ground geometry.
*   **Dual-Frame Wing Animation:** Dynamic 8x8 bird sprite that flaps its wings in real-time according to climb and fall velocity.
*   **Collision Particle Burst:** Real-time particle explosion effect upon crash before shifting to the Game Over screen.
*   **Persistent High Score Storage:** Saves your best record directly to the ESP32 internal flash memory using the `Preferences` library (retained even after powering down).
*   **Single-Click Jump Logic:** Debounced button control prevents accidental continuous jumping when holding down the push switch.
*   **Dynamic Sound Synthesis:** Multi-tone audio cues for wing flaps, passing pipes, round progression, and game over.

---

## 🛠️ Hardware Requirements

| Component | Quantity | Purpose |
| :--- | :--- | :--- |
| **ESP32 Development Board** | 1 | Main MCU running game logic, I2C rendering, and audio. |
| **0.96" I2C OLED Display (SSD1306)** | 1 | 128x64 monochrome screen for high-framerate rendering. |
| **Push Switch / Tactile Button** | 1 | Input trigger to control the bird's jump action. |
| **Piezo Buzzer (Passive)** | 1 | Audio feedback for flaps, scores, and crash effects. |
| **Breadboard & Jumper Wires** | Multiple | For prototyping and connections without soldering. |

---

## ⚡ Circuit & Wiring Diagram

The push switch utilizes the ESP32 internal pull-up resistor (`INPUT_PULLUP`), eliminating the need for an external resistor.

```text
               +--------------------------------------+
               |                ESP32                 |
               |                                      |
               |  [3V3]  [GND]   [D21]  [D22]   [D4]  [D18]
               +---|-------|-------|------|------|-----|--+
                   |       |       |      |      |     |
      +------------+       |       |      |      |     |
      |                    |       |      |      |     |
      |      +-------------+       |      |      |     |
      |      |                     |      |      |     |
      |      |   +-----------------+      |      |     |
      |      |   |   +--------------------+      |     |
      |      |   |   |                           |     |
   +--v------v---v---v----+                      |     |
   | VCC    GND SDA SCL   |                      |     |
   |                      |                      |     |
   |  OLED (SSD1306 I2C)  |                      |     |
   +----------------------+                      |     |
                                                 |     |
   +----------------------+                      |     |
   |     PUSH BUTTON      |                      |     |
   |  [PIN 1]     [PIN 2] |                      |     |
   +----|------------|----+                      |     |
        |            +---------------------------+     |
        |                                              |
        +------------+ (ESP32 Common GND)              |
                     |                                 |
   +-----------------v----+                            |
   |    PASSIVE BUZZER    |                            |
   |   (-)            (+) |                            |
   +----|--------------|--+                            |
        |              +-------------------------------+
        v
    (GND Rail)

SUPPORT

If you found this project helpful, consider giving this repository a **⭐ Star**.

Your support helps **TECHTADKA360** create more open-source Arduino, ESP32, IoT, and Robotics projects.
