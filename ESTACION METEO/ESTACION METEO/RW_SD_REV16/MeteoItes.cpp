
/// LIBRERIA ESTACIÓN METEO DESARROLLADA POR ITES
#include "MeteoItes.h"



////////////////////////////////////////////clases RELOJ RTCDS3231/////////////////////////////////////
/*
RTC_DATA_ATTR RTClib myRTC;
RTC_DATA_ATTR DS3231 myDS3231;
*/
//Probablemente sean las que saturan la memoria?
RTClib myRTC;
DS3231 myDS3231;

/////////////////////////////////////////////////////////////////////////////////

//Clases nombre archivo
String archivo; 

//Clase sensor temp. y hum.
Adafruit_AHT10 aht;
//Este sensor debe estar solo en el bus I2C como indica el fabricante. 
//por lo tanto utilizamos en segundo canal del esp32 dedicado para este sensor
#define SDA_2 33
#define SCL_2 32
TwoWire I2Ctwo = TwoWire(1);


//Clase sensor presión atm.
SFE_BMP180 bmp180;

//Función crea nombre archivo
void nombre_archivo()
{
    int minuto,hora,dia,mes;
    bool century=false , h12, PM_Time;
  
    // Se crea el archivo donde almacenar los datos leídos
    // Cada reinicio del equipo crea un archivo nuevo y para no sobreescribir el anterior
    // se incluye la fecha hora y minutos para identificar los distintos archivos.
    archivo.concat("/datos_");
    
    //Comenzamos comunicación con RTC y su lectura
    Wire.begin();
    archivo.concat(myDS3231.getYear());
    
    mes=myDS3231.getMonth(century); // me pasa en century 
    if (mes < 10)
    {
      archivo += '0';
      archivo += mes;
    }
    else 
      archivo += mes;
     
    dia = myDS3231.getDate();
    if (dia < 10)
    {
      archivo += '0';
      archivo += dia;
    }
    else 
      archivo += dia;
   
    //Completo con un 0 adelante la hora para tener siempre la misma longitud de nombre  archivo.
    hora = myDS3231.getHour(h12, PM_Time);
    if (hora < 10)
    {
      archivo += '0';
      archivo += hora;
    }
    else 
      archivo += hora;
    
    // Idem a lo que pasa con las horas
    minuto = myDS3231.getMinute();
    if (minuto < 10)
    {
      archivo += '0';
      archivo += minuto;
    }
    else 
      archivo += minuto;
    Wire.end();
    // Fin lectura RTC
    archivo.concat(".txt");
    archivo.concat("\0");
    
}



///// La funcion que lee temp
float sensor_temperatura()
{
      I2Ctwo.begin(SDA_2, SCL_2, 100000);
      //Serial.println("First call to I2C begin");
      sensors_event_t hum, temp;
        //TEMPERATURA
       //Contructor Clase AHt_10
      
       if (!aht.begin(&I2Ctwo,32))
       {
        I2Ctwo.end(); // Cerramos la comunicacion I2C que abrió el la libreria AHt_10 y nunca cerró
        return(-1.0) ; 
       }
       else
       {
         aht.getEvent(&hum, &temp);
         I2Ctwo.end(); // Cerramos la comunicacion I2C que abrió el la libreria AHt_10 y nunca cerró
         return(temp.temperature);
       }
        
}


//Obtiene la humedad
float sensor_humedad()
{
       I2Ctwo.begin(SDA_2, SCL_2, 100000);
       
       sensors_event_t hum, temp;
       //TEMPERATURA
       //Contructor Clase AHt_10
       if (!aht.begin(&I2Ctwo,32))
       {
        I2Ctwo.end(); // Cerramos la comunicacion I2C que abrió el la libreria AHt_10 y nunca cerró
        return(-1.0) ; 
       }
       else
       {
        aht.getEvent(&hum, &temp);
        I2Ctwo.end(); // Cerramos la comunicacion I2C que abrió el la libreria AHt_10 y nunca cerró
        return(hum.relative_humidity);
       }
}

