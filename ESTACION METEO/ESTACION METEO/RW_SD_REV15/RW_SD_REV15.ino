#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <Wire.h>
#include "rom/rtc.h"
/// LIBRERIA ESTACIÓN METEO DESARROLLADA POR ITES
#include "MeteoItes.h"
#include "MeteoDb.h"




//Version 15
/*
Chip is ESP32-D0WDQ6 (revision 1)
Features: WiFi, BT, Dual Core, 240MHz, VRef calibration in efuse, Coding Scheme None
Crystal is 40MHz
MAC: 08:3a:f2:aa:33:c0

Ver 12: En esta version se utiliza un nombre archivo fijo para evitar la creacion de varios archivos en cada power on reboot etc
Ver 14: Quitamos el switch al inicio y logueamos el motivo del reset en el archivo 
        Si se repite el hangup la proxima accion sera no utilizar un deepsleep y en cambio un delay.
Ver 15: Comentarios en Español. Se reduce el numero de llamados a la funcion SerialPrint
        Se incorpora la escritura en base de datos influxDB
*/


/// SLEEP MODE VARIABLES
#define uS_TO_S_FACTOR 1000000LL  /* Conversion factor for micro seconds to seconds  MAURI LL PARA OBTENER UN uint64_t */
#define TIME_TO_SLEEP  1    
//uint64_t time_to_sleep = TIME_TO_SLEEP * uS_TO_S_FACTOR;

#define TIME_VEL_PICKUP 10000 // tiempo para interrupciones anemómetro en ms

//Formato mensaje
//ID_ESTACION;LAT;LON;Altura; TEMP;SENSACION_TERMICA;HUMEDAD;PRESION;PRESIPITACION_ON_OFF;PRESIPITACION_MM;VEL_VIENTO;DIR_VIENTO;time_t
////////////////////////////////////////////FUNCIONES TARJETA SD/////////////////////////////////////
//Almacena el nombre del archivo en memoria RTC para evitar perderlo cuando pasa a deep_sleep
//Este archivo debe existir de antemano en la tarjeta SD

RTC_DATA_ATTR  char bufer[22] = {"/datos.txt"}; 



 /*
//FUNCION QUE LEE EL ARCHIVO
void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

//ESCRIBE EL ARCHIVO
void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}
*/

//AGREGAR LINEAS EN EL ARCHIVO
void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Escribiendo en archivo: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Fallo alabrir archivo para escritura");
        return;
    }
    if(file.print(message)){
        Serial.println("Datos escritos correctamente");
    } else {
        Serial.println("Fallo en la escritura datos");
    }
    file.flush();
    file.close();
}
/////////////////////////////////////////////////////////////////////////////////

////////////////////////CALCULO DE LA VELOCIDAD DEL VIENTO CON USO DE ITERRUPCIONES///////
long int Vuelta = 0;
void IRAM_ATTR BtnInterrupt() 
{
    Vuelta++;    
}

//Intervalo en segundos y RadioAnemometro en m
float velocidad_viento(long int Intervalo,float RadioAnemometro);
float velocidad_viento(long int Intervalo,float RadioAnemometro)
{
  float Velocidad = 0;
  Vuelta = Vuelta/Intervalo;
  Velocidad = Vuelta * 2 * PI * RadioAnemometro; // hasa aqui en metros/segundo
  Vuelta = 0;
  //Necesitamos km/h por lo que convertimos multiplicando por 3,6
  return(Velocidad*3.6);
}
///////////////////////////////FUNCION CREACION CLASE TARJETA SD Y CONTROL DE ERR////////////////////////////////////////////
uint8_t err=0;
uint8_t cardType; 
void setupSD()
{
  err=SD.begin(5);
  delay(100);
  if(!err)
  {
    return;
  }
  cardType = SD.cardType();
  if(cardType == CARD_NONE)
  {
    return;
  }
}

///SENDO TO SLEEP////////////////
void gotoSleep()
{

  esp_sleep_enable_timer_wakeup( uint64_t(TIME_TO_SLEEP) * uS_TO_S_FACTOR);
  esp_deep_sleep_start(); 

}

//BLINK FOR FEEDBAK////

void blinkFdk(int msg)
{
    int dly=10; //time between blinks
    //msg is the meaning
    switch (msg)
    {
      //Power On Reset
      case 1:
      dly=1000;
      break;
      //DeepSleep WAKEUP
      case 5:
      dly=100;
      break;
      //SD Card err
      case 21:
      dly=2000;
      default :
      dly=2000;
      break;
   }
   for (int i=0;i<3;i++)
   {
      digitalWrite(LED_BUILTIN, HIGH); 
      delay(dly);
      digitalWrite(LED_BUILTIN, LOW); 
      delay(dly);
   }
}

//Obtiene la direccion del viento con uso del mgnetometro
//Clases magnetómetro




///////////////////////////////////////////////////////////////////////////
//La Función setup()se ejecuta solo una sola vez al inicio o después de un reset 
//en ella debemos incluir las rutinas de configuración de los sensores.

