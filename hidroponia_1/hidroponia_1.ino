
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

//Relay PINs
#define lights 7

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
int nroFilaMinuto;
int cantidadLecturas = 0;
int cantidadLecturasValidas = 0;
float luzAcumulada = 0;
float promedioLuz = 0;
int lcdOn = -1;
int lightsStatus = 0;

int luzArray[5] = {-1, -1, -1, -1, -1};
int luzArrayIndex = 0;
int qDatosArrays = 0;
int zLuzArray = 0;
float promedioLuzArray = 0;

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
  
//Arrancamos siempre con el relay1 apagado
  pinMode(lights, OUTPUT);
  delay(1000);
  digitalWrite(lights, HIGH);
  lightsStatus = 0;

}

void loop() {

  delay(5000); //intervalo de lectura

  lcd.clear();
  h = dht.readHumidity();
  t = dht.readTemperature();
  l = lightMeter.readLightLevel();
  ht = 0;
  lectura = String(h)+", "+String(t)+", "+String(l)+", "+String(ht); //generamos string para grabar en SD card
  
  //Defino la cantidad de lecturas realizadas
  cantidadLecturas++;

  //Si nos pasamos de la cantidad de valores que puede contener el array, empujamos todo hacia atras y descartamos el primer valor.
  if(luzArrayIndex > 4){
    for(int i = 0; i < 4; i++){
      luzArray[i] = luzArray[i+1];
    }
    luzArray[5] = l;
  }
  else {
    luzArray[luzArrayIndex] = l;
  }
  qDatosArrays++;
  zLuzArray = 0;
    if(luzArrayIndex > 4){
    luzArrayIndex = 0;
  }
  else {
    luzArrayIndex++;
  }
  
  for(int i = 0; i < 5; i++){
    zLuzArray = zLuzArray + luzArray[i];
  }
  
  promedioLuzArray = zLuzArray / ((qDatosArrays < 6) ? qDatosArrays : 5);
   

  //Si es de noche no hacemos nada
  if(promedioLuzArray < 10){
    cantidadLecturasValidas = 0;
    luzAcumulada = 0;
    promedioLuz = 0;
    lcd.print("Lecturas nocturas.");
    lcd.setCursor(0,1);
    lcd.print(promedioLuz);
  }
  else {
    cantidadLecturasValidas++;
    luzAcumulada = luzAcumulada + l;
    promedioLuz = luzAcumulada / cantidadLecturasValidas;
 
    printDisplay(h, cantidadLecturasValidas, promedioLuzArray, promedioLuz);
    escribirSD(lectura);


    if(promedioLuz < 100 && lightsStatus == 1){
      Serial.println("Mantenemos prendidas las luces.");
    }
    else if(promedioLuz < 100 && lightsStatus == 0){
      switchLights(1);
    }
    else {
      switchLights(0);
    }
 
  }

  
 
 } //fin loop

 void switchLights(int _click){
    if(_click == 1) {
      digitalWrite(lights, LOW); 
      lightsStatus = 1;
      Serial.println("Prendemos luces");
    }
    else {
      digitalWrite(lights, HIGH);
      lightsStatus = 0;
      Serial.println("Apagamos luces");
    }
  
 }

 void escribirSD(String lectura){
    if(cantidadEscrituras < 10000){
      //escribir el archivo
      myFile.println(lectura);
      cantidadEscrituras++;
      Serial.println("Escritura sobre archivo.");
    }
    else {
       myFile.close();
       Serial.println("Archivo cerrado por exceso en cantidad de escrituras.");
    }
 }
 
 void printDisplay(int _humedad, int _cantidadLecturasValidas, int _promedioLuzArray, int _promedioLuz){

    lcd.print("H:");
    lcd.print(_humedad);
    lcd.setCursor(9,0);
    lcd.print("Q:");
    lcd.print(_cantidadLecturasValidas);
    lcd.setCursor(0,1);
    //lcd.print("T:");
    //lcd.print(t);
    lcd.print("X:");
    lcd.print(_promedioLuzArray);
    lcd.setCursor(9,1);
    //lcd.print(lux);
    lcd.print("M:");
    lcd.print(_promedioLuz);

 }



