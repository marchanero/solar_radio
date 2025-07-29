# 💻 Examples and Reference Code

## 🔍 Basic VEML7700 Sensor Reading

```cpp
#include <Adafruit_VEML7700.h>

Adafruit_VEML7700 veml = Adafruit_VEML7700();

void setup() {
  Serial.begin(9600);
  
  if (!veml.begin()) {
    Serial.println("Sensor not found");
    while (1);
  }
  
  Serial.println("VEML7700 initialized");
}

void loop() {
  float lux = veml.readLux();
  float als = veml.readALS();
  
  Serial.print("Lux: ");
  Serial.print(lux);
  Serial.print(" | ALS: ");
  Serial.println(als);
  
  delay(1000);
}
```

## 🔌 2N2222 Transistor Control

```cpp
const int TRANSISTOR_PIN = 16; // GPIO16

void setup() {
  pinMode(TRANSISTOR_PIN, OUTPUT);
  digitalWrite(TRANSISTOR_PIN, LOW); // Start OFF
}

void turnOnRadio() {
  digitalWrite(TRANSISTOR_PIN, HIGH);
  Serial.println("Radio ON");
}

void turnOffRadio() {
  digitalWrite(TRANSISTOR_PIN, LOW);
  Serial.println("Radio OFF");
}
```

## 💡 LED Blinking System

```cpp
const int LED_PIN = 2; // Built-in LED

void blinkLED(int times, int duration_ms) {
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_PIN, LOW);  // Turn ON (inverted on ESP8266)
    delay(duration_ms);
    digitalWrite(LED_PIN, HIGH); // Turn OFF
    if (i < times - 1) {
      delay(duration_ms);
    }
  }
}

void startupPattern() {
  blinkLED(3, 200); // 3 slow blinks
}

void errorPattern() {
  blinkLED(5, 100); // 5 fast blinks
}
```

## ⏰ Non-Blocking Timer

```cpp
unsigned long previous_time = 0;
const unsigned long INTERVAL = 60000; // 60 seconds

void loop() {
  unsigned long current_time = millis();
  
  if (current_time - previous_time >= INTERVAL) {
    previous_time = current_time;
    
    // Execute periodic task
    performMeasurement();
  }
  
  // Other non-blocking tasks
  checkButtons();
  updateDisplay();
}
```

## 🌅 Twilight Switch Logic

```cpp
const float TURN_ON_THRESHOLD = 30.0;
const float TURN_OFF_THRESHOLD = 50.0;
bool current_state = false;

void evaluateLight(float lux) {
  if (!current_state && lux <= TURN_ON_THRESHOLD) {
    // Turn on when dark
    current_state = true;
    turnOnDevice();
    Serial.println("DUSK: Turning ON");
    
  } else if (current_state && lux >= TURN_OFF_THRESHOLD) {
    // Turn off when bright
    current_state = false;
    turnOffDevice();
    Serial.println("DAWN: Turning OFF");
  }
  
  // Hysteresis prevents oscillations between 30-50 lux
}
```

## 🔄 Robust Retry System

```cpp
bool initializeSensorWithRetries(int max_attempts = 3) {
  for (int attempt = 1; attempt <= max_attempts; attempt++) {
    Serial.print("Attempt ");
    Serial.print(attempt);
    Serial.print("/");
    Serial.println(max_attempts);
    
    if (veml.begin()) {
      Serial.println("✅ Sensor initialized");
      return true;
    }
    
    Serial.println("❌ Sensor error");
    blinkLED(2, 100); // Indicate error
    delay(1000);
  }
  
  Serial.println("🚨 Critical sensor error");
  return false;
}
```

## 📊 Light Level Classification

```cpp
enum LightLevel {
  VERY_DARK,     // 0-10 lux
  DARK,          // 10-50 lux  
  INDOOR,        // 50-200 lux
  BRIGHT,        // 200-1000 lux
  DIRECT_SUN     // >1000 lux
};

LightLevel classifyLight(float lux) {
  if (lux < 10) return VERY_DARK;
  if (lux < 50) return DARK;
  if (lux < 200) return INDOOR;
  if (lux < 1000) return BRIGHT;
  return DIRECT_SUN;
}

String getDescription(LightLevel level) {
  switch (level) {
    case VERY_DARK: return "VERY DARK (NIGHT)";
    case DARK: return "DARK (TWILIGHT)";
    case INDOOR: return "INDOOR/CLOUDY";
    case BRIGHT: return "BRIGHT LIGHT";
    case DIRECT_SUN: return "DIRECT SUNLIGHT";
    default: return "UNKNOWN";
  }
}
```

## 🕐 Time Formatting

```cpp
void showTime(unsigned long milliseconds) {
  unsigned long seconds = milliseconds / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;
  
  seconds = seconds % 60;
  minutes = minutes % 60;
  
  if (hours > 0) {
    Serial.print(hours);
    Serial.print("h ");
  }
  
  if (minutes < 10) Serial.print("0");
  Serial.print(minutes);
  Serial.print(":");
  if (seconds < 10) Serial.print("0");
  Serial.print(seconds);
}

void showCountdownTimer(unsigned long time_remaining) {
  Serial.print("⏰ Next check in: ");
  showTime(time_remaining);
  Serial.println(" (mm:ss)");
}
```

