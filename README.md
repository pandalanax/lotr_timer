# Lord of the Rings Timer

An Arduino-based countdown timer that plays the Lord of the Rings theme when finished.

This project was vibe coded.

## Components List

*   1x Arduino board (ATmega8 compatible, 3.3V logic)
*   3x Tactile Buttons
*   3x 10kΩ Resistors
*   3x 0.1µF Capacitors
*   1x HT1621 LCD Display Module
*   1x Piezo Buzzer

## Wiring Diagram (Text Description)

**1. Buttons (Minute, Second, Start/Stop)**

Each button uses an **RC Debouncing Circuit**:
*   **Circuit:** 3.3V --- 10kΩ Resistor --- Arduino Pin --- 0.1µF Capacitor --- GND
*   **Button:** Connects Arduino Pin to GND when pressed.

*   **Pin Assignments:**
    *   **Set Minute:** Digital Pin **2** (PD2)
    *   **Set Second:** Digital Pin **3** (PD3)
    *   **Start/Stop:** Digital Pin **4** (PD4)

**2. HT1621 Display**

*   **DATA_PIN:** Digital Pin **11** (PB3)
*   **WR_PIN:** Digital Pin **13** (PB5)
*   **CS_PIN:** Digital Pin **10** (PB2)
*   **VCC:** 3.3V
*   **GND:** GND

**3. Piezo Buzzer**

*   Connect one leg to Digital Pin **12** (PB4).
*   Connect the other leg to **GND**.
