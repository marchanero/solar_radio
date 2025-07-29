# 💻 Ejemplos y Código de Referencia

## 🔍 Lectura Básica del Sensor VEML7700

```cpp
#include <Adafruit_VEML7700.h>

Adafruit_VEML7700 veml = Adafruit_VEML7700();

void setup() {
  Serial.begin(9600);
  
  if (!veml.begin()) {
    Serial.println("Sensor no encontrado");
    while (1);
  }
  
  Serial.println("VEML7700 inicializado");
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

## 🔌 Control de Transistor 2N2222

```cpp
const int TRANSISTOR_PIN = 16; // GPIO16

void setup() {
  pinMode(TRANSISTOR_PIN, OUTPUT);
  digitalWrite(TRANSISTOR_PIN, LOW); // Iniciar apagado
}

void encenderRadio() {
  digitalWrite(TRANSISTOR_PIN, HIGH);
  Serial.println("Radio ENCENDIDA");
}

void apagarRadio() {
  digitalWrite(TRANSISTOR_PIN, LOW);
  Serial.println("Radio APAGADA");
}
```

## 💡 Sistema de Parpadeos LED

```cpp
const int LED_PIN = 2; // LED built-in

void parpadear(int veces, int duracion_ms) {
  for (int i = 0; i < veces; i++) {
    digitalWrite(LED_PIN, LOW);  // Encender (invertido en ESP8266)
    delay(duracion_ms);
    digitalWrite(LED_PIN, HIGH); // Apagar
    if (i < veces - 1) {
      delay(duracion_ms);
    }
  }
}

void patronInicio() {
  parpadear(3, 200); // 3 parpadeos lentos
}

void patronError() {
  parpadear(5, 100); // 5 parpadeos rápidos
}
```

## ⏰ Temporizador No Bloqueante

```cpp
unsigned long tiempo_anterior = 0;
const unsigned long INTERVALO = 60000; // 60 segundos

void loop() {
  unsigned long tiempo_actual = millis();
  
  if (tiempo_actual - tiempo_anterior >= INTERVALO) {
    tiempo_anterior = tiempo_actual;
    
    // Ejecutar tarea periódica
    realizarMedicion();
  }
  
  // Otras tareas no bloqueantes
  verificarBotones();
  actualizarDisplay();
}
```

## 🌅 Lógica de Interruptor Crepuscular

```cpp
const float UMBRAL_ON = 30.0;
const float UMBRAL_OFF = 50.0;
bool estado_actual = false;

void evaluarLuz(float lux) {
  if (!estado_actual && lux <= UMBRAL_ON) {
    // Encender cuando esté oscuro
    estado_actual = true;
    encenderDispositivo();
    Serial.println("CREPÚSCULO: Encendiendo");
    
  } else if (estado_actual && lux >= UMBRAL_OFF) {
    // Apagar cuando haya luz
    estado_actual = false;
    apagarDispositivo();
    Serial.println("AMANECER: Apagando");
  }
  
  // Histéresis evita oscilaciones entre 30-50 lux
}
```

## 🔄 Sistema de Reintentos Robusto

```cpp
bool inicializarSensorConReintentos(int max_intentos = 3) {
  for (int intento = 1; intento <= max_intentos; intento++) {
    Serial.print("Intento ");
    Serial.print(intento);
    Serial.print("/");
    Serial.println(max_intentos);
    
    if (veml.begin()) {
      Serial.println("✅ Sensor inicializado");
      return true;
    }
    
    Serial.println("❌ Error en sensor");
    parpadear(2, 100); // Indicar error
    delay(1000);
  }
  
  Serial.println("🚨 Error crítico de sensor");
  return false;
}
```

## 📊 Clasificación de Niveles de Luz

```cpp
enum NivelLuz {
  MUY_OSCURO,    // 0-10 lux
  OSCURO,        // 10-50 lux  
  INTERIOR,      // 50-200 lux
  BRILLANTE,     // 200-1000 lux
  SOL_DIRECTO    // >1000 lux
};

NivelLuz clasificarLuz(float lux) {
  if (lux < 10) return MUY_OSCURO;
  if (lux < 50) return OSCURO;
  if (lux < 200) return INTERIOR;
  if (lux < 1000) return BRILLANTE;
  return SOL_DIRECTO;
}

String obtenerDescripcion(NivelLuz nivel) {
  switch (nivel) {
    case MUY_OSCURO: return "MUY OSCURO (NOCHE)";
    case OSCURO: return "OSCURO (CREPÚSCULO)";
    case INTERIOR: return "INTERIOR/NUBLADO";
    case BRILLANTE: return "LUZ BRILLANTE";
    case SOL_DIRECTO: return "LUZ SOLAR DIRECTA";
    default: return "DESCONOCIDO";
  }
}
```

## 🕐 Formateo de Tiempo

```cpp
void mostrarTiempo(unsigned long milisegundos) {
  unsigned long segundos = milisegundos / 1000;
  unsigned long minutos = segundos / 60;
  unsigned long horas = minutos / 60;
  
  segundos = segundos % 60;
  minutos = minutos % 60;
  
  if (horas > 0) {
    Serial.print(horas);
    Serial.print("h ");
  }
  
  if (minutos < 10) Serial.print("0");
  Serial.print(minutos);
  Serial.print(":");
  if (segundos < 10) Serial.print("0");
  Serial.print(segundos);
}

