// ---------------------------------------------------------------------------------------------//
// Istituto Superior Desarrollo Software                                                        //
// Web Site ITES                                                      					//
// Libreria Estación Meteo Desarrollada por alumnos del Instituto                               //
// Contribuyeron con Librerias:											//

#ifndef MeteoItes_h
#define MeteoItes_h

// Librerias que Utilizaremos
#include <Arduino.h>

//Para desabilitar el BROWNOUT funcion que podria sera la causante de PowerOn Reboot when in deep Sleep mode
//https://iotespresso.com/how-to-disable-brownout-detector-in-esp32-in-arduino/
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
/////////////////////////////////

// Libreria Veleta para obtener direccion viento utilizando Magnetometro
#include <QMC5883LCompass.h>

void direccion_viento(int MagneticDeclination); 
//Mode Control (MODE)
const byte qmc5883l_mode_stby = 0x00;
const byte qmc5883l_mode_cont = 0x01;
// Output Data Rate (ODR)
const byte qmc5883l_odr_10hz  = 0x00;
const byte qmc5883l_odr_50hz  = 0x04;
const byte qmc5883l_odr_100hz = 0x08;
const byte qmc5883l_odr_200hz = 0x0C;
// Full Scale Range (RNG)
const byte qmc5883l_rng_2g    = 0x00;
const byte qmc5883l_rng_8g    = 0x10;
// Over Sample Ratio (OSR)
const byte qmc5883l_osr_512   = 0x00;
const byte qmc5883l_osr_256   = 0x40;
const byte qmc5883l_osr_128   = 0x80;
const byte qmc5883l_osr_64    = 0xC0;


// Libreria Reloj tiempo real. Para tener control del tiempo ya que el reloj del microtrolador no 
// lleva la cuenta del tiempo transcurrido.
#include <DS3231.h>

///Sensor Temp 
#include <Adafruit_AHT10.h>

//Sensor Presion Atm
#include <SFE_BMP180.h>
const byte modoPresBMP180 = 3;
const double altitudActual = 171; //Eduardo Castex



//Declaracion de variables a las cuales accederemos desde distintos archivos.
////////////////////////////////////////////clases RELOJ RTCDS3231/////////////////////////////////////
extern RTClib myRTC;
extern DS3231 myDS3231;
////////////////////////////////////////////clases String para el nombre del archivo///////////////////
extern String archivo;
////////////////////////////////////////////clases  para el sensor temp. y hum. ///////////////////
extern Adafruit_AHT10 aht;
////////////////////////////////////////////clases  para el sensor pres          ///////////////////
extern SFE_BMP180 bmp180;

extern QMC5883LCompass compass;
// Declinacion magnética para la prov. de la La Pampa
#define  MAGNETIC_DECLINATION -3;
extern char direction[strlen("NNE") + 1];
///////////////////////////////////////////////Prototipo funciones

// Genera el nombre del archivo
void nombre_archivo(); 


//Obtiene la Temperatura
float sensor_temperatura();
//Obtiene la Humedad
float sensor_humedad();
//Obtiene presión atm
float get_pres();

//Calcula Sensación Térmica
float get_SensTrc(float temp, float vel_viento);

//Calculo Velocidad del Viento
float velocidad_viento(long int Intervalo,float RadioAnemometro);

//Direccion del viento

void direccion_viento();

/*/Manejo de archivos
void readFile(fs::FS &fs, const char * path);
void writeFile(fs::FS &fs, const char * path, const char * message);
void appendFile(fs::FS &fs, const char * path, const char * message);
*/
#endif

/*
 
    Reset Reason List
    case 1  : Serial.println ("Vbat power on reset");break;
    case 3  : Serial.println ("Software reset digital core");break;
    case 4  : Serial.println ("Legacy watch dog reset digital core");break;
    case 5  : Serial.println ("Deep Sleep reset digital core");break;
    case 6  : Serial.println ("Reset by SLC module, reset digital core");break;
    case 7  : Serial.println ("Timer Group0 Watch dog reset digital core");break;
    case 8  : Serial.println ("Timer Group1 Watch dog reset digital core");break;
    case 9  : Serial.println ("RTC Watch dog Reset digital core");break;
    case 10 : Serial.println ("Instrusion tested to reset CPU");break;
    case 11 : Serial.println ("Time Group reset CPU");break;
    case 12 : Serial.println ("Software reset CPU");break;
    case 13 : Serial.println ("RTC Watch dog Reset CPU");break;
    case 14 : Serial.println ("for APP CPU, reseted by PRO CPU");break;
    case 15 : Serial.println ("Reset when the vdd voltage is not stable");break;
    case 16 : Serial.println ("RTC Watch dog reset digital core and rtc module");break;
    default : Serial.println ("NO_MEAN");
 
 */
