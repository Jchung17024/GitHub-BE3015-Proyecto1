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


//-----------------------------------------------------------------------------
// VARIABLES GLOBALES
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISR
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// CONFIG.
//-----------------------------------------------------------------------------
void setup() {
  // put your setup code here, to run once:
}


//-----------------------------------------------------------------------------
// LOOP PRINCIPAL
//-----------------------------------------------------------------------------
void loop() {
  // put your main code here, to run repeatedly:
}


//-----------------------------------------------------------------------------
// FUNCIONES ADICIONALES
//-----------------------------------------------------------------------------