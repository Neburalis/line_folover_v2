#include <MCP3008.h>
#include "SoftServo.h"
#include "Servo.h"
#include "timer.h"

// debug mode
#define debug

/* Режим работы:
0 - Обычная езда
1 - Просмотр данных переднего датчика 
2 - TODO: Просмотр данных переднего датчика 
3 - TODO: Тест моторов
4 - TODO: Тест импеллера

Для работы любых режимов кроме 0 необходим debug, т.к. он включает/выключает Serial
*/
#define mode 1

// #define enable_imp // if commented - impeller is disable
#define enable_motors // if commented - motors is disable

#define defaultSpeed 150
#define backToLineSpeed 50

// ****************************** pin defines ******************************
// Motor Driver
#define STBY 24
#define MAP 2
#define MBP 3
#define MA1 29
#define MA2 27
#define MB1 26
#define MB2 28

// MCP3008
#define CS_PIN 6
#define CLOCK_PIN 9
#define MOSI_PIN 7
#define MISO_PIN 8

// Other
#define btnPin 4
#define impPin 12  
#define ledPin 13

// ****************************** Sensors ******************************
// back sensor bs (analogRead)
// #define bs // if commented - back sensor is disable

#ifdef bs
#define bSensorAmo 8
int bwhite[bSensorAmo] = { 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000 };
int bblack[bSensorAmo];
int bgrey[bSensorAmo];
int bsensorPin[bSensorAmo] = { A0, A2, A4, A6, A8, A10, A12, A14 };
const int8_t bsensorkoefs[bSensorAmo] = { -4, -3, -2, -1, 1, 2, 3, 4 };
int bdata[bSensorAmo];
#endif

//// front sensor (MCP3008))
#define fSensorAmo 8

int fwhite[fSensorAmo] = { 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000 };
int fblack[fSensorAmo];
int fgrey[fSensorAmo];
const int8_t fsensorkoefs[fSensorAmo] = { -4, -3, -2, -1, 1, 2, 3, 4 };
int fdata[fSensorAmo];

// ****************************** Global objects and vars ******************************
MCP3008 fSensor(CLOCK_PIN, MOSI_PIN, MISO_PIN, CS_PIN); // MCP3008 object (front sensor)
Servo imp; // impeller

bool foutline = false; // outline flag