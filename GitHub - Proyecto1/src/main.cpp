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
#include "AdafruitIO_WiFi.h"

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

// username y key para el dashboard de adafruit IO
#define IO_USERNAME  "Chung17024"
#define IO_KEY       "aio_vTSZ60wBJ65qzXgwEDq7ROzFVOLZ"
#define WIFI_SSID    "WeeFee"
#define WIFI_PASS    "Wallao2231"

AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);

//-----------------------------------------------------------------------------
// PROTOTIPO DE FUNCIONES
//-----------------------------------------------------------------------------
void config_pwm(void);
void desplegar7seg_decenas(uint8_t digit);
void desplegar7seg_unidades(uint8_t digit);
void desplegar7seg_decimales(uint8_t digit);

//-----------------------------------------------------------------------------
// VARIABLES GLOBALES
//-----------------------------------------------------------------------------
int adcTemperature = 0;
float temperature = 0;
int dutycycle_red = 0;
int dutycycle_green = 0;
int dutycycle_blue = 0;
int pos_servo = 0;
int temp = 0;
int digit_decenas;
int digit_unidades;
int digit_decimales;
unsigned int lastime;
unsigned int sampletime = 3000;

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
AdafruitIO_Feed *Temperatura = io.feed("Temperatura");
void setup() {
  // baud rate de 115200Hz
  Serial.begin(115200);

  // config. de WIFI y ADAFRUIT IO
  //---------------------------------------------------------------------------
  while (!Serial)
    ;
  Serial.print("Connecting to Adafruit IO");

  // connect to io.adafruit.com
  io.connect();

  // wait for a connection
  while (io.status() < AIO_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  // we are connected
  Serial.println();
  Serial.println(io.statusText());

  // config. pines como entradas y salidas
  // el boton de read_temp es entrada y los displays de 7 seg. son salidas
  pinMode(read_temp, INPUT_PULLUP);
  pinMode(display0,  OUTPUT);     // display 0 despliega decenas
  pinMode(display1,  OUTPUT);     // display 1 despliega unidades
  pinMode(display2,  OUTPUT);     // display 2 despliega decimales

  // config. todos los leds de los display de 7 seg. como salidas digitales
  pinMode(a,  OUTPUT);
  pinMode(b,  OUTPUT);
  pinMode(c,  OUTPUT);
  pinMode(d,  OUTPUT);
  pinMode(e,  OUTPUT);
  pinMode(f,  OUTPUT);
  pinMode(g,  OUTPUT);
  pinMode(dp, OUTPUT);

// disp. de 7 seg. anodo comun. 1 = off, 0 = on. todos los leds off al inicio
  digitalWrite(a,  1);
  digitalWrite(b,  1);
  digitalWrite(c,  1);
  digitalWrite(d,  1);
  digitalWrite(e,  1);
  digitalWrite(f,  1);
  digitalWrite(g,  1);
  digitalWrite(dp, 1);

  config_pwm();

  // isr del boton conectado en pin 13, interrupcion ISR_readtemp, modo falling
  attachInterrupt(read_temp, ISR_readTemp, FALLING);

  // todos los display off inicialmente
  digitalWrite(display0,0);
  digitalWrite(display1,0);
  digitalWrite(display2,0);
}


//-----------------------------------------------------------------------------
// LOOP PRINCIPAL
//-----------------------------------------------------------------------------
void loop() {
  // una alternativa al delay();
  // cada 3s se actualiza el dashboard de adafruit
  if (millis() - lastime >= sampletime)
    {
    io.run();
    Serial.print("sending Temperatura -> ");
    Serial.println(temperature, 1);
    Serial.print(" °C  ");
    Temperatura->save(temperature); // guarda los datos en el cloud de adafruit io

    Serial.print(" \n");
    Serial.print("Enviando ");
    Serial.print(" \n");

    lastime = millis();
    }
    
  // semaforo y reloj de temperatura
  //-----------------------------------------------------------------------------
  // dutycycle de 255 = off y 0 = brightest, led RGB off inicialmente
  dutycycle_red = 255;
  dutycycle_green = 255;
  dutycycle_blue = 255;
  // 6 es el valor para 0 grados del servo 
  pos_servo = 6;

  // metodo para desplegar un valor en 3 displays de 7 seg.
  // para desplegar decenas
  temp = temperature * 10;
  digit_decenas = (temp)/100;
  desplegar7seg_decenas(digit_decenas);
  delay(5);
  // para desplegar unidades
  temp = temp - (digit_decenas*100);
  digit_unidades = temp/10;
  desplegar7seg_unidades(digit_unidades);
  delay(5);
  // para desplegar decimales
  temp = temp - (digit_unidades*10);
  digit_decimales = temp;
  desplegar7seg_decimales(digit_decimales);
  delay(5);

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

// func. para desplegar en displays los valores analógicos 
//-----------------------------------------------------------------------------
void desplegar7seg_decenas(uint8_t digit){
  // se prende el display 0 y apaga el resto, este despliega valor de decenas
  digitalWrite(display0,1);
  digitalWrite(display1,0);
  digitalWrite(display2,0);

  /*a continuacion se ven las combinaciones de pines que se deben encender
  para desplegar un número de 0 a 9 en el display
  esto o aplica para decenas, unidades y decimales
  */
  switch (digit)
  {
  case 0:
    digitalWrite(a,  0);
    digitalWrite(b,  0);
    digitalWrite(c,  0);
    digitalWrite(d,  0);
    digitalWrite(e,  0);
    digitalWrite(f,  0);
    digitalWrite(g,  1);
    digitalWrite(dp,  1);
    break;
  case 1:
    digitalWrite(a,  1);
    digitalWrite(b,  0);
    digitalWrite(c,  0);
    digitalWrite(d,  1);
    digitalWrite(e,  1);
    digitalWrite(f,  1);
    digitalWrite(g,  1);
    digitalWrite(dp,  1);
    break;

  case 2:
    digitalWrite(a,  0);
    digitalWrite(b,  0);
    digitalWrite(c,  1);
    digitalWrite(d,  0);
    digitalWrite(e,  0);
    digitalWrite(f,  1);
    digitalWrite(g,  0);
    digitalWrite(dp,  1);
    break;

   case 3:
    digitalWrite(a,  0);
    digitalWrite(b,  0);
    digitalWrite(c,  0);
    digitalWrite(d,  0);
    digitalWrite(e,  1);
    digitalWrite(f,  1);
    digitalWrite(g,  0);
    digitalWrite(dp,  1);
    break;

  case 4:
    digitalWrite(a,  1);
    digitalWrite(b,  0);
    digitalWrite(c,  0);
    digitalWrite(d,  1);
    digitalWrite(e,  1);
    digitalWrite(f,  0);
    digitalWrite(g,  0);
    digitalWrite(dp,  1);
    break;

  case 5:
    digitalWrite(a,  0);
    digitalWrite(b,  1);
    digitalWrite(c,  0);
    digitalWrite(d,  0);
    digitalWrite(e,  1);
    digitalWrite(f,  0);
    digitalWrite(g,  0);
    digitalWrite(dp,  1);
    break;

  case 6:
    digitalWrite(a,  0);
    digitalWrite(b,  1);
    digitalWrite(c,  0);
    digitalWrite(d,  0);
    digitalWrite(e,  0);
    digitalWrite(f,  0);
    digitalWrite(g,  0);
    digitalWrite(dp,  1);
    break;

  case 7:
    digitalWrite(a,  0);
    digitalWrite(b,  0);
    digitalWrite(c,  0);
    digitalWrite(d,  1);
    digitalWrite(e,  1);
    digitalWrite(f,  1);
    digitalWrite(g,  1);
    digitalWrite(dp,  1);
    break;

  case 8:
    digitalWrite(a,  0);
    digitalWrite(b,  0);
    digitalWrite(c,  0);
    digitalWrite(d,  0);
    digitalWrite(e,  0);
    digitalWrite(f,  0);
    digitalWrite(g,  0);
    digitalWrite(dp,  1);
    break;

  case 9:
    digitalWrite(a,  0);
    digitalWrite(b,  0);
    digitalWrite(c,  0);
    digitalWrite(d,  0);
    digitalWrite(e,  1);
    digitalWrite(f,  0);
    digitalWrite(g,  0);
    digitalWrite(dp,  1);
    break;
    
  default:
      break;
  }
}
void desplegar7seg_unidades(uint8_t digit){
  // se prende el display 1 y apaga el resto, este despliega valor de unidades
  digitalWrite(display0, 0);
  digitalWrite(display1, 1);
  digitalWrite(display2, 0);

  /*a continuacion se ven las combinaciones de pines que se deben encender
  para desplegar un número de 0 a 9 en el display
  esto o aplica para decenas, unidades y decimales
  */
  switch (digit)
  {
  case 0:
    digitalWrite(a,  0);
    digitalWrite(b,  0);
    digitalWrite(c,  0);
    digitalWrite(d,  0);
    digitalWrite(e,  0);
    digitalWrite(f,  0);
    digitalWrite(g,  1);
    digitalWrite(dp,  0);
    break;
  case 1:
    digitalWrite(a,  1);
    digitalWrite(b,  0);
    digitalWrite(c,  0);
    digitalWrite(d,  1);
    digitalWrite(e,  1);
    digitalWrite(f,  1);
    digitalWrite(g,  1);
    digitalWrite(dp,  0);
    break;

  case 2:
    digitalWrite(a,  0);
    digitalWrite(b,  0);
    digitalWrite(c,  1);
    digitalWrite(d,  0);
    digitalWrite(e,  0);
    digitalWrite(f,  1);
    digitalWrite(g,  0);
    digitalWrite(dp,  0);
    break;

   case 3:
    digitalWrite(a,  0);
    digitalWrite(b,  0);
    digitalWrite(c,  0);
    digitalWrite(d,  0);
    digitalWrite(e,  1);
    digitalWrite(f,  1);
    digitalWrite(g,  0);
    digitalWrite(dp,  0);
    break;

  case 4:
    digitalWrite(a,  1);
    digitalWrite(b,  0);
    digitalWrite(c,  0);
    digitalWrite(d,  1);
    digitalWrite(e,  1);
    digitalWrite(f,  0);
    digitalWrite(g,  0);
    digitalWrite(dp,  0);
    break;

  case 5:
    digitalWrite(a,  0);
    digitalWrite(b,  1);
    digitalWrite(c,  0);
    digitalWrite(d,  0);
    digitalWrite(e,  1);
    digitalWrite(f,  0);
    digitalWrite(g,  0);
    digitalWrite(dp,  0);
    break;

  case 6:
    digitalWrite(a,  0);
    digitalWrite(b,  1);
    digitalWrite(c,  0);
    digitalWrite(d,  0);
    digitalWrite(e,  0);
    digitalWrite(f,  0);
    digitalWrite(g,  0);
    digitalWrite(dp,  0);
    break;

  case 7:
    digitalWrite(a,  0);
    digitalWrite(b,  0);
    digitalWrite(c,  0);
    digitalWrite(d,  1);
    digitalWrite(e,  1);
    digitalWrite(f,  1);
    digitalWrite(g,  1);
    digitalWrite(dp,  0);
    break;

  case 8:
    digitalWrite(a,  0);
    digitalWrite(b,  0);
    digitalWrite(c,  0);
    digitalWrite(d,  0);
    digitalWrite(e,  0);
    digitalWrite(f,  0);
    digitalWrite(g,  0);
    digitalWrite(dp,  0);
    break;

  case 9:
    digitalWrite(a,  0);
    digitalWrite(b,  0);
    digitalWrite(c,  0);
    digitalWrite(d,  0);
    digitalWrite(e,  1);
    digitalWrite(f,  0);
    digitalWrite(g,  0);
    digitalWrite(dp,  0);
    break;
    
  default:
      break;
  }
}
void desplegar7seg_decimales(uint8_t digit){
  // se prende el display 2 y apaga el resto, este despliega valor de decimales
  digitalWrite(display0, 0);
  digitalWrite(display1, 0);
  digitalWrite(display2, 1);

  /*a continuacion se ven las combinaciones de pines que se deben encender
  para desplegar un número de 0 a 9 en el display
  esto o aplica para decenas, unidades y decimales
  */
  switch (digit)
  {
  case 0:
    digitalWrite(a,  0);
    digitalWrite(b,  0);
    digitalWrite(c,  0);
    digitalWrite(d,  0);
    digitalWrite(e,  0);
    digitalWrite(f,  0);
    digitalWrite(g,  1);
    digitalWrite(dp,  1);
    break;
  case 1:
    digitalWrite(a,  1);
    digitalWrite(b,  0);
    digitalWrite(c,  0);
    digitalWrite(d,  1);
    digitalWrite(e,  1);
    digitalWrite(f,  1);
    digitalWrite(g,  1);
    digitalWrite(dp,  1);
    break;

  case 2:
    digitalWrite(a,  0);
    digitalWrite(b,  0);
    digitalWrite(c,  1);
    digitalWrite(d,  0);
    digitalWrite(e,  0);
    digitalWrite(f,  1);
    digitalWrite(g,  0);
    digitalWrite(dp,  1);
    break;

   case 3:
    digitalWrite(a,  0);
    digitalWrite(b,  0);
    digitalWrite(c,  0);
    digitalWrite(d,  0);
    digitalWrite(e,  1);
    digitalWrite(f,  1);
    digitalWrite(g,  0);
    digitalWrite(dp,  1);
    break;

  case 4:
    digitalWrite(a,  1);
    digitalWrite(b,  0);
    digitalWrite(c,  0);
    digitalWrite(d,  1);
    digitalWrite(e,  1);
    digitalWrite(f,  0);
    digitalWrite(g,  0);
    digitalWrite(dp,  1);
    break;

  case 5:
    digitalWrite(a,  0);
    digitalWrite(b,  1);
    digitalWrite(c,  0);
    digitalWrite(d,  0);
    digitalWrite(e,  1);
    digitalWrite(f,  0);
    digitalWrite(g,  0);
    digitalWrite(dp,  1);
    break;

  case 6:
    digitalWrite(a,  0);
    digitalWrite(b,  1);
    digitalWrite(c,  0);
    digitalWrite(d,  0);
    digitalWrite(e,  0);
    digitalWrite(f,  0);
    digitalWrite(g,  0);
    digitalWrite(dp,  1);
    break;

  case 7:
    digitalWrite(a,  0);
    digitalWrite(b,  0);
    digitalWrite(c,  0);
    digitalWrite(d,  1);
    digitalWrite(e,  1);
    digitalWrite(f,  1);
    digitalWrite(g,  1);
    digitalWrite(dp,  1);
    break;

  case 8:
    digitalWrite(a,  0);
    digitalWrite(b,  0);
    digitalWrite(c,  0);
    digitalWrite(d,  0);
    digitalWrite(e,  0);
    digitalWrite(f,  0);
    digitalWrite(g,  0);
    digitalWrite(dp,  1);
    break;

  case 9:
    digitalWrite(a,  0);
    digitalWrite(b,  0);
    digitalWrite(c,  0);
    digitalWrite(d,  0);
    digitalWrite(e,  1);
    digitalWrite(f,  0);
    digitalWrite(g,  0);
    digitalWrite(dp,  1);
    break;
    
  default:
      break;
  }
}
