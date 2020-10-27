<!-- effect=fireworks -->

## #ArduinoOnRails

@saramic

---

<!-- fg=white bg=red -->

## #ArduinoOnRails

@saramic

---

## What is Arduino?

---

## What is Arduino?

- Open-source electronic prototyping platform enabling users to create
  interactive electronic objects.

---

> let's take a step back

---

## What is Rails?

- A web-application framework that includes everything needed to create
  database-backed web applications according to the Model-View-Controller
  (MVC).

---

## What is Rails?

- a "computer" who's interface are REST end points?

---

> back to arduino

---

## What is Arduino?

- a "computer" who's interface is voltages

---

## What is Arduino?

- read voltage on inputs
- set voltages on outputs
- perform computations based on inputs

---

## Arduino inputs

- **Inputs**
  - analog voltage 0-5 volts
    - light meter
    - temperature sensor
    - microphone
  - serial data
    - GPS
    - RFID reader
    - real time clock

---

## Arduino outputs

- **Outputs**
  - digital low 0 volts and high 5 volts
    - turn on an LED
    - turn on a relay
  - PWM (Pulse Width Modulation)
    - dim an LED
    - play sound
    - change speed of a motor
  - serial data
    - drive an OLED display
    - drive a servo

---

## Show me

---

### Blink

```
 1: void setup() {
 2:   pinMode(LED_BUILTIN, OUTPUT);
 3: }
 4:
 5: void loop() {
 6:   digitalWrite(LED_BUILTIN, HIGH);
 7:   delay(200);
 8:   digitalWrite(LED_BUILTIN, LOW);
 9:   delay(200);
10: }
```

---

### Blink - improved

```
 1: const int period = 200;
 2: long lastChangeTime = 0;
 3:
 4: void setup() {
 5:   pinMode(LED_BUILTIN, OUTPUT);
 6: }
 7:
 8: void blink() {
 9:   long now = millis();
10:   if (now > lastChangeTime + period) {
11:     ledState = !ledState;
12:     digitalWrite(ledPin, ledState);
13:     lastChangeTime = now;
14:   }
15: }
16:
17: void loop() {
18:   blink();
19:   delay(20);
20: }
```

---

### PWM

```
 0% (0)     25% (64)                50% (128)
            --          --          -----        ----
           |  |        |  |        |     |      |    |
           |  |        |  |        |     |      |    |
           |  |        |  |        |     |      |    |
           |  |        |  |        |     |      |    |
           |  |        |  |        |     |      |    |
           |  |        |  |        |     |      |    |
 ----------    --------    --------       ------      -----
```

```
 75% (192)               100% (255)
  --------    ------    -----------------------------------
 |        |  |      |  |
 |        |  |      |  |
 |        |  |      |  |
 |        |  |      |  |
 |        |  |      |  |
 |        |  |      |  |
           --        --
```

--- 

### Pulse

```
 1: void loop() {
 2:   long now = millis();
 3:   int fadeModulus = ((now / delay) % 512);
 4:   int fadeOffset = fadeModulus - 255;
 5:   int fadeAmount = abs(fadeOffset);
 6:   analogWrite(ledPin, fadeAmount);
 7:   delay(20);
 8: }
```

---

### Tune

```
 1: void loop() {
 2:   tone(speakerPin, 440, 1000);
 3:   delay(300);
 4:   noTone(speakerPin);
 5: }
```

---

### Temperature

```
 1: void setup() {
 2:   Serial.begin(9600);
 3: }
 4:
 5: void loop() {
 6:   sensorValue = analogRead(analogInPin);
 7:   // 0.489 = 500/1023
 8:   //   10mV for the range 0 - 5V
 9:   //   via 10bit ADC 2^10 1,024 0 - 1,023
10:   float temp_val = analogRead(LM335_pin) * 0.489;
11:
12:   // Convert Kelvin to degree Celsius
13:   float temp_val_celsius = temp_val - 273.16;
14:   Serial.print(temp_val_celsius);
15:   Serial.println(" Degree Celsius");
16:   delay(20);
17: }
```

---

### Button

```
 1: void setup() {
 2:   pinMode(2, INPUT_PULLUP);
 3: }
 4:
 5: void loop() {
 6:   int sensorVal = digitalRead(2);
 7:
 8:   if (sensorVal == HIGH) {
 9:     // switch ON
10:   } else {
11:     // switch OFF
12:   }
13: }
```

---

### Button - with debounce and interrupts

```
 1: const int debounceDelay = 100;
 2: long lastChangeTime = 0;
 3: long lastDebounceTime = 0;
 4: int lastSwitchState = HIGH;
 5: 
 6: void setup() {
 7:   pinMode(2, INPUT_PULLUP);
 8:   attachInterrupt(digitalPinToInterrupt(2), change, CHANGE);
 9: }
10: 
11: void change() {
12:   int switchState = digitalRead(switchPin);
13:   if (switchState != lastSwitchState) {
14:     lastDebounceTime = millis();
15:   }
16:   if ((millis - lastDebounceTime) > debounceDelay) {
17:     if (switchState != lastSwitchState) {
18:       lastSwitchState = switchState;
19:       if (switchState == LOW) {
20:         changeState();
21:       }
22:     }
23:   }
24: }
25: 
26: void loop() {
27:   // do other stuff
28: }
```

