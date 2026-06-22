
# Arduino Smart Car (Exhibition Project)

An intelligent, obstacle-avoiding, and environment-aware smart car built on Arduino. The vehicle features Bluetooth manual override control, automated color-based traffic stopping, automatic headlights for darkness detection, and dual ultrasonic sensors for direction-aware collision prevention.

---

## 🚀 Key Features
* **Direction-Aware Obstacle Avoidance:** Uses front and rear ultrasonic sensors to block movement if an obstacle is detected within 5 cm in the direction of travel.
* **Automatic Traffic Light Stopping:** Leverages an RGB color sensor to automatically stop the vehicle if a red surface/light is detected.
* **Intelligent Headlights:** Uses a hardware interrupt routine to instantly trigger headlights when ambient light drops below a safe threshold.
* **Bluetooth Remote Control:** Supports manual serial commands (`f`, `b`, `l`, `r`, `s`) via a Bluetooth module interface.

---

## 🛠️ Components Used
* **Microcontroller:** Arduino Uno / Nano (or compatible ATmega328P board)
* **Motor Driver:** L298N Dual H-Bridge Motor Driver Module and two BO motors and one castor wheel to put at front
* **Distance Sensors:** HC-SR04 Ultrasonic Sensors (x2)
* **Color Sensor:** Adafruit TCS34725 RGB Color Sensor
* **Headlights:** LEDs (x2)
* **Wireless Module:** HC-05 Bluetooth Module
* **Prototyping:** Mini Breadboard (for power distribution)
* **Batteries : ** Li-ion batteries and its holder
* **Resistors : ** For bluetooth voltage divider circuit and appropriate resistors for LEDs

---

## 🔌 Wiring Reference Matrix

### 1. Power & Breadboard Distribution (Safe Hardware Isolation)
> ⚠️ **Important Setup Note:** Use a mini breadboard to create a common 5V power rail and a common GND rail. Connect the Arduino's 5V pin and GND pin to separate rows on the breadboard to distribute power easily to all components.

| From (Component Pin) | To (Target Pin / Rail) | Description |
| :--- | :--- | :--- |
| **Battery Positive (+)** | **L298N 12V Power In** | High-current power direct to the motors |
| **Battery Negative (-)** | **L298N GND** | Main ground return line |
| **L298N GND** | **Arduino GND (explained below as GND bridging)** | **Common Ground Connection** (Crucial for signal reference) |
| **L298N 5V Out** | **Arduino 5V Pin** | Logic power supply from the driver module to power the Arduino board |
| **Arduino 5V** | **Breadboard Positive Rail**| Establishes the 5V power row for all sensors |
| **Arduino GND** | **Breadboard Negative Rail**| Establishes the ground row for all components |
| **Sensors VCC (All)** | **Breadboard Positive Rail**| Powers the logic sensors |
| **Sensors GND (All)** | **Breadboard Negative Rail**| Completes the sensor ground loop |


> ⭐**GND Bridging**

* **The 3-Way Bridge:** The negative (GND) wire coming out of the battery holder is physically twisted and bridged directly with two separate wires.
* **To Arduino & Driver:** One of those wires connects straight to the **GND pin on the Arduino**, and the other connects straight to the **GND terminal on the L298N motor driver**.


### 2. DC Motors & L298N Driver Connections
| Component Pin | Arduino Pin | Description |
| :--- | :--- | :--- |
| **ENA** | D6 | Speed Control (PWM) - Left Motors |
| **IN1** | D7 | Direction Control - Left Motors |
| **IN2** | D8 | Direction Control - Left Motors |
| **IN3** | D9 | Direction Control - Right Motors |
| **IN4** | D10 | Direction Control - Right Motors |
| **ENB** | D11 | Speed Control (PWM) - Right Motors |

### 3. Sensors & Outputs
| Component Pin | Arduino Pin / Rail | Description |
| :--- | :--- | :--- |
| **Trig 1** (Front) | D3 | Front Ultrasonic Trigger |
| **Echo 1** (Front) | D4 | Front Ultrasonic Echo |
| **Trig 2** (Rear) | A2 | Rear Ultrasonic Trigger |
| **Echo 2** (Rear) | A1 | Rear Ultrasonic Echo |
| **TCS34725 SDA** | SDA (A4) | I2C Data Line |
| **TCS34725 SCL** | SCL (A5) | I2C Clock Line |
| **TCS34725 INT** | D2 | External Interrupt for Darkness |
| **L1 (Headlight 1)** | D12 -> **Anode** (Long Leg) | Left Headlight Control (Cathode/Short leg to GND rail) |
| **L2 (Headlight 2)** | D13 -> **Anode** (Long Leg) | Right Headlight Control (Cathode/Short leg to GND rail) |
| **Bluetooth RX** | TX (D1) | Serial Communication line |
| **Bluetooth TX** | RX (D0) | Serial Communication line |

---

## ⚙️ Calibration Note

* **Color & Darkness Thresholds:** The values for triggering the red light stop and the dark headlights (`LOWT`) are currently preset based on specific testing environment lighting. Depending on your room's ambient lighting, you will need to open the code and adjust these threshold levels accordingly to match your surroundings.

---

## 🎮 Bluetooth Control Guide

Open any smartphone Bluetooth Terminal application, pair with your module, and use these standard controls:
* `f` — Move Forward
* `b` — Move Backward
* `l` — Turn Left
* `r` — Turn Right
* `s` — Hard Stop Motors


### 🔌 Physical Assembly 

#### 1. Chassis setup
* **Lower Deck:** Two yellow BO (Battery Operated) DC motors are mounted at the rear for driving power, balanced by an omnidirectional caster wheel at the front for steering freedom. The heavy battery holder is mounted underneath the chassis to keep the center of gravity low and prevent tipping.
* **Upper Deck:** The Arduino, L298N motor driver, and the mini breadboard are secured on top for easy access and wiring.

### 📶 Bluetooth Module (HC-05/HC-06) Logic Safety Note
* **Arduino TX to Bluetooth RX Line:** Because the Arduino outputs 5V signals and the HC-05 module expects a safer 3.3V logic level on its receiving pin, a quick voltage divider is used to protect the module. 

* **The Resistors:** A **1kΩ resistor** is connected in series from the Arduino TX pin to the HC-05 RX pin. Then, a **2kΩ resistor** one leg is first twisted to 1 kohm resistor and other side to a jumper wire that connect directly to the common GND rail. 

* **Bluetooth TX to Arduino RX Line:** This line is connected straight through without any resistors, as the Arduino safely reads the module's 3.3V transmission as a clear high signal.





   