## 🛡️ millis() Overflow Protection

```cpp
bool timeElapsed(unsigned long &previous_time, unsigned long interval) {
  unsigned long current_time = millis();
  
  // Handle overflow (every ~49 days)
  if (current_time < previous_time) {
    previous_time = current_time; // Reset on overflow
  }
  
  if (current_time - previous_time >= interval) {
    previous_time = current_time;
    return true;
  }
  
  return false;
}

// Usage:
unsigned long last_blink = 0;
const unsigned long BLINK_INTERVAL = 5000;

void loop() {
  if (timeElapsed(last_blink, BLINK_INTERVAL)) {
    blinkLED(1, 100);
  }
}
```

## 🔧 Dynamic Threshold Configuration

```cpp
struct LightConfiguration {
  float turn_on_threshold;
  float turn_off_threshold;
  unsigned long interval_ms;
  bool debug_active;
};

LightConfiguration config = {
  .turn_on_threshold = 30.0,
  .turn_off_threshold = 50.0,
  .interval_ms = 60000,
  .debug_active = true
};

void adjustSensitivity(String mode) {
  if (mode == "high") {
    config.turn_on_threshold = 10.0;
    config.turn_off_threshold = 30.0;
    Serial.println("High sensitivity mode");
    
  } else if (mode == "low") {
    config.turn_on_threshold = 50.0;
    config.turn_off_threshold = 100.0;
    Serial.println("Low sensitivity mode");
    
  } else {
    config.turn_on_threshold = 30.0;
    config.turn_off_threshold = 50.0;
    Serial.println("Normal sensitivity mode");
  }
}
```

## 📈 Performance Monitoring

```cpp
struct SystemStats {
  unsigned long cycles_completed;
  unsigned long sensor_errors;
  unsigned long state_changes;
  unsigned long total_on_time;
  unsigned long startup_time;
};

SystemStats stats = {0};

void initializeStats() {
  stats.startup_time = millis();
}

void showStats() {
  unsigned long uptime = millis() - stats.startup_time;
  
  Serial.println("📊 SYSTEM STATISTICS");
  Serial.print("⏱️ Uptime: ");
  showTime(uptime);
  Serial.println();
  
  Serial.print("🔄 Cycles completed: ");
  Serial.println(stats.cycles_completed);
  
  Serial.print("⚠️ Sensor errors: ");
  Serial.println(stats.sensor_errors);
  
  Serial.print("🔀 State changes: ");
  Serial.println(stats.state_changes);
  
  float on_percentage = (stats.total_on_time * 100.0) / uptime;
  Serial.print("💡 Radio on time: ");
  Serial.print(on_percentage, 1);
  Serial.println("%");
}
```

## 🔍 I2C Scanner for Diagnostics

```cpp
void scanI2C() {
  Serial.println("Scanning I2C devices...");
  
  int devices_found = 0;
  
  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("Device found at 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      
      // Identify known devices
      if (address == 0x10) Serial.print(" (VEML7700)");
      
      Serial.println();
      devices_found++;
    }
  }
  
  if (devices_found == 0) {
    Serial.println("❌ No I2C devices found");
  } else {
    Serial.print("✅ Found ");
    Serial.print(devices_found);
    Serial.println(" devices");
  }
}
```

---

## 🚀 Advanced Extensions

### WiFi and Web Server

```cpp
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "YourWiFi";
const char* password = "YourPassword";

ESP8266WebServer server(80);

void setupWiFi() {
  WiFi.begin(ssid, password);
  
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.print("Connected. IP: ");
  Serial.println(WiFi.localIP());
}

void setupServer() {
  server.on("/", []() {
    String html = "<h1>Twilight Switch</h1>";
    html += "<p>Current Lux: " + String(veml.readLux()) + "</p>";
    html += "<p>Radio State: " + String(radio_on ? "ON" : "OFF") + "</p>";
    server.send(200, "text/html", html);
  });
  
  server.begin();
  Serial.println("Web server started");
}
```

### Data Storage

```cpp
#include <EEPROM.h>

struct PersistentConfig {
  float turn_on_threshold;
  float turn_off_threshold;
  unsigned long interval;
  uint16_t checksum;
};

void saveConfig(PersistentConfig config) {
  config.checksum = calculateChecksum(config);
  EEPROM.put(0, config);
  EEPROM.commit();
  Serial.println("Configuration saved");
}

bool loadConfig(PersistentConfig &config) {
  EEPROM.get(0, config);
  
  if (config.checksum == calculateChecksum(config)) {
    Serial.println("Configuration loaded");
    return true;
  }
  
  Serial.println("Corrupted configuration, using defaults");
  return false;
}
```

---

💡 **Tip**: These examples can be combined and adapted according to your project's specific needs.