void mostrarTemporizadorRegresivo(unsigned long tiempo_restante) {
  Serial.print("⏰ Próxima verificación en: ");
  mostrarTiempo(tiempo_restante);
  Serial.println(" (mm:ss)");
}
```

## 🛡️ Protección contra Desbordamiento de millis()

```cpp
bool tiempoTranscurrido(unsigned long &tiempo_anterior, unsigned long intervalo) {
  unsigned long tiempo_actual = millis();
  
  // Manejo de desbordamiento (cada ~49 días)
  if (tiempo_actual < tiempo_anterior) {
    tiempo_anterior = tiempo_actual; // Reset en desbordamiento
  }
  
  if (tiempo_actual - tiempo_anterior >= intervalo) {
    tiempo_anterior = tiempo_actual;
    return true;
  }
  
  return false;
}

// Uso:
unsigned long ultimo_parpadeo = 0;
const unsigned long INTERVALO_PARPADEO = 5000;

void loop() {
  if (tiempoTranscurrido(ultimo_parpadeo, INTERVALO_PARPADEO)) {
    parpadear(1, 100);
  }
}
```

## 🔧 Configuración Dinámica de Umbrales

```cpp
struct ConfiguracionLuz {
  float umbral_encendido;
  float umbral_apagado;
  unsigned long intervalo_ms;
  bool debug_activo;
};

ConfiguracionLuz config = {
  .umbral_encendido = 30.0,
  .umbral_apagado = 50.0,
  .intervalo_ms = 60000,
  .debug_activo = true
};

void ajustarSensibilidad(String modo) {
  if (modo == "alta") {
    config.umbral_encendido = 10.0;
    config.umbral_apagado = 30.0;
    Serial.println("Modo alta sensibilidad");
    
  } else if (modo == "baja") {
    config.umbral_encendido = 50.0;
    config.umbral_apagado = 100.0;
    Serial.println("Modo baja sensibilidad");
    
  } else {
    config.umbral_encendido = 30.0;
    config.umbral_apagado = 50.0;
    Serial.println("Modo sensibilidad normal");
  }
}
```

## 📈 Monitoreo de Rendimiento

```cpp
struct EstadisticasSistema {
  unsigned long ciclos_completados;
  unsigned long errores_sensor;
  unsigned long cambios_estado;
  unsigned long tiempo_encendido_total;
  unsigned long uptime_inicio;
};

EstadisticasSistema stats = {0};

void inicializarEstadisticas() {
  stats.uptime_inicio = millis();
}

void mostrarEstadisticas() {
  unsigned long uptime = millis() - stats.uptime_inicio;
  
  Serial.println("📊 ESTADÍSTICAS DEL SISTEMA");
  Serial.print("⏱️ Tiempo funcionamiento: ");
  mostrarTiempo(uptime);
  Serial.println();
  
  Serial.print("🔄 Ciclos completados: ");
  Serial.println(stats.ciclos_completados);
  
  Serial.print("⚠️ Errores de sensor: ");
  Serial.println(stats.errores_sensor);
  
  Serial.print("🔀 Cambios de estado: ");
  Serial.println(stats.cambios_estado);
  
  float porcentaje_encendido = (stats.tiempo_encendido_total * 100.0) / uptime;
  Serial.print("💡 Tiempo radio encendida: ");
  Serial.print(porcentaje_encendido, 1);
  Serial.println("%");
}
```

## 🔍 Scanner I2C para Diagnóstico

```cpp
void scanearI2C() {
  Serial.println("Escaneando dispositivos I2C...");
  
  int dispositivos_encontrados = 0;
  
  for (byte direccion = 1; direccion < 127; direccion++) {
    Wire.beginTransmission(direccion);
    byte error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("Dispositivo encontrado en 0x");
      if (direccion < 16) Serial.print("0");
      Serial.print(direccion, HEX);
      
      // Identificar dispositivos conocidos
      if (direccion == 0x10) Serial.print(" (VEML7700)");
      
      Serial.println();
      dispositivos_encontrados++;
    }
  }
  
  if (dispositivos_encontrados == 0) {
    Serial.println("❌ No se encontraron dispositivos I2C");
  } else {
    Serial.print("✅ Se encontraron ");
    Serial.print(dispositivos_encontrados);
    Serial.println(" dispositivos");
  }
}
```

---

## 🚀 Extensiones Avanzadas

### WiFi y Servidor Web

```cpp
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "TuWiFi";
const char* password = "TuPassword";

ESP8266WebServer server(80);

void configurarWiFi() {
  WiFi.begin(ssid, password);
  
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.print("Conectado. IP: ");
  Serial.println(WiFi.localIP());
}

void configurarServidor() {
  server.on("/", []() {
    String html = "<h1>Interruptor Crepuscular</h1>";
    html += "<p>Lux actual: " + String(veml.readLux()) + "</p>";
    html += "<p>Estado radio: " + String(radio_encendida ? "ON" : "OFF") + "</p>";
    server.send(200, "text/html", html);
  });
  
  server.begin();
  Serial.println("Servidor web iniciado");
}
```

### Almacenamiento de Datos

```cpp
#include <EEPROM.h>

struct ConfiguracionPersistente {
  float umbral_on;
  float umbral_off;
  unsigned long intervalo;
  uint16_t checksum;
};

void guardarConfiguracion(ConfiguracionPersistente config) {
  config.checksum = calcularChecksum(config);
  EEPROM.put(0, config);
  EEPROM.commit();
  Serial.println("Configuración guardada");
}

bool cargarConfiguracion(ConfiguracionPersistente &config) {
  EEPROM.get(0, config);
  
  if (config.checksum == calcularChecksum(config)) {
    Serial.println("Configuración cargada");
    return true;
  }
  
  Serial.println("Configuración corrupta, usando valores por defecto");
  return false;
}
```

---

💡 **Tip**: Estos ejemplos pueden combinarse y adaptarse según las necesidades específicas de tu proyecto.
