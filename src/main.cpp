#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_VEML7700.h>

// GPIO pin to control 2N2222 transistor
// On Wemos D1: D2 = GPIO4, D3 = GPIO0, D4 = GPIO2, etc.
const int RADIO_CONTROL_PIN = 16; // GPIO16 (corresponds to D0 on Wemos D1)

// Built-in LED pin (GPIO2 on Wemos D1)
const int LED_BUILTIN_PIN = 2;

// External LED pin (GPIO13 on Wemos D1)
const int EXTERNAL_LED_PIN = 13; // GPIO13 (corresponds to D7 on Wemos D1)

// VEML7700 sensor object
Adafruit_VEML7700 veml = Adafruit_VEML7700();

// Cycle times (in milliseconds)
const unsigned long CHECK_INTERVAL = 60000;  // Check light every 60 seconds
const unsigned long INITIAL_ON_PERIOD = 120000;  // Keep radio ON for first 2 minutes (120000ms)

// Light thresholds for twilight switch (in lux)
const float TURN_ON_THRESHOLD = 30.0;   // Turn on when below 30 lux (early twilight)
const float TURN_OFF_THRESHOLD = 50.0;  // Turn off when above 50 lux (very early dawn)

// Current radio state
bool radio_on = false;

// Initial period control
unsigned long system_start_time = 0;
bool initial_period_active = true;

// Cycle counter
unsigned long cycle_counter = 0;

// Timer variables
unsigned long last_check_time = 0;
unsigned long next_check_time = 0;

// External LED blink timer (every 30 seconds)
const unsigned long EXTERNAL_LED_BLINK_INTERVAL = 30000; // 30 seconds
unsigned long last_external_blink = 0;

// Function to make LED blink (visual beep)
void blinkLED(int times, int duration_ms) {
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_BUILTIN_PIN, LOW);  // Turn on LED (LOW on ESP8266)
    delay(duration_ms);
    digitalWrite(LED_BUILTIN_PIN, HIGH); // Turn off LED (HIGH on ESP8266)
    if (i < times - 1) {
      delay(duration_ms);
    }
  }
}

// Function for system startup beep
void startupBeep() {
  Serial.println("🔊 BEEP-BEEP! System starting...");
  blinkLED(3, 200); // 3 blinks of 200ms each
}

// Function for measurement beep
void measurementBeep() {
  Serial.println("🔊 BEEP! Taking measurement...");
  blinkLED(1, 100); // 1 short blink of 100ms
}

// Function to control external LED
void setExternalLED(bool state) {
  digitalWrite(EXTERNAL_LED_PIN, state ? HIGH : LOW);
}

// Function for synchronized measurement indicator (built-in + external LED) with 220 resistor
void measurementIndicator() {
  Serial.println("🔊 BEEP! Taking measurement...");
  
  // Blink built-in LED
  blinkLED(1, 100);
  
  // Flash external LED synchronized with measurement
  digitalWrite(EXTERNAL_LED_PIN, HIGH);
  delay(100);
  digitalWrite(EXTERNAL_LED_PIN, LOW);
}

// Function for external LED double blink (status indicator)
void externalLEDDoubleBlink() {
  // First blink
  digitalWrite(EXTERNAL_LED_PIN, HIGH);
  delay(200); // Longer duration for better visibility
  digitalWrite(EXTERNAL_LED_PIN, LOW);
  delay(150); // Short pause between blinks
  
  // Second blink
  digitalWrite(EXTERNAL_LED_PIN, HIGH);
  delay(200); // Longer duration for better visibility
  digitalWrite(EXTERNAL_LED_PIN, LOW);
}

// Function to format time in mm:ss
void showTime(unsigned long milliseconds) {
  unsigned long seconds = milliseconds / 1000;
  unsigned long minutes = seconds / 60;
  seconds = seconds % 60;
  
  if (minutes < 10) Serial.print("0");
  Serial.print(minutes);
  Serial.print(":");
  if (seconds < 10) Serial.print("0");
  Serial.print(seconds);
}

// Function to show timer until next check
void showTimer() {
  unsigned long current_time = millis();
  
  if (next_check_time > current_time) {
    unsigned long remaining_time = next_check_time - current_time;
    Serial.print("⏰ Next check in: ");
    showTime(remaining_time);
    Serial.println(" (mm:ss)");
  } else {
    Serial.println("⏰ Check in progress...");
  }
}