//Obtiene la presión atm.
float get_pres()
{
    byte status;
    double temp, pres;

 //  Serial.println("Inside get_pres()1");
    
    if (!bmp180.begin())
    {
      Wire.end(); // Cerramos la comunicacion I2C que abrió el la libreria BMP180 y nunca cerró
      return(-1.0);
    }
   //Inicio de lectura de temperatura
    status = bmp180.startTemperature();
    // Comprobar resultado de la lectura temperatura
    // Devuelve 0 si hay error
    // milisegundos que hay que esperar en caso contrario
    if (status != 0)
    {
      // Pausa hasta que finalice la lectura
      delay(status);
      // Obtener temperatura
      // La temperatura se almacena en temp en grados Celsius
      status = bmp180.getTemperature(temp);
      // Comprobar resultado de la temperatura
      // Devuelve 0 si hay error
      // Devuelve 1 si todo ha ido bien
      if (status == 0)
      {
        pres=-1.0;
      }
      else
      {
        // Inicio lectura de presión
        status = bmp180.startPressure(modoPresBMP180);
        // Comprobar resultado de la lectura presión
        // Devuelve 0 si hay error
        // milisegundos que hay que esperar en caso contrario
        if (status != 0)
        {
          // Pausa hata que finalice la lectura
          delay(status);
          // Obtener presión atomosférica
          // La prsión se almacena en la variable pres en milibar (mbar)
          // Esta función requiere la medida de temperatura
          status = bmp180.getPressure(pres, temp);
          // Comprobar resultado de la presión
          // Devuelve 0 si hay error
          // Devuelve 1 si todo ha ido bien
          if (status == 0)
              pres=-1.0;
        }
      }
    }
    Wire.end(); // Cerramos la comunicacion I2C que abrió el la libreria BMP180 y nunca cerró
    return (pres);
}

//Calcula la sensacón térmica
float get_SensTrc(float temp, float vel_viento)
{
    float Sens_Trc;

    //Si hubo error en la lectura de la temperatura devolvemos -1.0
    if (temp==-1.0)
    {
       return(temp);
    }  
    // La fórmula es válida para velocidades del viento superiores a 5km/h
    if (vel_viento < 5.0  || vel_viento ==-1.0 )
     {
    
      return(temp);
      //Serial.println("Sensación Térmica");
     }
    else
      return( 33 + (temp-33)*(0.474+0.454*sqrt(vel_viento)-0.0454*vel_viento));

    
}

// Clase para lectura del sensor campo magnetico terrestre
QMC5883LCompass compass;
char direction[strlen("NNE") + 1];




//funcion que lee efectivamente la direccion del viento
void direccion_viento()
{
  int i;
  int azimuth=0;


  compass.init();
  compass.setCalibration(928, 2888, 2922, 5418, -1, 2373);
  compass.setMode(qmc5883l_mode_cont, qmc5883l_odr_10hz, qmc5883l_rng_8g, qmc5883l_osr_64);
  compass.setSmoothing(10,1); //10 steps and advanced smoothing (will remove max and min values.
  delay(50);
  
  for(i=0;i<10;i++)
  {
  // Read compass values via I2C
    
    if(!compass.read())
    {
      azimuth = compass.getAzimuth()+MAGNETIC_DECLINATION;
      compass.getDirection(direction,azimuth);
      direction[3] = '\0';

    }
  
   else
   {
      
      direction[0]='-';
      direction[1]='-';
      direction[2]='-';
      direction[3] = '\0';
      azimuth=-1;
          
   } 
   delay(100);
  }
}
///////////////////////////////////////////////////////////////////////////


/*
/// SD CARD FUNCTIONS

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

//AGREGAR LINEAS EN EL ARCHIVO
void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}

*/
