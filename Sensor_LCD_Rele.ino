/*
    LECTURA DE TEMPERATURA Y HUMEDAD MEDIANTE EL SENSOR ATH-10 Y MUESTRA EN PANTALLA LCD 
    Libreria del sensor de humedad y temperatura AHT10
    https://github.com/enjoyneering/AHT10

    Libreria del I2C que maneja el LCD , LiquidCrystal_I2C
    https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library.
*/


#include <Adafruit_AHTX0.h>  // Incluimos la libreria necesaria para manejar el sensor
#include <Wire.h> 
#include <LiquidCrystal_I2C.h> // Incluimos la libreria necesaria para manejar el LCD

#define RELAY1 9 // Pin de salida para RELAY1 de la resistencia de la incubadora

#define RELAY2 8 // Pin de salida para controlar el RELAY2 del motorcito girahuevos (se necesita bateria externa para el motorcito porque activarlo directo desde arduino nos puede dañar la placa)
unsigned long tiempoAnterior = 0; // Variable para almacenar el tiempo anterior


String frase;                  // Para la frase que queremos mostrar en el lcd
int linea ;                    // Para la linea donde vamos a escribir
int columna ;                  // Para la columna de la linea donde vamos a escribir
int total_lineas = 2;          // Para indicar cuantas lineas tiene nuestro lcd
int total_columnas = 16;       // Para indicar cuantas columnas tiene nuestro lcd

Adafruit_AHTX0 aht;                                            // Asigamos el sensor 
LiquidCrystal_I2C lcd_i2c(0x27, total_columnas, total_lineas); // indicamos cuantos caracteres y lineas tiene nuestro lcd


void setup(){
    // Incializamos la pantalla LCD
    lcd_i2c.begin(16, 2);
    lcd_i2c.backlight();
    lcd_i2c.clear();
    // NOS ASEGURAMOS QUE ESTA PRESENTE EL MODULO AHT10 SI NO ES ASI, EL BUCLE NO CONTINUARA LA CARGA
    if (! aht.begin()) {      // Iniciamos el sensor
             while (1) delay(10);
    }
    // Configurar el pin de salida y el estado inicial del RELAY1
    pinMode(RELAY1, OUTPUT);
    digitalWrite(RELAY1, LOW);

    // Configurar el pin de salida del RELAY2
    pinMode(RELAY2, OUTPUT);
}

void loop() {
    
   
    sensors_event_t humedad, temperatura;
    aht.getEvent(&humedad, &temperatura);   // rellena los datos de temp. y humedad. con datos nuevos en cuanto van siendo sensados
    
     
    frase = "Temp.: " + String (temperatura.temperature) + " " + (char)223 + "C";   // Definimos como aparecerá escrita  la temperatura en el LCD
    linea = 0; columna = 0;
    lcd_i2c.setCursor(columna, linea);
    lcd_i2c.print(frase);

    frase = "Hum. : " + String (humedad.relative_humidity) + " %Hr";  // Definimos como aparecerá escrita la humedad en el LCD
    linea = 1; columna = 0;
    lcd_i2c.setCursor(columna, linea);
    lcd_i2c.print(frase);     


    // Código del RELAY1, que activará la resistencia si el sensor detecta una temperatura fuera del rango
    if( aht.getEvent (&humedad, &temperatura) <= 37.1){    //Función "Si", si la temperatura es menor o igual a 37.1°C, el relay se accionará y cerrará el circuito de la resistencia
      digitalWrite(RELAY1, HIGH);
    }
    // //Función "Sino", si la temperatura NO es menor o igual a 37.1°C, el relay continua abierto y no se accionará hasta que la temperatura se encuentre fuera de los rangos preestablecidos
    else{   
      digitalWrite(RELAY1, LOW);
    }


    // Código del RELAY2, que activará el motor durante 5 segundos cada 8 horas
    { // Obtener el tiempo actual en milisegundos
    unsigned long tiempoActual = millis();

    // Verificar si han pasado 8 horas desde la última activación
    if (tiempoActual - tiempoAnterior >= 28800000) { //(28.800.000 milisegundos equivalen a 8 horas)
    // Activar el motor DC durante 5 segundos
    digitalWrite(RELAY2, HIGH);
    delay(5000);  // Mantener activado el motor durante 5 segundos
    digitalWrite(RELAY2, LOW);

    // Guardar el tiempo actual como tiempo anterior
    tiempoAnterior = tiempoActual;}
    else {
    // Sino han pasado 8 horas desde la última activación del motor, mantener el motor DC apagado
    digitalWrite(RELAY2, LOW);
  }

    delay(500);
    }
}