// Function to read and display light sensor data
void showLightData() {
  float lux = veml.readLux();
  float als = veml.readALS();
  
  Serial.print("💡 Light intensity: ");
  Serial.print(lux);
  Serial.print(" lux | ALS: ");
  Serial.println(als);
  
  // Classify light level
  if (lux < 10) {
    Serial.println("   📊 Level: VERY DARK (NIGHT)");
  } else if (lux < 50) {
    Serial.println("   📊 Level: DARK (TWILIGHT)");
  } else if (lux < 200) {
    Serial.println("   📊 Level: INDOOR/CLOUDY");
  } else if (lux < 1000) {
    Serial.println("   📊 Level: BRIGHT LIGHT");
  } else {
    Serial.println("   📊 Level: DIRECT SUNLIGHT (DAY)");
  }
  
  // Show twilight switch state
  Serial.print("   🌅 Current state: Radio ");
  if (radio_on) {
    Serial.println("ON");
  } else {
    Serial.println("OFF");
  }
  
  // Show expected next action
  if (radio_on && lux >= TURN_OFF_THRESHOLD) {
    Serial.print("   🔄 Next action: TURN OFF (light >= ");
    Serial.print(TURN_OFF_THRESHOLD);
    Serial.println(" lux - DAWN)");
  } else if (!radio_on && lux <= TURN_ON_THRESHOLD) {
    Serial.print("   🔄 Next action: TURN ON (light <= ");
    Serial.print(TURN_ON_THRESHOLD);
    Serial.println(" lux - TWILIGHT)");
  } else {
    Serial.println("   ✅ Correct state for current conditions");
  }
}

void setup() {
  // Configure pins FIRST (before everything else)
  pinMode(RADIO_CONTROL_PIN, OUTPUT);
  pinMode(LED_BUILTIN_PIN, OUTPUT);
  pinMode(EXTERNAL_LED_PIN, OUTPUT);
  
  // Initialize LED off and radio off
  digitalWrite(LED_BUILTIN_PIN, HIGH); // HIGH = off on ESP8266
  digitalWrite(RADIO_CONTROL_PIN, LOW); // Ensure radio off at startup
  digitalWrite(EXTERNAL_LED_PIN, LOW); // External LED off initially
  
  // Pause for system stabilization
  delay(2000); // Increased for better battery stabilization
  
  // Startup beep (works without Serial)
  startupBeep();
  
  // Initialize serial communication for debugging
  Serial.begin(9600);
  delay(500); // Pause to establish serial connection
  
  Serial.println("=== TWILIGHT SWITCH WITH TIMER ===");
  Serial.println("Automatic radio control with VEML7700 sensor");
  Serial.print("🚀 INITIAL ON period: ");
  Serial.print(INITIAL_ON_PERIOD / 1000);
  Serial.println(" seconds (radio stays ON regardless of light)");
  Serial.print("🌅 TURN ON threshold (twilight): ");
  Serial.print(TURN_ON_THRESHOLD);
  Serial.println(" lux");
  Serial.print("🌄 TURN OFF threshold (dawn): ");
  Serial.print(TURN_OFF_THRESHOLD);
  Serial.println(" lux");
  Serial.print("⏰ Check interval: ");
  Serial.print(CHECK_INTERVAL / 1000);
  Serial.println(" seconds");
  Serial.println("🔊 Built-in LED configured for visual indicators");
  Serial.println("💡 External LED (GPIO13) configured for status indication");
  Serial.print("   - Fixed ON during initial 2-minute period");
  Serial.println();
  Serial.print("   - Double blink every ");
  Serial.print(EXTERNAL_LED_BLINK_INTERVAL / 1000);
  Serial.println(" seconds during normal operation");
  Serial.println("   - Single flash with each measurement");
  Serial.println();
  
  // Initialize I2C with retries for better robustness
  Wire.begin();
  delay(100);
  
  // Initialize VEML7700 sensor with retries
  bool sensor_initialized = false;
  for (int attempt = 1; attempt <= 3; attempt++) {
    if (veml.begin()) {
      sensor_initialized = true;
      Serial.println("✅ VEML7700 sensor initialized correctly");
      break;
    } else {
      Serial.print("⚠️ Attempt ");
      Serial.print(attempt);
      Serial.println("/3 - Error initializing VEML7700 sensor");
      // Error blink (2 fast blinks)
      blinkLED(2, 100);
      delay(1000);
    }
  }
  
  if (!sensor_initialized) {
    Serial.println("❌ CRITICAL ERROR: Could not initialize VEML7700 sensor!");
    // Critical error blink (5 fast blinks)
    while (1) {
      blinkLED(5, 200);
      delay(2000);
    }
  }
  
  // Initialize radio in ON state for initial period
  digitalWrite(RADIO_CONTROL_PIN, HIGH);  // Turn radio ON initially
  digitalWrite(EXTERNAL_LED_PIN, HIGH);   // Turn external LED ON for initial period
  radio_on = true;
  system_start_time = millis();  // Record system start time
  initial_period_active = true;
  
  Serial.println("✅ Radio initialized in ON state for initial 2-minute period");
  Serial.println("💡 External LED turned ON for initial period");
  Serial.print("⏰ Initial period will last: ");
  Serial.print(INITIAL_ON_PERIOD / 1000);
  Serial.println(" seconds");
  
  // Take initial reading to show conditions (but don't act on them yet)
  Serial.println("\n📊 Evaluating initial conditions (radio will stay ON for 2 minutes)...");
  measurementBeep(); // Use simple beep during initial setup (external LED is already ON)
  showLightData();
  
  // Initialize timer
  last_check_time = millis();
  next_check_time = last_check_time + CHECK_INTERVAL;
  last_external_blink = millis(); // Initialize external LED blink timer
  showTimer();
  
  Serial.println("================================\n");
}

