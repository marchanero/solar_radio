#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_VEML7700.h>

// Pin GPIO para controlar el transistor 2N2222
// En Wemos D1: D2 = GPIO4, D3 = GPIO0, D4 = GPIO2, etc.
const int RADIO_CONTROL_PIN = 16; // GPIO16 (corresponde a D0 en Wemos D1)

// Pin del LED built-in (GPIO2 en Wemos D1)
const int LED_BUILTIN_PIN = 2;

// Objeto del sensor VEML7700
Adafruit_VEML7700 veml = Adafruit_VEML7700();

// Tiempos de ciclo (en milisegundos)
const unsigned long TIEMPO_VERIFICACION = 60000;  // Verificar luz cada 30 segundos

// Umbrales de luz para interruptor crepuscular (en lux)
const float UMBRAL_ENCENDIDO = 30.0;   // Encender cuando baje de 30 lux (crepúsculo temprano)
const float UMBRAL_APAGADO = 50.0;     // Apagar cuando suba de 50 lux (amanecer muy temprano)

// Estado actual de la radio
bool radio_encendida = false;

// Contador de ciclos
unsigned long contador_ciclos = 0;

// Variables para el temporizador
unsigned long tiempo_ultima_verificacion = 0;
unsigned long tiempo_siguiente_verificacion = 0;

// Función para hacer parpadeo del LED (bip bip visual)
void bipBipLED(int veces, int duracion_ms) {
  for (int i = 0; i < veces; i++) {
    digitalWrite(LED_BUILTIN_PIN, LOW);  // Encender LED (LOW en ESP8266)
    delay(duracion_ms);
    digitalWrite(LED_BUILTIN_PIN, HIGH); // Apagar LED (HIGH en ESP8266)
    if (i < veces - 1) {
      delay(duracion_ms);
    }
  }
}

// Función para bip de inicio del sistema
void bipInicio() {
  Serial.println("🔊 ¡BIP-BIP! Sistema iniciando...");
  bipBipLED(3, 200); // 3 parpadeos de 200ms cada uno
}

// Función para bip de medición
void bipMedicion() {
  Serial.println("🔊 ¡BIP! Realizando medición...");
  bipBipLED(1, 100); // 1 parpadeo corto de 100ms
}

// Función para formatear tiempo en mm:ss
void mostrarTiempo(unsigned long milisegundos) {
  unsigned long segundos = milisegundos / 1000;
  unsigned long minutos = segundos / 60;
  segundos = segundos % 60;
  
  if (minutos < 10) Serial.print("0");
  Serial.print(minutos);
  Serial.print(":");
  if (segundos < 10) Serial.print("0");
  Serial.print(segundos);
}

// Función para mostrar el temporizador hasta la próxima verificación
void mostrarTemporizador() {
  unsigned long tiempo_actual = millis();
  
  if (tiempo_siguiente_verificacion > tiempo_actual) {
    unsigned long tiempo_restante = tiempo_siguiente_verificacion - tiempo_actual;
    Serial.print("⏰ Próxima verificación en: ");
    mostrarTiempo(tiempo_restante);
    Serial.println(" (mm:ss)");
  } else {
    Serial.println("⏰ Verificación en curso...");
  }
}

// Función para leer y mostrar datos del sensor de luz
void mostrarDatosLuz() {
  float lux = veml.readLux();
  float als = veml.readALS();
  
  Serial.print("💡 Intensidad lumínica: ");
  Serial.print(lux);
  Serial.print(" lux | ALS: ");
  Serial.println(als);
  
  // Clasificar nivel de luz
  if (lux < 10) {
    Serial.println("   📊 Nivel: MUY OSCURO (NOCHE)");
  } else if (lux < 50) {
    Serial.println("   📊 Nivel: OSCURO (CREPÚSCULO)");
  } else if (lux < 200) {
    Serial.println("   📊 Nivel: INTERIOR/NUBLADO");
  } else if (lux < 1000) {
    Serial.println("   📊 Nivel: LUZ BRILLANTE");
  } else {
    Serial.println("   📊 Nivel: LUZ SOLAR DIRECTA (DÍA)");
  }
  
  // Mostrar estado del interruptor crepuscular
  Serial.print("   🌅 Estado actual: Radio ");
  if (radio_encendida) {
    Serial.println("ENCENDIDA");
  } else {
    Serial.println("APAGADA");
  }
  
  // Mostrar próxima acción esperada
  if (radio_encendida && lux >= UMBRAL_APAGADO) {
    Serial.print("   🔄 Próxima acción: APAGAR (luz >= ");
    Serial.print(UMBRAL_APAGADO);
    Serial.println(" lux - AMANECER)");
  } else if (!radio_encendida && lux <= UMBRAL_ENCENDIDO) {
    Serial.print("   🔄 Próxima acción: ENCENDER (luz <= ");
    Serial.print(UMBRAL_ENCENDIDO);
    Serial.println(" lux - CREPÚSCULO)");
  } else {
    Serial.println("   ✅ Estado correcto para las condiciones actuales");
  }
}

