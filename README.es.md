# 🌅 Interruptor Crepuscular Solar para Radio

Un sistema automatizado de control de radio que se activa y desactiva según las condiciones de luz ambiente, diseñado para funcionar con energía solar.

![Estado del Proyecto](https://img.shields.io/badge/Estado-Funcional-brightgreen)
![Plataforma](https://img.shields.io/badge/Plataforma-ESP8266-blue)
![Framework](https://img.shields.io/badge/Framework-Arduino-teal)
![Licencia](https://img.shields.io/badge/Licencia-MIT-yellow)

## 📋 Descripción

Este proyecto implementa un **interruptor crepuscular inteligente** que controla automáticamente una radio basándose en los niveles de luz ambiente. Utiliza un sensor VEML7700 para medir la luminosidad y activa/desactiva la radio a través de un transistor 2N2222, ideal para instalaciones solares autónomas.

### ✨ Características Principales

- 🌙 **Control automático crepuscular**: Enciende la radio al anochecer (< 30 lux) y la apaga al amanecer (> 50 lux)
- 🔋 **Optimizado para batería**: Funciona independientemente del monitor serie
- 📊 **Monitoreo en tiempo real**: Muestra datos de luminosidad, estado y temporizadores
- 💡 **Indicaciones visuales**: LED built-in con diferentes patrones para cada estado
- ⏰ **Temporizador visual**: Cuenta regresiva hasta la próxima verificación
- 🔧 **Reintentos automáticos**: Sistema robusto de inicialización del sensor
- 🎯 **Histéresis inteligente**: Evita oscilaciones con umbrales diferenciados

## 🛠️ Hardware Requerido

### Componentes Principales
- **Microcontrolador**: Wemos D1 Mini (ESP8266)
- **Sensor de luz**: VEML7700 (comunicación I2C)
- **Transistor**: 2N2222 (para control de la radio)
- **Alimentación**: Batería 3.7V + panel solar (opcional)

### Conexiones

```
Wemos D1 Mini    →    Componente
GPIO16 (D0)      →    Base del transistor 2N2222
GPIO2 (D4)       →    LED built-in (integrado)
GPIO4 (D2)       →    SDA del VEML7700
GPIO5 (D1)       →    SCL del VEML7700
3.3V             →    VCC del VEML7700
GND              →    GND del VEML7700
```

### Circuito del Transistor

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

## 🚀 Instalación

### 1. Configuración del Entorno

```bash
# Instalar PlatformIO
pip install platformio

# Clonar el repositorio
git clone https://github.com/tuusuario/solar_radio_wemos.git
cd solar_radio_wemos
```

### 2. Compilación y Carga

```bash
# Compilar el proyecto
pio build

# Cargar al microcontrolador
pio upload

# Monitorear serie (opcional)
pio device monitor
```

### 3. Dependencias

Las siguientes librerías se instalan automáticamente:
- `Adafruit VEML7700 Library` v2.1.6
- `Adafruit BusIO` v1.16.1

## 🎛️ Configuración

### Umbrales de Luz

```cpp
// Configuración en src/main.cpp
const float UMBRAL_ENCENDIDO = 30.0;  // Enciende con menos de 30 lux
const float UMBRAL_APAGADO = 50.0;    // Apaga con más de 50 lux
```

### Intervalos de Tiempo

```cpp
const unsigned long TIEMPO_VERIFICACION = 60000;  // Verifica cada 60 segundos
```

## 📊 Funcionamiento

### Estados del Sistema

| Nivel de Luz | Estado Radio | Acción |
|--------------|--------------|--------|
| < 30 lux     | 🟢 ENCENDIDA | Modo nocturno activo |
| 30-50 lux    | 🔄 SIN CAMBIO | Zona de histéresis |
| > 50 lux     | 🔴 APAGADA   | Modo diurno activo |

### Indicaciones Visuales (LED Built-in)

| Patrón | Significado |
|--------|-------------|
| 3 parpadeos lentos (200ms) | 🔊 Sistema iniciando |
| 1 parpadeo medio (100ms) | 📊 Realizando medición |
| 3 parpadeos rápidos (100ms) | ⏰ Sistema activo (cada 5s) |
| 2 parpadeos rápidos | ⚠️ Error de sensor (reintentando) |
| 5 parpadeos continuos | ❌ Error crítico de sensor |

### Salida del Monitor Serie

```
=== INTERRUPTOR CREPUSCULAR CON TEMPORIZADOR ===
🌅 Umbral ENCENDIDO (crepúsculo): 30.00 lux
🌄 Umbral APAGADO (amanecer): 50.00 lux
⏰ Intervalo de verificación: 60 segundos

🔍 VERIFICACIÓN #1 - 0:01
🔊 ¡BIP! Realizando medición...
💡 Intensidad lumínica: 25.4 lux | ALS: 142
   📊 Nivel: OSCURO (CREPÚSCULO)
   🌅 Estado actual: Radio APAGADA
   🔄 Próxima acción: ENCENDER (luz <= 30.0 lux - CREPÚSCULO)

🌙 CREPÚSCULO DETECTADO - Encendiendo radio...
✅ RADIO ENCENDIDA (luz: 25.4 <= 30.0 lux)
🎯 ESTADO CAMBIADO!
📊 Estado final: Radio 🟢 ENCENDIDA
```

## 🔧 Personalización

### Modificar Umbrales de Sensibilidad

Para mayor sensibilidad (respuesta más rápida):
```cpp
const float UMBRAL_ENCENDIDO = 20.0;  // Más sensible al anochecer
const float UMBRAL_APAGADO = 40.0;    // Más sensible al amanecer
```

Para menor sensibilidad (más estable):
```cpp
const float UMBRAL_ENCENDIDO = 50.0;  // Menos sensible al anochecer
const float UMBRAL_APAGADO = 80.0;    // Menos sensible al amanecer
```

### Cambiar Intervalos de Verificación

```cpp
const unsigned long TIEMPO_VERIFICACION = 30000;  // Cada 30 segundos (más frecuente)
const unsigned long TIEMPO_VERIFICACION = 300000; // Cada 5 minutos (ahorro de batería)
```

## 🔋 Optimización para Energía Solar

### Configuración Recomendada
- **Panel solar**: 6V 1W mínimo
- **Batería**: 3.7V Li-Ion 2000mAh
- **Regulador**: TP4056 con protección
- **Consumo promedio**: ~80mA en funcionamiento

### Consejos de Instalación
1. Orientar el sensor VEML7700 hacia el cielo (evitar sombras)
2. Proteger la electrónica de la humedad
3. Usar cables cortos para minimizar pérdidas
4. Verificar conexiones I2C (pull-ups internos suficientes)

## 🐛 Solución de Problemas

### Problema: No funciona con batería
**Solución**: 
- Verificar voltaje de batería (mín. 3.0V)
- Comprobar conexiones I2C del VEML7700
- Observar patrones de LED para diagnóstico

### Problema: Oscilaciones frecuentes
**Solución**: 
- Aumentar diferencia entre umbrales
- Verificar estabilidad de la fuente de luz
- Limpiar el sensor VEML7700

### Problema: Sensor no detectado
**Solución**: 
- Verificar conexiones SDA/SCL
- Probar con diferentes direcciones I2C
- Reiniciar el sistema

## 📈 Desarrollo Futuro

- [ ] Integración WiFi para monitoreo remoto
- [ ] Aplicación web de configuración
- [ ] Almacenamiento de datos históricos
- [ ] Control por calendario/horario
- [ ] Soporte para múltiples sensores
- [ ] Modo de bajo consumo avanzado

## 🤝 Contribuciones

Las contribuciones son bienvenidas. Por favor:

1. Fork el proyecto
2. Crea una rama para tu feature (`git checkout -b feature/AmazingFeature`)
3. Commit tus cambios (`git commit -m 'Add some AmazingFeature'`)
4. Push a la rama (`git push origin feature/AmazingFeature`)
5. Abre un Pull Request

## 📄 Licencia

Este proyecto está bajo la Licencia MIT - ver el archivo [LICENSE](LICENSE) para más detalles.

## 👨‍💻 Autor

**Roberto Marchanero**
- GitHub: [@marchanero](https://github.com/marchanero)
- Proyecto: [solar_radio_wemos](https://github.com/marchanero/solar_radio)

## 🙏 Agradecimientos

- [Adafruit](https://www.adafruit.com/) por las librerías del sensor VEML7700
- [PlatformIO](https://platformio.org/) por el excelente entorno de desarrollo
- Comunidad ESP8266 por el soporte y documentación

---

⭐ ¡Si este proyecto te ha sido útil, considera darle una estrella!

---

## 🌍 Otros Idiomas

- [English](README.md) - English version
