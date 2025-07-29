# 📋 Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/).

## [1.0.0] - 2025-07-29

### ✨ Added
- **Automatic twilight switch** with VEML7700 sensor
- **2N2222 transistor control** for radio activation
- **Differentiated threshold system** (30 lux on / 50 lux off)
- **LED visual indicators** with different patterns for each state
- **Visual timer** with countdown to next check
- **Complete serial monitor** with emojis and detailed information
- **Retry system** for robust sensor initialization
- **Battery optimization** with USB-independent operation
- **5-second blinks** to confirm system activity

### 🔧 Technical Features
- Light check every 60 seconds
- 20 lux hysteresis to prevent oscillations
- I2C support for VEML7700 sensor
- Compatible with ESP8266 (Wemos D1 Mini)
- Optimized consumption for solar power

### 📊 System States
- **Startup**: 3 blinks of 200ms
- **Measurement**: 1 blink of 100ms  
- **Activity**: 3 blinks of 100ms every 5s
- **Sensor error**: 2 fast blinks
- **Critical error**: 5 continuous blinks

### 🌡️ Operating Ranges
- **Very dark**: 0-10 lux (Night)
- **Dark**: 10-50 lux (Twilight) 
- **Indoor**: 50-200 lux
- **Bright**: 200-1000 lux
- **Sunlight**: >1000 lux (Day)

### ⚙️ Configuration
- Turn on threshold: 30.0 lux
- Turn off threshold: 50.0 lux  
- Check interval: 60000ms
- Control pin: GPIO16
- LED builtin pin: GPIO2

## [Unreleased] - Future Features

### 🚀 Planned
- [ ] **WiFi connectivity** for remote monitoring
- [ ] **Web interface** for configuration
- [ ] **Historical charts** of luminosity
- [ ] **Low battery alerts** via email/SMS
- [ ] **Calendar control** with custom schedules
- [ ] **Multiple sensors** and control zones
- [ ] **Hibernation mode** for ultra-low power
- [ ] **MQTT integration** for home automation
- [ ] **OTA Updates** for remote updates
- [ ] **SD card storage** for historical data

---

## 🏷️ Version Format

- **[Major.Minor.Patch]** - Date in YYYY-MM-DD format
- **Major**: Breaking API changes
- **Minor**: New backward-compatible functionality  
- **Patch**: Backward-compatible bug fixes

## 📝 Types of Changes

- **✨ Added** - for new features
- **🔧 Changed** - for changes in existing functionality
- **⚠️ Deprecated** - for features that will be removed
- **🗑️ Removed** - for removed features
- **🐛 Fixed** - for bug fixes
- **🔒 Security** - for security vulnerabilities

---

## 🌍 Other Languages

- [Español (Spanish)](CHANGELOG.es.md) - Versión en español