void setup() {
  // Configurar pines PRIMERO (antes que todo)
  pinMode(RADIO_CONTROL_PIN, OUTPUT);
  pinMode(LED_BUILTIN_PIN, OUTPUT);
  
  // Inicializar LED apagado y radio apagada
  digitalWrite(LED_BUILTIN_PIN, HIGH); // HIGH = apagado en ESP8266
  digitalWrite(RADIO_CONTROL_PIN, LOW); // Asegurar radio apagada al inicio
  
  // Pausa para estabilización del sistema
  delay(2000); // Aumentado para mejor estabilización con batería
  
  // BIP BIP de inicio del sistema (funciona sin Serial)
  bipInicio();
  
  // Inicializar comunicación serie para depuración
  Serial.begin(9600);
  delay(500); // Pausa para establecer conexión serie
  
  Serial.println("=== INTERRUPTOR CREPUSCULAR CON TEMPORIZADOR ===");
  Serial.println("Control automático de radio con sensor VEML7700");
  Serial.print("🌅 Umbral ENCENDIDO (crepúsculo): ");
  Serial.print(UMBRAL_ENCENDIDO);
  Serial.println(" lux");
  Serial.print("🌄 Umbral APAGADO (amanecer): ");
  Serial.print(UMBRAL_APAGADO);
  Serial.println(" lux");
  Serial.print("⏰ Intervalo de verificación: ");
  Serial.print(TIEMPO_VERIFICACION / 1000);
  Serial.println(" segundos");
  Serial.println("🔊 LED integrado configurado para indicaciones visuales");
  Serial.println();
  
  // Inicializar I2C con reintentos para mayor robustez
  Wire.begin();
  delay(100);
  
  // Inicializar sensor VEML7700 con reintentos
  bool sensor_inicializado = false;
  for (int intento = 1; intento <= 3; intento++) {
    if (veml.begin()) {
      sensor_inicializado = true;
      Serial.println("✅ Sensor VEML7700 inicializado correctamente");
      break;
    } else {
      Serial.print("⚠️ Intento ");
      Serial.print(intento);
      Serial.println("/3 - Error inicializando sensor VEML7700");
      // Parpadeo de error (2 parpadeos rápidos)
      bipBipLED(2, 100);
      delay(1000);
    }
  }
  
  if (!sensor_inicializado) {
    Serial.println("❌ ERROR CRÍTICO: No se pudo inicializar el sensor VEML7700!");
    // Parpadeo de error crítico (5 parpadeos rápidos)
    while (1) {
      bipBipLED(5, 200);
      delay(2000);
    }
  }
  
  // Inicializar radio en estado apagado
  digitalWrite(RADIO_CONTROL_PIN, LOW);
  radio_encendida = false;
  Serial.println("✅ Radio inicializada en estado APAGADO");
  
  // Hacer una lectura inicial para determinar el estado
  Serial.println("\n📊 Evaluando condiciones iniciales...");
  bipMedicion(); // BIP para la medición inicial
  mostrarDatosLuz();
  
  // Inicializar temporizador
  tiempo_ultima_verificacion = millis();
  tiempo_siguiente_verificacion = tiempo_ultima_verificacion + TIEMPO_VERIFICACION;
  mostrarTemporizador();
  
  Serial.println("================================\n");
}

