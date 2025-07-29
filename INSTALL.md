# 📦 Guía de Instalación y Configuración

## 🔧 Instalación Paso a Paso

### 1. Preparación del Entorno de Desarrollo

#### Opción A: VS Code + PlatformIO (Recomendado)
```bash
# Instalar VS Code
# Descargar desde: https://code.visualstudio.com/

# Instalar extensión PlatformIO IDE
# En VS Code: Extensions → Buscar "PlatformIO IDE" → Install
```

#### Opción B: PlatformIO Core
```bash
# Instalar Python (si no está instalado)
python --version

# Instalar PlatformIO
pip install platformio

# Verificar instalación
pio --version
```

### 2. Obtener el Código Fuente

```bash
# Clonar el repositorio
git clone https://github.com/marchanero/solar_radio.git
cd solar_radio

# O descargar ZIP desde GitHub y extraer
```

### 3. Configuración del Hardware

#### Componentes Necesarios
- [ ] Wemos D1 Mini (ESP8266)
- [ ] Sensor VEML7700
- [ ] Transistor 2N2222
- [ ] Resistencia 1kΩ
- [ ] Protoboard y cables de conexión
- [ ] Batería 3.7V Li-Ion (opcional)
- [ ] Panel solar 6V 1W (opcional)

#### Conexiones Detalladas

```
Wemos D1 Mini          VEML7700 Sensor
GPIO4 (D2) ────────────── SDA
GPIO5 (D1) ────────────── SCL
3.3V ──────────────────── VIN
GND ───────────────────── GND

Wemos D1 Mini          Circuito de Control
GPIO16 (D0) ──── 1kΩ ──── Base (2N2222)
3.3V ──────────────────── Colector (2N2222) ──── Radio (+)
GND ───────────────────── Emisor (2N2222) ──── Radio (-)
```

### 4. Compilación y Carga

#### Usando VS Code + PlatformIO
1. Abrir VS Code
2. File → Open Folder → Seleccionar carpeta del proyecto
3. PlatformIO: Build (Ctrl+Alt+B)
4. Conectar Wemos D1 Mini por USB
5. PlatformIO: Upload (Ctrl+Alt+U)

#### Usando PlatformIO Core
```bash
# Compilar
pio build

# Cargar (con cable USB conectado)
pio upload

# Monitorear salida serie
pio device monitor --baud 9600
```

### 5. Verificación del Funcionamiento

#### Primera Ejecución
1. **LED de inicio**: Debe hacer 3 parpadeos lentos al arrancar
2. **Monitor serie**: Debe mostrar el mensaje de bienvenida
3. **Sensor**: Debe detectar el VEML7700 correctamente
4. **Mediciones**: Debe mostrar valores de lux cada 60 segundos

#### Diagnóstico por LED
| Patrón LED | Estado | Acción |
|------------|--------|--------|
| 3 parpadeos lentos | ✅ Sistema OK | Continuar |
| 2 parpadeos rápidos | ⚠️ Error sensor | Verificar conexiones I2C |
| 5 parpadeos continuos | ❌ Error crítico | Revisar alimentación y conexiones |

### 6. Configuración Inicial

#### Ajustar Umbrales de Luz
Editar en `src/main.cpp`:
```cpp
// Para interior (menos luz)
const float UMBRAL_ENCENDIDO = 10.0;   // Más sensible
const float UMBRAL_APAGADO = 30.0;

// Para exterior (más luz)
const float UMBRAL_ENCENDIDO = 50.0;   // Menos sensible
const float UMBRAL_APAGADO = 100.0;
```

#### Cambiar Intervalo de Verificación
```cpp
// Más frecuente (gasta más batería)
const unsigned long TIEMPO_VERIFICACION = 30000;  // 30 segundos

// Menos frecuente (ahorra batería)
const unsigned long TIEMPO_VERIFICACION = 300000; // 5 minutos
```

## 🔋 Configuración para Energía Solar

### Esquema de Alimentación Solar

```
Panel Solar (6V) ──► TP4056 ──► Batería Li-Ion ──► Wemos D1 Mini
                        │                           │
                        └── LED Indicador           └── VEML7700
                                                    └── 2N2222 → Radio
```

### Componentes Adicionales para Solar
- **Panel solar**: 6V 1W mínimo (recomendado 2W)
- **Controlador de carga**: TP4056 con protección
- **Batería**: 18650 Li-Ion 3000mAh o superior
- **Caja protectora**: IP65 para exterior

### Instalación Solar
1. **Orientación del panel**: Sur, inclinación 30-45°
2. **Ubicación del sensor**: Sin sombras, vista despejada del cielo
3. **Protección**: Caja estanca para componentes
4. **Cables**: Lo más cortos posible (máx. 2m)

## 🐛 Solución de Problemas Comunes

### Problema: "Sensor no detectado"
```bash
# Verificar conexiones I2C
# Usar scanner I2C para detectar dispositivos
```
**Soluciones**:
- Verificar SDA/SCL conectados correctamente
- Probar con cables más cortos
- Verificar alimentación 3.3V del sensor

### Problema: "No compila"
**Error común**: Librerías faltantes
```bash
# Reinstalar dependencias
pio lib install

# Limpiar y recompilar
pio run --target clean
pio run
```

### Problema: "No carga el código"
**Soluciones**:
- Verificar puerto COM correcto
- Presionar botón RESET durante la carga
- Probar con cable USB diferente

### Problema: "Funciona con USB pero no con batería"
**Soluciones**:
- Verificar voltaje batería (mín. 3.0V)
- Comprobar capacidad de corriente
- Revisar conexiones de alimentación

## 📊 Calibración del Sensor

### Encontrar Umbrales Ideales

1. **Monitorear durante 24h**:
```cpp
// Habilitar logs detallados
#define DEBUG_MODE 1
```

2. **Anotar valores típicos**:
   - Luz solar directa: 1000-10000 lux
   - Día nublado: 100-1000 lux
   - Interior iluminado: 50-200 lux
   - Crepúsculo: 10-50 lux
   - Noche: 0-10 lux

3. **Ajustar según ubicación**:
   - Zona urbana (más luz artificial): Umbrales más bajos
   - Zona rural (menos luz): Umbrales más altos

## 🚀 Mejoras y Personalizaciones

### Añadir WiFi y Monitoreo Remoto
```cpp
// Próximas funcionalidades
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
```

### Control por Aplicación Móvil
- Interface web para configuración
- Gráficos de luminosidad histórica
- Alertas por bajo voltaje

### Múltiples Salidas
- Control de varios dispositivos
- Horarios personalizados
- Perfiles estacionales

## 📞 Soporte

### Recursos Adicionales
- **Documentación PlatformIO**: https://docs.platformio.org/
- **Datasheet VEML7700**: https://www.vishay.com/docs/84286/veml7700.pdf
- **ESP8266 Arduino Core**: https://github.com/esp8266/Arduino

### Comunidad
- **GitHub Issues**: Para reportar bugs
- **Discussions**: Para preguntas generales
- **Wiki**: Documentación extendida

---

💡 **Consejo**: Siempre prueba primero con alimentación USB antes de pasar a batería solar.
