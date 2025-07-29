# 📋 Changelog

Todos los cambios notables de este proyecto serán documentados en este archivo.

El formato está basado en [Keep a Changelog](https://keepachangelog.com/es-ES/1.0.0/),
y este proyecto adhiere al [Versionado Semántico](https://semver.org/lang/es/).

## [1.0.0] - 2025-07-29

### ✨ Agregado
- **Interruptor crepuscular automático** con sensor VEML7700
- **Control de transistor 2N2222** para activación de radio
- **Sistema de umbrales diferenciados** (30 lux encendido / 50 lux apagado)
- **Indicaciones visuales LED** con diferentes patrones para cada estado
- **Temporizador visual** con cuenta regresiva hasta próxima verificación
- **Monitor serie completo** con emojis y información detallada
- **Sistema de reintentos** para inicialización robusta del sensor
- **Optimización para batería** con funcionamiento independiente del USB
- **Parpadeos cada 5 segundos** para confirmar actividad del sistema

### 🔧 Características Técnicas
- Verificación de luz cada 60 segundos
- Histéresis de 20 lux para evitar oscilaciones
- Soporte I2C para sensor VEML7700
- Compatible con ESP8266 (Wemos D1 Mini)
- Consumo optimizado para energía solar

### 📊 Estados del Sistema
- **Inicio**: 3 parpadeos de 200ms
- **Medición**: 1 parpadeo de 100ms  
- **Actividad**: 3 parpadeos de 100ms cada 5s
- **Error sensor**: 2 parpadeos rápidos
- **Error crítico**: 5 parpadeos continuos

### 🌡️ Rangos de Funcionamiento
- **Muy oscuro**: 0-10 lux (Noche)
- **Oscuro**: 10-50 lux (Crepúsculo) 
- **Interior**: 50-200 lux
- **Brillante**: 200-1000 lux
- **Luz solar**: >1000 lux (Día)

### ⚙️ Configuración
- Umbral de encendido: 30.0 lux
- Umbral de apagado: 50.0 lux  
- Intervalo de verificación: 60000ms
- Pin de control: GPIO16
- Pin LED builtin: GPIO2

## [Unreleased] - Funcionalidades Futuras

### 🚀 Planificado
- [ ] **Conectividad WiFi** para monitoreo remoto
- [ ] **Interface web** de configuración
- [ ] **Gráficos históricos** de luminosidad
- [ ] **Alertas de batería baja** por email/SMS
- [ ] **Control por calendario** con horarios personalizados
- [ ] **Múltiples sensores** y zonas de control
- [ ] **Modo hibernación** para ultra bajo consumo
- [ ] **Integración MQTT** para domótica
- [ ] **OTA Updates** para actualizaciones remotas
- [ ] **Almacenamiento en SD** de datos históricos

---

## 🏷️ Formato de Versiones

- **[Major.Minor.Patch]** - Fecha en formato YYYY-MM-DD
- **Major**: Cambios incompatibles en la API
- **Minor**: Nueva funcionalidad compatible hacia atrás  
- **Patch**: Correcciones de bugs compatibles

## 📝 Tipos de Cambios

- **✨ Agregado** - para nuevas funcionalidades
- **🔧 Cambiado** - para cambios en funcionalidad existente
- **⚠️ Deprecado** - para funcionalidades que serán removidas
- **🗑️ Removido** - para funcionalidades removidas
- **🐛 Arreglado** - para corrección de bugs
- **🔒 Seguridad** - para vulnerabilidades corregidas
