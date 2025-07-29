# 🌅 Solar Twilight Switch for Radio Control

An automated radio control system that activates and deactivates based on ambient light conditions, designed to work with solar power.

![Project Status](https://img.shields.io/badge/Status-Functional-brightgreen)
![Platform](https://img.shields.io/badge/Platform-ESP8266-blue)
![Framework](https://img.shields.io/badge/Framework-Arduino-teal)
![License](https://img.shields.io/badge/License-MIT-yellow)

## 📋 Description

This project implements a **smart twilight switch** that automatically controls a radio based on ambient light levels. It uses a VEML7700 sensor to measure luminosity and activates/deactivates the radio through a 2N2222 transistor, ideal for autonomous solar installations.

### ✨ Main Features

- 🌙 **Automatic twilight control**: Turns on radio at dusk (< 30 lux) and turns off at dawn (> 50 lux)
- 🔋 **Battery optimized**: Works independently of serial monitor
- 📊 **Real-time monitoring**: Shows luminosity data, status and timers
- 💡 **Visual indicators**: Built-in LED with different patterns for each state
- ⏰ **Visual timer**: Countdown to next verification
- 🔧 **Automatic retries**: Robust sensor initialization system
- 🎯 **Smart hysteresis**: Prevents oscillations with differentiated thresholds

## 🛠️ Required Hardware

### Main Components
- **Microcontroller**: Wemos D1 Mini (ESP8266)
- **Light sensor**: VEML7700 (I2C communication)
- **Transistor**: 2N2222 (for radio control)
- **Power supply**: 3.7V battery + solar panel (optional)

### Connections

```
Wemos D1 Mini    →    Component
GPIO16 (D0)      →    2N2222 transistor base
GPIO2 (D4)       →    Built-in LED (integrated)
GPIO4 (D2)       →    VEML7700 SDA
GPIO5 (D1)       →    VEML7700 SCL
3.3V             →    VEML7700 VCC
GND              →    VEML7700 GND
```

### Transistor Circuit

```
3.3V ──┬── Radio (+)
       │
      ┌─┴─┐
      │ ⚡ │ Relay/Radio
      └─┬─┘
        │
     ┌──┴──┐
     │ 2N2222
     │  C│E │
     └──┬──┘
        │B
      ┌─┴─┐
      │ R │ 1kΩ
      └─┬─┘
        │
    GPIO16 (D0)
```

## 🚀 Installation

### 1. Environment Setup

```bash
# Install PlatformIO
pip install platformio

# Clone repository
git clone https://github.com/yourusername/solar_radio_wemos.git
cd solar_radio_wemos
```

### 2. Build and Upload

```bash
# Build project
pio build

# Upload to microcontroller
pio upload

# Monitor serial (optional)
pio device monitor
```

### 3. Dependencies

The following libraries are installed automatically:
- `Adafruit VEML7700 Library` v2.1.6
- `Adafruit BusIO` v1.16.1

## 🎛️ Configuration

### Light Thresholds

```cpp
// Configuration in src/main.cpp
const float UMBRAL_ENCENDIDO = 30.0;  // Turn on below 30 lux
const float UMBRAL_APAGADO = 50.0;    // Turn off above 50 lux
```

### Time Intervals

```cpp
const unsigned long TIEMPO_VERIFICACION = 60000;  // Check every 60 seconds
```

## 📊 Operation

### System States

| Light Level | Radio State | Action |
|-------------|-------------|--------|
| < 30 lux    | 🟢 ON       | Night mode active |
| 30-50 lux   | 🔄 NO CHANGE | Hysteresis zone |
| > 50 lux    | 🔴 OFF      | Day mode active |

### Visual Indicators (Built-in LED)

| Pattern | Meaning |
|---------|---------|
| 3 slow blinks (200ms) | 🔊 System starting |
| 1 medium blink (100ms) | 📊 Taking measurement |
| 3 fast blinks (100ms) | ⏰ System active (every 5s) |
| 2 fast blinks | ⚠️ Sensor error (retrying) |
| 5 continuous blinks | ❌ Critical sensor error |

### Serial Monitor Output

```
=== TWILIGHT SWITCH WITH TIMER ===
🌅 ON threshold (twilight): 30.00 lux
🌄 OFF threshold (dawn): 50.00 lux
⏰ Check interval: 60 seconds

🔍 CHECK #1 - 0:01
🔊 BEEP! Taking measurement...
💡 Light intensity: 25.4 lux | ALS: 142
   📊 Level: DARK (TWILIGHT)
   🌅 Current state: Radio OFF
   🔄 Next action: TURN ON (light <= 30.0 lux - TWILIGHT)

🌙 TWILIGHT DETECTED - Turning on radio...
✅ RADIO ON (light: 25.4 <= 30.0 lux)
🎯 STATE CHANGED!
📊 Final state: Radio 🟢 ON
```

## 🔧 Customization

### Modify Sensitivity Thresholds

For higher sensitivity (faster response):
```cpp
const float UMBRAL_ENCENDIDO = 20.0;  // More sensitive to dusk
const float UMBRAL_APAGADO = 40.0;    // More sensitive to dawn
```

For lower sensitivity (more stable):
```cpp
const float UMBRAL_ENCENDIDO = 50.0;  // Less sensitive to dusk
const float UMBRAL_APAGADO = 80.0;    // Less sensitive to dawn
```

### Change Check Intervals

```cpp
const unsigned long TIEMPO_VERIFICACION = 30000;  // Every 30 seconds (more frequent)
const unsigned long TIEMPO_VERIFICACION = 300000; // Every 5 minutes (battery saving)
```

## 🔋 Solar Power Optimization

### Recommended Configuration
- **Solar panel**: 6V 1W minimum
- **Battery**: 3.7V Li-Ion 2000mAh
- **Regulator**: TP4056 with protection
- **Average consumption**: ~80mA in operation

### Installation Tips
1. Orient VEML7700 sensor towards sky (avoid shadows)
2. Protect electronics from moisture
3. Use short cables to minimize losses
4. Check I2C connections (internal pull-ups sufficient)

## 🐛 Troubleshooting

### Problem: Doesn't work with battery
**Solution**: 
- Check battery voltage (min. 3.0V)
- Verify VEML7700 I2C connections
- Observe LED patterns for diagnosis

### Problem: Frequent oscillations
**Solution**: 
- Increase difference between thresholds
- Check light source stability
- Clean VEML7700 sensor

### Problem: Sensor not detected
**Solution**: 
- Check SDA/SCL connections
- Try different I2C addresses
- Reset system

## 📈 Future Development

- [ ] WiFi integration for remote monitoring
- [ ] Web configuration interface
- [ ] Historical data storage
- [ ] Calendar/schedule control
- [ ] Multiple sensor support
- [ ] Advanced low power mode

## 🤝 Contributing

Contributions are welcome. Please:

1. Fork the project
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 📄 License

This project is under the MIT License - see the [LICENSE](LICENSE) file for details.

## 👨‍💻 Author

**Roberto Marchanero**
- GitHub: [@marchanero](https://github.com/marchanero)
- Project: [solar_radio_wemos](https://github.com/marchanero/solar_radio)

## 🙏 Acknowledgments

- [Adafruit](https://www.adafruit.com/) for VEML7700 sensor libraries
- [PlatformIO](https://platformio.org/) for excellent development environment
- ESP8266 community for support and documentation

---

⭐ If this project has been useful to you, consider giving it a star!

---

## 🌍 Other Languages

- [Español (Spanish)](README.es.md) - Versión en español
