
//librerias sensor humedad y temperatura
#include "DHT.h"
#define DHTPIN 2 
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

//librerias de sensor luz
#include <Wire.h> //tambien requerida por LCD 
#include <BH1750.h>

//librerias LCD
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

//librerias SD
#include <SD.h>
#include <SPI.h>

///Inicializaciones
DHT dht(DHTPIN, DHTTYPE);
BH1750 lightMeter;
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
int CS_PIN = 10;
File myFile;

int cantidadEscrituras = 0;
int cantidadLecturas = 0;
float h = 0.0;
float t = 0.0;
float l = 0.0;
float ht = 0.0;
String lectura = "";
//humedad, temperatura, luz y humedadTierra
float sensoresOnline[6][4]; //un registro cada 10 segundos
float sensoresMinuto[60][4]; //un registro por minuto
int hora = 0;
//Datos diarios
float luzAcumuladaDia = 0.0;
//Datos ultima hora
float humedadPromedio = 0.0; 
float temperaturaPromedio = 0.0; 
float humedadTierraPromedio = 0.0; 
int nroFilaOnline;
int nroFilaMinuto;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Inicializando puerto serial, sensores y SD card...");

  if (!SD.begin(4)) {
    Serial.println("SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialization done.");
 
  dht.begin();
  lightMeter.begin();
  lcd.begin(16,2); 
  lcd.backlight();
  lcd.setCursor(0,0);

  //Abrimos archivos para escribir resultados
  myFile = SD.open("datosHuerta.txt", FILE_WRITE);
  if(myFile){
    Serial.print("Writing to test.txt...");
  }
  else{
    Serial.println("Se cago la papa");
  }

}

void loop() {

  delay(10000); //leemos cada 10 segundos
  lcd.clear();
  h = dht.readHumidity();
  t = dht.readTemperature();
  l = lightMeter.readLightLevel();
  ht = 0;
  lectura = String(h)+", "+String(t)+", "+String(l)+", "+String(ht); //generamos string para grabar en SD card

  //Defino la cantidad de lecturas realizadas
  cantidadLecturas++;
  nroFilaOnline = filaOnline(cantidadLecturas);

  //Leemos las variables cada 10 segundos
  
    //Si estamos dentro del minuto
    if(nroFilaOnline > 0){
      //grabamos los valores de las variables en la fila correspondiente
        sensoresOnline[nroFilaOnline][0] = t;
        sensoresOnline[nroFilaOnline][1] = h;
        sensoresOnline[nroFilaOnline][2] = l;
        sensoresOnline[nroFilaOnline][3] = ht;
      //salimos del loop
        continue;
    } 
    else {
      //calculamos el promedio del minuto anterior para cada medicion (i.e. h, t, l, t)
      

      //movemos todos los valores de los datos acumulados
      
      //grabamos los promedios calculados en la ultima linea de los datos acumulados

      //grabamos los valores de las variables en la fila 0

      //reiniciamos el loop
      
    }


  //Datos acumulados: calcular luzAcumuladaDia, humedadPromedio, temperaturaPromedio, humedadTierraPromedio

  //Iniciar logica de acciones con datos acumulados

  
  Serial.println(lux);
  lcd.print("H:");
  lcd.print(h);
  lcd.setCursor(0,1);
  lcd.print("T:");
  lcd.print(t);
  lcd.setCursor(9,1);
  lcd.print(lux);
  delay(60000); //un minuto

/* //Escribir tarjeta SD
  if(cantidadEscrituras < 10){
    //escribir el archivo
    myFile.println(lectura);
    cantidadEscrituras++;
  }
  else {
     myFile.close();
     Serial.println("archivo cerrado. Se cocio la papa");
  }
  */
  
 } //fin loop

//Funcion numero de fila TODO: corregir funciones!
int filaOnline(int cantidadLecturas){
  int nroFila;
  int ultimoDigito;
  ultimoDigito = cantidadLecturas % 10;
  if(cantidadLecturas <= 10){
    nroFila = cantidadLecturas - 1;
  }
  else {
    if(ultimoDigito = 0){
    nroFila = 9;
    } 
   else{
    nroFila = ultimoDigito - 1;
    }
  }
  return nroFila;
 
}

int filaMinuto(int cantidadLecturas){
  int nroFila;
  nroFila = (int) ceil(cantidadLecturas / 60.0);
  return nroFila;
}


