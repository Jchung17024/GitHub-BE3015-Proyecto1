/*-----------------------------------------------------------------------------
Universidad del Valle de Guatemala
BE3015 - Electrónica DIgital 2
Laboratorio 2 - Interrupciones y Temporizador
Jeffrey Chung
17024
-----------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------
// LIBRERIAS
//-----------------------------------------------------------------------------
#include <Arduino.h>


//-----------------------------------------------------------------------------
// DEFINICION DE PINES
//-----------------------------------------------------------------------------
#define read_temp           13
#define sensor_temp         35

#define RED                 32
#define GREEN               25
#define BLUE                26
#define servo               33

// canales 0, 1 y 2 para PWM de rojo, verde y azul respectivamente
#define PWMchannel_red      0
#define PWMchannel_green    1
#define PWMchannel_blue     2

// frecuencia de 144Hz para el PWM del LED RGB
#define PWMfreq_led         144

// canal 3 para el PWM del servo motor
#define PWMchannel_servo    3

// esta frecuencia es específicamente para el SG90
#define PWMfreq_servo       50

// resolución de 8 bits, esta resolucion es la misma para todos los PWM
#define resolution          8

#define display0            27
#define display1            14
#define display2            12

// estos son los pines de los displays de 7 seg.
#define a                   23
#define b                   22
#define c                   21
#define d                   19
#define e                   18
#define f                   5  
#define g                   17
#define dp                  16

//-----------------------------------------------------------------------------
// PROTOTIPO DE FUNCIONES
//-----------------------------------------------------------------------------
void config_pwm(void);

//-----------------------------------------------------------------------------
// VARIABLES GLOBALES
//-----------------------------------------------------------------------------
int adcTemperature = 0;
float temperature = 0;
int dutycycle_red = 0;
int dutycycle_green = 0;
int dutycycle_blue = 0;
int pos_servo = 0;

//-----------------------------------------------------------------------------
// ISR
//-----------------------------------------------------------------------------
// interrupcion para medicion de temp. con boton
void IRAM_ATTR ISR_readTemp(){
  adcTemperature = analogRead(sensor_temp);
  temperature = adcTemperature/204.8;
  Serial.print("Temperature: ");
  Serial.print(temperature, 1);
  Serial.print("°C");
}

//-----------------------------------------------------------------------------
// CONFIG.
//-----------------------------------------------------------------------------
void setup() {
  // baud rate de 115200Hz
  Serial.begin(115200);

  // config. pines como entradas y salidas
  pinMode(read_temp, INPUT_PULLUP);

  config_pwm();

  // isr del boton conectado en pin 13, interrupcion ISR_readtemp, modo falling
  attachInterrupt(read_temp, ISR_readTemp, FALLING);
}


//-----------------------------------------------------------------------------
// LOOP PRINCIPAL
//-----------------------------------------------------------------------------
void loop() {
  // semaforo y reloj de temperatura
  //-----------------------------------------------------------------------------
  // dutycycle de 255 = off y 0 = brightest, led RGB off inicialmente
  dutycycle_red = 255;
  dutycycle_green = 255;
  dutycycle_blue = 255;
  // 6 es el valor para 0 grados del servo 
  pos_servo = 6;

  // si el valor analógico es menor a 7.5, led RGB brilla vede
  if(temperature < 7.5){
    dutycycle_red = 255;
    dutycycle_green = 0;
    dutycycle_blue = 255;
    pos_servo = 10;
  }
  
  //si el valor analógico esta entre 7.5 y 15, led RGB brilla amarillo
  else if(temperature <= 15 && temperature >= 7.5 ){
    dutycycle_red = 255;
    dutycycle_green = 180;
    dutycycle_blue = 127;
    pos_servo = 15;
    }

  // si el valor analógico es mayor a 15, led RGB brilla rojo
  else if(temperature > 15){
    dutycycle_red = 0;
    dutycycle_green = 255;
    dutycycle_blue = 255;
    pos_servo = 20;
    }
  
  ledcWrite(PWMchannel_red,   dutycycle_red);
  ledcWrite(PWMchannel_green, dutycycle_green);
  ledcWrite(PWMchannel_blue,  dutycycle_blue);
  ledcWrite(PWMchannel_servo, pos_servo);
}


//-----------------------------------------------------------------------------
// FUNCIONES ADICIONALES
//-----------------------------------------------------------------------------

// config. PWM
//-----------------------------------------------------------------------------
void config_pwm(void){
  // config. pwm para RED
  // pwm channel 0 para led rojo, freq de 144Hz, res. de 8 bits
  ledcSetup(PWMchannel_red, PWMfreq_led, resolution);
  ledcAttachPin(RED, PWMchannel_red);

  // config. pwm para GREEN
  // pwm channel 1 para led verde, freq de 144Hz, res. de 8 bits
  ledcSetup(PWMchannel_green, PWMfreq_led, resolution);
  ledcAttachPin(GREEN, PWMchannel_green);

  // config. pwm para BLUE
  // pwm channel 2 para led azul, freq de 144Hz, res. de 8 bits
  ledcSetup(PWMchannel_blue, PWMfreq_led, resolution);
  ledcAttachPin(BLUE, PWMchannel_blue);

  // config. pwm para servo motor
  // pwm channel 3 para servo motor, freq de 50Hz, res. de 8 bits
  ledcSetup(PWMchannel_servo, PWMfreq_servo, resolution);
  ledcAttachPin(servo, PWMchannel_servo);
}