---

### Servo

```
 1: #include <Servo.h>
 2: Servo myservo;
 3: #define SWEEP_PERIOD 1000
 4:
 5: void setup() {
 6:   myservo.attach(9);
 7: }
 8:
 9: void loop() {
10:   int millisPosition = millis() % SWEEP_PERIOD;
11:   double floatPosition = TWO_PI * (((float) millisPosition ) / SWEEP_PERIOD);
12:   int servoPosition = (70 * sin(floatPosition)) + 90;
13:   Serial.println(servoPosition);
14:   myservo.write(servoPosition);
15:   delay(20);
16: }
```

---

## What else can Arduino do?

- real time
- timers
- interrupts
- Serial communications: I2C, 1-WIre, SPI, UART
- USB
- bit manipulation

---

## Flavours of Arduino

- Arduino Uno
  - 16Mhz
  - 14 digital I/O (inputs/outputs)
  - 6 analog inputs
  - 32kb of flash memory to hold your program.
- Arduino Due
  - 84Mhz
  - 54 digital I/O
  - 12 analog inputs
  - 512k flash memory
- Arduino Leonardo, Leostick, Arduino pro mini, many cheap imitations

---

## Even more Arduino like

- ESP32 (160Mhz) with WiFi and Bluetooth
- ESP8266 (80Mhz) with WiFi and Bluetooth
- Adafruit FONA - with GSM
- Arduino nano 33 BLE sense - Machine learning with Tiny ML
- NodeMCU
- Particle

**NOT** _Raspberry Pi, micro:bit, OrangeCrab FPGA (Field Programmable Gate
  Array), arm MBED, Atmel AVR, ARM, Microchip PIC, 8051_

---

## But what about #ArduinoOnRails?

_rails is ruby and Arduino is C++ in 32kb_

---

## Some attempts ...

---

## ESP32 has Wifi

Generate a model

```
rails generate model Counter name:string count:bigint
```

find or create and increment a default counter

```
 1: class Counter < ApplicationRecord
 2:   def self.increment_default_counter
 3:     Counter
 4:       .find_or_create_by(name: 'default')
 5:       .increment!(:count)
 6:   end
 7:
 8:   def to_s
 9:     count.to_s
10:   end
11: end
```

---

click counter middleware

```
cat app/middleware/click_counter.rb

 1: def call(env)
 2:   @status, @headers, @response = @app.call(env)
 3:   @count = Counter.increment_default_counter
 4:   @headers.merge!('X-Request-Count' => @count.to_s)
 5:   [@status, @headers, self]
 6: end
 7:
 8: def each(&block)
 9:   block.call("<!-- Counter: #{@count} -->\n") if @headers['Content-Type'].include?('text/html')
10:   @response.each(&block)
11: end
```

configure rails to use the middleware

```
 1: class Application < Rails::Application
 2:    ...
 3:    config.middleware.use ClickCounter
 4: end
```

---

take a look at the home page and the `/counters` endpoint

http://bit.ly/roroOct2020

---

get a bunch of libs

```
 1: #include <WiFi.h>
 2: #include <HTTPClient.h>
 3: #include <WiFiClientSecure.h>
 4: #include <ArduinoJson.h>
```

and the Certificate Authority for your https certificate

```
 1: // Heroku certified by DigiCert
 2: // DigiCert High Assurance EV Root CA
 3: // https://www.digicert.com/kb/digicert-root-certificates.htm
 4: // Serial #: 02:AC:5C:26:6A:0B:40:9B:8F:0B:79:F2:AE:46:25:77
 5: static const char DIGICERT_CA_CERTIFICATE[] PROGMEM = R"EOF(
 6: -----BEGIN CERTIFICATE-----
 7: MIIDxTCCAq2gAwIBAgIQAqxcJmoLQJuPC3nyrkYldzANBgkqhkiG9w0BAQUFADBs
 8: MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
 9: ...
10: Yzi9RKR/5CYrCsSXaQ3pjOLAEFe4yHYSkVXySGnYvCoCWw9E1CAx2/S6cCZdkGCe
11: vEsXCS+0yx5DaMkHJ8HSXPfqIbloEpw8nL+e/IBcm2PN7EeqJSdnoDfzAIJ9VNep
12: +OkuE6N36B9K
13: -----END CERTIFICATE-----
14: )EOF";
```

---

and a bunch of WiFi, connect, json parse, code

let's see the counter in action!

---

## #ArduinoOnRails, You promissed!

---

Well what if we made every request go through arduino by getting rack
middleware to contact a vanilla arduino via the serial port?

---

Maybe next time?

---

## Is it just a toy?

_from my examples, YES_

---

## Cloud Scale

weighing scale integration via AWS IoT

1. read a scale
1. publish data to AWS IoT cloud as MQTT
1. subscribe a website via websockets
1. real time updates from scale to website


Maybe next time?

---

<!-- effect=matrix -->

---

## Next steps

- get a cheap Arduino Uno kit with sensors _~$20_
- videos -> http://bit.ly/roroOct2020
- Join me for Electronc Wings Context 2020
  - submit a hardware project by 15th Dec 2020

---

## Thank you

- @saramic (github, twitter)
- #ArduinoOnRails (twitter)
- slides, links and code
  - http://bit.ly/roroOct2020

