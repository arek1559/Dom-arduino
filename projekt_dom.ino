#include<EEPROM.h>
#include <Keypad.h> //biblioteka do klawiatury 4x4
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "Servo.h"
#include "DHT.h" 
#define DHT11_PIN 8 
Servo serwomechanizm; 
DHT dht; 
LiquidCrystal_I2C lcd(0x27,16,2);
int wartoscAnalog = 0;
byte wartoscZmapowana = 0;        
char info[96];
int sensorPin = A1; // czujnik
int sensorValue = 0; // zmienna
const byte wiersze= 4; //ilość wierszy
const byte kolumny= 4; //ilość kolumn
char mapaklawiszy[wiersze][kolumny]= 
{
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte wiersze_piny[wiersze] = {52, 50, 48, 46}; // wiersze 0-3
byte kolumny_piny[kolumny]= {53, 51, 49, 47}; // kolumny 0-3
int pamiec[5]; // pamięc wciśniętych klawiszy - 5 znaków
volatile int szyfr=0; // szyfr=0 -> brak szyfr=1 jest aktywny
volatile int l=0; // zmienna pomocnicza, licznik znaków
volatile int result=0; // rezultat porównania ciągu 5-ciu znaków
int mykey[5]; // tablica wciśnietych znaków
Keypad myKeypad= Keypad(makeKeymap(mapaklawiszy), wiersze_piny, kolumny_piny,
 wiersze, kolumny); 
#define d 3
#define di 4
#define dio 5
#define diod 6
String data = "";
void setup() {
    Serial.begin(9600);
    lcd.begin();//uruchamia wyswietlacz
      lcd.clear();

    dht.setup(DHT11_PIN); 
         pinMode(d, OUTPUT);
         pinMode(di, OUTPUT);
         pinMode(dio, OUTPUT);
         pinMode(diod, OUTPUT); 
         pinMode(2, OUTPUT); 
         pinMode(52, OUTPUT);
         pinMode(48, OUTPUT);
         pinMode(46, OUTPUT);
         pinMode(13, INPUT_PULLUP); 
         digitalWrite(d, LOW);
         digitalWrite(di, LOW);
         digitalWrite(dio, LOW);
         digitalWrite(diod, LOW);

         serwomechanizm.attach(12); 
         serwomechanizm.write(0); 


}
void temp(){
int wilgotnosc = dht.getHumidity();//wilgotnosc 
int temperatura = dht.getTemperature();//temperatura
if (dht.getStatusString() == "OK") {
delay(dht.getMinimumSamplingPeriod());   
lcd.clear();
lcd.setCursor(0,0);  
lcd.print("Tem:"); 
lcd.print(temperatura);
lcd.print("*C "); 
lcd.print("Wil:");    
lcd.print(wilgotnosc);
lcd.print("%");
Serial.print("Temp=" );
Serial.println(temperatura);
Serial.print("Wil=" );
Serial.println(wilgotnosc); 
lcd.setCursor(0,1);
lcd.print("Dym gaz: "); 
if (wartoscZmapowana < 65) {
lcd.print("OK");
Serial.print("Dym gaz: OK\n");
}

delay(1000);
}
}
void diody(){
if(Serial.available() > 0) { 
data = Serial.readStringUntil('\n');
if(data=="1"){
digitalWrite(3, HIGH);    
}
if (data == "2") {
digitalWrite(4, HIGH);   
}
if (data == "3") {
digitalWrite(5, HIGH);
digitalWrite(6, HIGH);   
}
if(data=="10"){
digitalWrite(d, LOW);    
}
if(data=="20"){
digitalWrite(di, LOW);
}
if(data=="30"){
digitalWrite(dio, LOW); 
digitalWrite(diod, LOW);   
}
if(data=="0"){
digitalWrite(d, LOW);
digitalWrite(di, LOW);
digitalWrite(dio, LOW);
digitalWrite(diod, LOW);    
}     

      }
      delay(50);
      }


void odczyt_EEPROM() // funkcja sprawdza obecnośc szyfru w pamięci EEPROM
  {
   for (int i=0;i<5;i++) {pamiec[i]=EEPROM.read(i);} 
   if(pamiec[0]!=255 && pamiec[1]!=255 && pamiec[2]!=255 && pamiec[3]!=255 
   && pamiec[4]!=255) szyfr=1; else szyfr=0; 
   // jeśli jest KOD to szyfr =1 else szyfr =0
  }
void fnowy_kod() // funkcja ustawia nową kombinację dla szyfru
  {
   for(int i=0;i<5;i++)EEPROM.write(i,255); //kasowanie EEPROM
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("Skasowano stary kod dostepu");
   lcd.setCursor(0,1);
   lcd.print("Wprowadz nowy KOD dostepu: "); 
   while(l<5)
     {
     char keypressed = myKeypad.getKey();
     if (keypressed != NO_KEY)
       {
       EEPROM.write(l,keypressed);l++; // zapis pojedynczego znaku
       lcd.print(keypressed); // komunikacja po RS
       } 
     }
   for(int k=0;k<3;k++){
}
   lcd.print(" ---> Kod dostepu zapisany");l=0; 
  }
void dostep() // funkcja sterująca zamkiem
  {   
lcd.setCursor(0, 0);
lcd.print("BLOKADA ZAMKA");
lcd.setCursor(13, 0);
lcd.print("    ");
lcd.setCursor(0, 1);
lcd.print("WPISZ KOD");
lcd.setCursor(9, 1);
lcd.print("        ");
   while(l<5)
     {
     char keypressed = myKeypad.getKey();
     if (keypressed != NO_KEY)
       {
       mykey[l]=keypressed;l++;
       lcd.clear();
       lcd.setCursor(0,0);
       lcd.print(keypressed);
       
       } 
     }
   if( mykey[0]==pamiec[0] && mykey[1]==pamiec[1] && mykey[2]==pamiec[2] 
   && mykey[3]==pamiec[3] && mykey[4]==pamiec[4] )
     {
     lcd.clear();
     lcd.setCursor(0, 0);
     lcd.print(" OK ");
     lcd.setCursor(0, 1);
     lcd.print("OTWIERAM ZAMEK!");
     serwomechanizm.write(90);delay(3000);serwomechanizm.write(0);
     temp(); 
     // otwieram zamek na 3 sek 
     } 
   // sprawdzamy czy wybrano specjalny kod do zmiany szyfru 
   else 
     {
       result++;
       if(mykey[0]=='*' && mykey[1]=='7' && mykey[2]=='#' && mykey[3]=='A' 
       && mykey[4]=='C' )
         {
         // *7#AC 
         for(int k=0;k<3;k++){
         delay(300);}
         lcd.clear();
         lcd.setCursor(0, 0);
         lcd.print("Wykryto polecenie zmiany kodu dostepu...");
         lcd.setCursor(0, 1);
         lcd.print(" wykonuje procedure zmiany kodu");
         delay(500);l=0;fnowy_kod();
         } 
       else result++;
     }
   if(result>1){ lcd.print("Kod NIEPRAWIDLOWY");
delay(500);result=0;}
   l=0; 
  }

void rosl(){
sensorValue = analogRead(sensorPin);
delay(1000);
Serial.print("wartosc podlania = " );
Serial.println(sensorValue);
if (sensorValue < 15) {
lcd.clear();
digitalWrite(2, HIGH);
lcd.setCursor(0,0);  
lcd.print("Podlej rosliny");
Serial.print("Podlej rosliny" );
delay(500);
}
else{
  Serial.print("Rosliny podlane\n" );
  digitalWrite(2, LOW);
}
}


void gas(){
wartoscAnalog = analogRead(A0);//czujnik gazu
wartoscZmapowana = map(wartoscAnalog, 0, 1023, 0, 100);
sprintf(info, "Wykryto gaz lub dym");
Serial.print("wartosc gaz = " );
Serial.println(wartoscZmapowana);
if (wartoscZmapowana > 65) {
lcd.clear();
digitalWrite(2, HIGH);
lcd.setCursor(0,0);  
lcd.print("WYKRYTO GAZ LUB DYM");
lcd.setCursor(0,1);  
lcd.print("DYM");
Serial.print(info);
Serial.print("\n");
delay(500);
digitalWrite(2, LOW);
}}

void loop() {
rosl();
diody();
gas();
if (digitalRead(13) == HIGH) { 
  temp();
}
else{
odczyt_EEPROM();

delay(30);
if(szyfr==0)fnowy_kod();
else dostep();
 }
if (digitalRead(3) == HIGH){
Serial.print("Swiatlo pokoj 1 wlaczone");
Serial.print("\n");
}
if (digitalRead(4) == HIGH){
Serial.print("Swiatlo pokoj 2 wlaczone");
Serial.print("\n");
}
if (digitalRead(5) == HIGH && digitalRead(6) == HIGH){
Serial.print("Swiatlo pokoj 3 wlaczone");
Serial.print("\n");
}

if (digitalRead(3) == LOW){
Serial.print("Swiatlo pokoj 1 wylaczone");
Serial.print("\n");
}
if (digitalRead(4) == LOW){
Serial.print("Swiatlo pokoj 2 wylaczone");
Serial.print("\n");
}
if (digitalRead(5) == LOW && digitalRead(6) == LOW){
Serial.print("Swiatlo pokoj 3 wylaczone");
Serial.print("\n");
}

}
