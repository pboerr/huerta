
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
//int cantidadLecturas = 0;
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
int cantidadLecturas = 0;
float luzAcumulada = 0;
float promedioLuz = 0;

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

  //Leemos las variables cada 10 segundos

  if(l < 100){
    cantidadLecturas = 0;
    luzAcumulada = 0;
    promedioLuz = 0;
  }
  else {
    cantidadLecturas++;
    luzAcumulada = luzAcumulada + l;
    promedioLuz = luzAcumulada / cantidadLecturas;
 
  //Iniciar logica de acciones con datos acumulados
  //Serial.println(lux);
  lcd.print("H:");
  lcd.print(h);
  lcd.setCursor(0,1);
  lcd.print("T:");
  lcd.print(t);
  lcd.setCursor(9,1);
  //lcd.print(lux);
  lcd.print(promedioLuz);
  delay(60000); //un minuto
  }
 
 } //fin loop


