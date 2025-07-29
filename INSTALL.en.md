# 📦 Installation and Configuration Guide

## 🔧 Step-by-Step Installation

### 1. Development Environment Setup

#### Option A: VS Code + PlatformIO (Recommended)

```bash
# Install VS Code
# Download from: https://code.visualstudio.com/

# Install PlatformIO IDE extension
# In VS Code: Extensions → Search "PlatformIO IDE" → Install
```

#### Option B: PlatformIO Core

```bash
# Install Python (if not already installed)
python --version

# Install PlatformIO
pip install platformio

# Verify installation
pio --version
```

### 2. Get Source Code

```bash
# Clone repository
git clone https://github.com/marchanero/solar_radio.git
cd solar_radio

# Or download ZIP from GitHub and extract
```

### 3. Hardware Configuration

#### Required Components

- [ ] Wemos D1 Mini (ESP8266)
- [ ] VEML7700 sensor
- [ ] 2N2222 transistor
- [ ] 1kΩ resistor
- [ ] Breadboard and connection wires
- [ ] 3.7V Li-Ion battery (optional)
- [ ] 6V 1W solar panel (optional)

#### Detailed Connections

```
Wemos D1 Mini          VEML7700 Sensor
GPIO4 (D2) ────────────── SDA
GPIO5 (D1) ────────────── SCL
3.3V ──────────────────── VIN
GND ───────────────────── GND

Wemos D1 Mini          Control Circuit
GPIO16 (D0) ──── 1kΩ ──── Base (2N2222)
3.3V ──────────────────── Collector (2N2222) ──── Radio (+)
GND ───────────────────── Emitter (2N2222) ──── Radio (-)
```

### 4. Build and Upload

#### Using VS Code + PlatformIO

1. Open VS Code
2. File → Open Folder → Select project folder
3. PlatformIO: Build (Ctrl+Alt+B)
4. Connect Wemos D1 Mini via USB
5. PlatformIO: Upload (Ctrl+Alt+U)

#### Using PlatformIO Core

```bash
# Build
pio build

# Upload (with USB cable connected)
pio upload

# Monitor serial output
pio device monitor --baud 9600
```

### 5. Operation Verification

#### First Run

1. **Startup LED**: Should make 3 slow blinks when starting
2. **Serial monitor**: Should display welcome message
3. **Sensor**: Should detect VEML7700 correctly
4. **Measurements**: Should show lux values every 60 seconds

#### LED Diagnosis

| LED Pattern | Status | Action |
|-------------|--------|--------|
| 3 slow blinks | ✅ System OK | Continue |
| 2 fast blinks | ⚠️ Sensor error | Check I2C connections |
| 5 continuous blinks | ❌ Critical error | Check power and connections |

### 6. Initial Configuration

#### Adjust Light Thresholds

Edit in `src/main.cpp`:

```cpp
// For indoor use (less light)
const float UMBRAL_ENCENDIDO = 10.0;   // More sensitive
const float UMBRAL_APAGADO = 30.0;

// For outdoor use (more light)
const float UMBRAL_ENCENDIDO = 50.0;   // Less sensitive
const float UMBRAL_APAGADO = 100.0;
```

#### Change Check Interval

```cpp
// More frequent (uses more battery)
const unsigned long TIEMPO_VERIFICACION = 30000;  // 30 seconds

// Less frequent (saves battery)
const unsigned long TIEMPO_VERIFICACION = 300000; // 5 minutes
```

## 🔋 Solar Power Configuration

### Solar Power Scheme

```
Solar Panel (6V) ──► TP4056 ──► Li-Ion Battery ──► Wemos D1 Mini
                        │                           │
                        └── LED Indicator           └── VEML7700
                                                    └── 2N2222 → Radio
```

### Additional Components for Solar

- **Solar panel**: 6V 1W minimum (recommended 2W)
- **charge controller**: TP4056 with protection
- **Battery**: 18650 Li-Ion 3000mAh or higher
- **Protective case**: IP65 for outdoor use

### Solar Installation

1. **Panel orientation**: South, 30-45° inclination
2. **Sensor location**: No shadows, clear sky view
3. **Protection**: Waterproof case for components
4. **Cables**: As short as possible (max. 2m)

## 🐛 Common Troubleshooting

### Problem: "Sensor not detected"

```bash
# Check I2C connections
# Use I2C scanner to detect devices
```

**Solutions**:
- Check SDA/SCL connected correctly
- Try shorter cables
- Verify 3.3V power to sensor

### Problem: "Won't compile"

**Common error**: Missing libraries

```bash
# Reinstall dependencies
pio lib install

# Clean and rebuild
pio run --target clean
pio run
```

### Problem: "Won't upload code"

**Solutions**:
- Check correct COM port
- Press RESET button during upload
- Try different USB cable

### Problem: "Works with USB but not with battery"

**Solutions**:
- Check battery voltage (min. 3.0V)
- Verify current capacity
- Check power connections

## 📊 Sensor Calibration

### Finding Ideal Thresholds

1. **Monitor for 24h**:

```cpp
// Enable detailed logs
#define DEBUG_MODE 1
```

2. **Note typical values**:
   - Direct sunlight: 1000-10000 lux
   - Cloudy day: 100-1000 lux
   - Indoor lit: 50-200 lux
   - Twilight: 10-50 lux
   - Night: 0-10 lux

3. **Adjust by location**:
   - Urban area (more artificial light): Lower thresholds
   - Rural area (less light): Higher thresholds

## 🚀 Improvements and Customizations

### Add WiFi and Remote Monitoring

```cpp
// Upcoming features
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
```

### Mobile App Control
- Web interface for configuration
- Historical luminosity charts
- Low voltage alerts

### Multiple Outputs
- Control multiple devices
- Custom schedules
- Seasonal profiles

## 📞 Support

### Additional Resources
- **PlatformIO Documentation**: https://docs.platformio.org/
- **VEML7700 Datasheet**: https://www.vishay.com/docs/84286/veml7700.pdf
- **ESP8266 Arduino Core**: https://github.com/esp8266/Arduino

### Community
- **GitHub Issues**: To report bugs
- **Discussions**: For general questions
- **Wiki**: Extended documentation

---

💡 **Tip**: Always test first with USB power before switching to solar battery.

---

## 🌍 Other Languages

- [Español (Spanish)](INSTALL.es.md) - Versión en español