void loop() {
  unsigned long current_time = millis();
  
  // Check if we're still in the initial 2-minute period
  if (initial_period_active) {
    unsigned long elapsed_time = current_time - system_start_time;
    
    if (elapsed_time >= INITIAL_ON_PERIOD) {
      // Initial period has ended
      initial_period_active = false;
      Serial.println("\n🎯 INITIAL 2-MINUTE PERIOD COMPLETED!");
      Serial.println("📡 Radio will now follow automatic twilight switch logic");
      Serial.println("💡 External LED will now sync with measurements");
      
      // Turn off external LED (it will now flash with measurements)
      setExternalLED(false);
      
      Serial.println("================================");
      
      // Take a reading to determine what state we should be in now
      measurementIndicator(); // Use new synchronized function
      float current_lux = veml.readLux();
      showLightData();
      
      // Apply normal twilight logic immediately
      if (current_lux <= TURN_ON_THRESHOLD) {
        Serial.println("🌙 Light level indicates radio should stay ON");
        // Radio is already ON, so no change needed
      } else if (current_lux >= TURN_OFF_THRESHOLD) {
        Serial.println("🌅 Light level indicates radio should turn OFF");
        digitalWrite(RADIO_CONTROL_PIN, LOW);
        radio_on = false;
        Serial.println("✅ RADIO OFF (transitioning from initial period)");
      }
      
      // Reset timer for normal operation
      last_check_time = current_time;
      next_check_time = current_time + CHECK_INTERVAL;
      last_external_blink = current_time; // Reset external LED blink timer
      Serial.println("-------------------\n");
      
    } else {
      // Still in initial period - show countdown every 10 seconds
      static unsigned long last_countdown = 0;
      if (current_time - last_countdown >= 10000) {
        last_countdown = current_time;
        
        unsigned long remaining_time = INITIAL_ON_PERIOD - elapsed_time;
        Serial.print("🚀 INITIAL PERIOD: Radio staying ON for ");
        showTime(remaining_time);
        Serial.println(" more (mm:ss)");
        Serial.println("   💡 External LED: ON (fixed during initial period)");
        
        // Show current light conditions for reference
        float current_lux = veml.readLux();
        Serial.print("   💡 Current light: ");
        Serial.print(current_lux);
        Serial.print(" lux (");
        if (current_lux < 10) {
          Serial.print("VERY DARK");
        } else if (current_lux < 50) {
          Serial.print("DARK");
        } else if (current_lux < 200) {
          Serial.print("INDOOR/CLOUDY");
        } else if (current_lux < 1000) {
          Serial.print("BRIGHT");
        } else {
          Serial.print("DIRECT SUNLIGHT");
        }
        Serial.println(")");
        
        // Blink to show system is active
        blinkLED(1, 100);
      }
    }
    
    // Small pause and return (skip normal twilight logic during initial period)
    delay(100);
    return;
  }
  
  // NORMAL OPERATION: Check if it's time for a new verification
  if (current_time >= next_check_time) {
    // Increment check counter
    cycle_counter++;
    
    // Update times for next cycle
    last_check_time = current_time;
    next_check_time = current_time + CHECK_INTERVAL;
    
    // Show check information
    Serial.print("🔍 CHECK #");
    Serial.print(cycle_counter);
    Serial.print(" - ");
    
    // Get current time (simulated with millis)
    unsigned long hours = (current_time / 3600000) % 24;
    unsigned long minutes = (current_time / 60000) % 60;
    Serial.print(hours);
    Serial.print(":");
    if (minutes < 10) Serial.print("0");
    Serial.println(minutes);
    
    // Read current light level
    measurementIndicator(); // Use synchronized measurement indicator
    float current_lux = veml.readLux();
    showLightData();
    Serial.println();
    
    // TWILIGHT SWITCH LOGIC
    bool state_changed = false;
    
    if (!radio_on && current_lux <= TURN_ON_THRESHOLD) {
      // CONDITION: TURN ON (Twilight/Night)
      Serial.println("🌙 TWILIGHT DETECTED - Turning on radio...");
      digitalWrite(RADIO_CONTROL_PIN, HIGH);
      radio_on = true;
      state_changed = true;
      
      Serial.print("✅ RADIO ON (light: ");
      Serial.print(current_lux);
      Serial.print(" <= ");
      Serial.print(TURN_ON_THRESHOLD);
      Serial.println(" lux)");
      
    } else if (radio_on && current_lux >= TURN_OFF_THRESHOLD) {
      // CONDITION: TURN OFF (Dawn/Day)
      Serial.println("🌅 DAWN DETECTED - Turning off radio...");
      digitalWrite(RADIO_CONTROL_PIN, LOW);
      radio_on = false;
      state_changed = true;
      
      Serial.print("✅ RADIO OFF (light: ");
      Serial.print(current_lux);
      Serial.print(" >= ");
      Serial.print(TURN_OFF_THRESHOLD);
      Serial.println(" lux)");
      
    } else {
      // NO CHANGES - Maintain current state
      if (radio_on) {
        Serial.print("🔄 Radio staying ON (night continues - ");
        Serial.print(current_lux);
        Serial.print(" lux < ");
        Serial.print(TURN_OFF_THRESHOLD);
        Serial.println(" lux)");
      } else {
        Serial.print("🔄 Radio staying OFF (day continues - ");
        Serial.print(current_lux);
        Serial.print(" lux > ");
        Serial.print(TURN_ON_THRESHOLD);
        Serial.println(" lux)");
      }
    }
    
    // Show current state summary
    if (state_changed) {
      Serial.println("🎯 STATE CHANGED!");
    }
    
    Serial.print("📊 Final state: Radio ");
    if (radio_on) {
      Serial.println("🟢 ON");
    } else {
      Serial.println("🔴 OFF");
    }
    
    // Show timer for next check
    showTimer();
    Serial.println("-------------------");
    Serial.println();
    
  } else {
    // Show timer every 5 seconds while waiting
    static unsigned long last_timer = 0;
    if (current_time - last_timer >= 5000) {
      last_timer = current_time;
      
      // Visual blink every 5 seconds to indicate system is active
      Serial.println("🔊 BEEP! System active - checking timer...");
      blinkLED(1, 100); // 1 blink of 100ms
      
      Serial.print("💤 Waiting... ");
      showTimer();
      
      // Show current radio state
      Serial.print("   📊 Radio currently: ");
      if (radio_on) {
        Serial.println("🟢 ON");
      } else {
        Serial.println("🔴 OFF");
      }
    }
    
    // External LED double blink every 30 seconds (only during normal operation)
    if (!initial_period_active && (current_time - last_external_blink >= EXTERNAL_LED_BLINK_INTERVAL)) {
      last_external_blink = current_time;
      Serial.println("💡 External LED: Double blink status indicator");
      externalLEDDoubleBlink();
    }
  }
  
  // Small pause to not overload the processor
  delay(100);
}