void setup()
{
  const int HALL=15;
  pinMode(HALL, INPUT_PULLUP);
  pinMode(2, OUTPUT);
  String InfoMeteo;
  char wind_dir[4];
  char *VelocidadViento;
  float vel_viento=0.0;
  float value;  //Como buffer a la hora de leer y logear;
  esp_sleep_wakeup_cause_t wakeup_reason;
  int rtc_reset_reason=0;
   
  
  Serial.begin(115200);
  
  Vuelta=0;
  attachInterrupt(digitalPinToInterrupt(HALL), BtnInterrupt, FALLING);

  
  
  //Para evitar problemas de hangup when on battery and deep sleep.
  //https://github.com/espressif/arduino-esp32/issues/796
  //Ademas adicionaremos un capacitor en la alimentacion
  delay(5000); 
    
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable   detector brownout que causa falsos deepsleep wakeups
  
  //Create SD class and open it
  setupSD();
 
  // Para ahorrar energía el microprocesador pasa a un estado Deep_Sleep_Mode
  // y en un período configurable (por archivo) el timer lo despierta.
  
  wakeup_reason = esp_sleep_get_wakeup_cause(); 
  rtc_reset_reason = rtc_get_reset_reason(0);


//  if( rtc_reset_reason ==1) 
//  {
    ///////////////************************ INFLUX DB **************///////////////////
    db_connect();
   
    ///////////////***************************************/////////////////////////////

 // }


  // Deep Sleep Reset WakeUp Reason logged
  InfoMeteo= String(rtc_reset_reason); 
  InfoMeteo +="," ;  
  blinkFdk(rtc_reset_reason); //Power on reboot wakeup feedback

  
   
  //La línea a escribir en el archivo se completa, con el llamado a cada función, con el siguiente orden:
  /*
  ID_ESTACION;
  LAT;
  LON;
  Altura;
  TEMP;
  SENSACION_TERMICA;
  HUMEDAD;
  PRESION;
  PRESIPITACION_ON_OFF;
  PRESIPITACION_MM;
  VEL_VIENTO;
  DIR_VIENTO;
  time_t
  */
  
  
  if((err!=0) && (cardType != CARD_NONE))
  {
      
     
     
    //VELOCIDAD DEL VIENTO
    //Espero 10 segundos para que el anemometro mida la velocidad del viento usando interrupciones
    delay(TIME_VEL_PICKUP);
    Serial.println("milliseconds elapsed since start");
    Serial.println(millis());
    vel_viento = velocidad_viento((millis()/1000),0.1);
    
    InfoMeteo += "01,";       //ID_ESTACION;
    InfoMeteo += "-35.9,";    //LATITUD
    InfoMeteo += "-64.3,";    //LONGITUD
    InfoMeteo += "171,";      //ALTURA
    
    //TEMPERATURA
    value = sensor_temperatura();
    db_write("Temperatura",value,2);
    String buff = String(value,1);
    InfoMeteo.concat(buff);
    InfoMeteo +="," ; 
    //Serial.print("Temperatura > ");
    //Serial.println(buff);
    
    value=get_SensTrc(sensor_temperatura(),vel_viento);
    db_write("Sens_Termica",value,2);
    String buff1 = String(value);
    InfoMeteo.concat(buff1);
    InfoMeteo +="," ; 
    //Serial.print("Sens > ");
    //Serial.println(buff1);
    
    value=sensor_humedad();
    db_write("Rel_Humidity",value,2);
    String buff2 = String(value,2);
    InfoMeteo.concat(buff2);
    InfoMeteo +="," ; 
    //Serial.print("Humedad > ");
    //Serial.println(buff2);
    
    value=get_pres();
    db_write("Atm_Pressure",value,2);
    String buff3 = String(value,1);
    //Serial.print("Presion > ");
    //Serial.println(buff3);
    
    InfoMeteo.concat(buff3);
    InfoMeteo +="," ; 
    
    InfoMeteo += "1,";       //PRESIPITACIÓN
    InfoMeteo += "0,";        //PRESIPITACIÓN MM

    
    // Creo un String de caractares con el valor de la velocidad del viento y 1 decimales
    // y lo concateno al String InfoMeteo
    String buff0 = String(vel_viento,1);
    InfoMeteo.concat(buff0);
    InfoMeteo +="," ; 
    //Serial.print("Viento > ");
    //Serial.println(buff0);
    

    //DIRECCIÓN VIENTO
   // Serial.println("Direccion Viento ");
    
    
   direccion_viento();  
   // Serial.print(direction);
    InfoMeteo.concat(direction);
    InfoMeteo += ",";
  
    
    //Lectura del time_t
    Wire.begin();
    delay(1000);
    DateTime now = myRTC.now();
    InfoMeteo.concat( now.unixtime());
    Wire.end();
    
    
    InfoMeteo.concat('\n');     //Nueva Linea
    Serial.println(archivo.c_str());
    Serial.println(InfoMeteo);
    appendFile(SD, bufer, InfoMeteo.c_str());
    InfoMeteo = "";
  }
  else
  {
     blinkFdk(21); //SD MOUNT ERROR
  }

  //Serial.println("Going to sleep now for 3600 sec, bye!! ");
  //Serial.flush(); 
  delay(5000);  //just trying to solve the deep_sleep_wake_up problem
  gotoSleep();
 
}
  
void loop()
{
  
}