void loop() {
  unsigned long tiempo_actual = millis();
  
  // Verificar si es momento de hacer una nueva verificación
  if (tiempo_actual >= tiempo_siguiente_verificacion) {
    // Incrementar contador de verificaciones
    contador_ciclos++;
    
    // Actualizar tiempos para el próximo ciclo
    tiempo_ultima_verificacion = tiempo_actual;
    tiempo_siguiente_verificacion = tiempo_actual + TIEMPO_VERIFICACION;
    
    // Mostrar información de la verificación
    Serial.print("🔍 VERIFICACIÓN #");
    Serial.print(contador_ciclos);
    Serial.print(" - ");
    
    // Obtener hora actual (simulada con millis)
    unsigned long horas = (tiempo_actual / 3600000) % 24;
    unsigned long minutos = (tiempo_actual / 60000) % 60;
    Serial.print(horas);
    Serial.print(":");
    if (minutos < 10) Serial.print("0");
    Serial.println(minutos);
    
    // Leer nivel de luz actual
    bipMedicion(); // BIP para indicar que se está midiendo
    float lux_actual = veml.readLux();
    mostrarDatosLuz();
    Serial.println();
    
    // LÓGICA DEL INTERRUPTOR CREPUSCULAR
    bool cambio_estado = false;
    
    if (!radio_encendida && lux_actual <= UMBRAL_ENCENDIDO) {
      // CONDICIÓN: ENCENDER (Crepúsculo/Noche)
      Serial.println("🌙 CREPÚSCULO DETECTADO - Encendiendo radio...");
      digitalWrite(RADIO_CONTROL_PIN, HIGH);
      radio_encendida = true;
      cambio_estado = true;
      
      Serial.print("✅ RADIO ENCENDIDA (luz: ");
      Serial.print(lux_actual);
      Serial.print(" <= ");
      Serial.print(UMBRAL_ENCENDIDO);
      Serial.println(" lux)");
      
    } else if (radio_encendida && lux_actual >= UMBRAL_APAGADO) {
      // CONDICIÓN: APAGAR (Amanecer/Día)
      Serial.println("🌅 AMANECER DETECTADO - Apagando radio...");
      digitalWrite(RADIO_CONTROL_PIN, LOW);
      radio_encendida = false;
      cambio_estado = true;
      
      Serial.print("✅ RADIO APAGADA (luz: ");
      Serial.print(lux_actual);
      Serial.print(" >= ");
      Serial.print(UMBRAL_APAGADO);
      Serial.println(" lux)");
      
    } else {
      // SIN CAMBIOS - Mantener estado actual
      if (radio_encendida) {
        Serial.print("🔄 Radio manteniéndose ENCENDIDA (noche continúa - ");
        Serial.print(lux_actual);
        Serial.print(" lux < ");
        Serial.print(UMBRAL_APAGADO);
        Serial.println(" lux)");
      } else {
        Serial.print("🔄 Radio manteniéndose APAGADA (día continúa - ");
        Serial.print(lux_actual);
        Serial.print(" lux > ");
        Serial.print(UMBRAL_ENCENDIDO);
        Serial.println(" lux)");
      }
    }
    
    // Mostrar resumen del estado actual
    if (cambio_estado) {
      Serial.println("🎯 ESTADO CAMBIADO!");
    }
    
    Serial.print("📊 Estado final: Radio ");
    if (radio_encendida) {
      Serial.println("🟢 ENCENDIDA");
    } else {
      Serial.println("🔴 APAGADA");
    }
    
    // Mostrar temporizador para la próxima verificación
    mostrarTemporizador();
    Serial.println("-------------------");
    Serial.println();
    
  } else {
    // Mostrar temporizador cada 5 segundos mientras espera
    static unsigned long ultimo_temporizador = 0;
    if (tiempo_actual - ultimo_temporizador >= 5000) {
      ultimo_temporizador = tiempo_actual;
      
      // Parpadeo visual cada 5 segundos para indicar que el sistema está activo
      Serial.println("🔊 ¡BIP! Sistema activo - verificando temporizador...");
      bipBipLED(1, 100); // 1 parpadeo de 100ms
      
      Serial.print("💤 Esperando... ");
      mostrarTemporizador();
      
      // Mostrar estado actual de la radio
      Serial.print("   📊 Radio actualmente: ");
      if (radio_encendida) {
        Serial.println("🟢 ENCENDIDA");
      } else {
        Serial.println("🔴 APAGADA");
      }
    }
  }
  
  // Pequeña pausa para no saturar el procesador
  delay(100);
}
