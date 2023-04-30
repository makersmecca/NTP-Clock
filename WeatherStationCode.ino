#include "DHT.h"
#include "Wire.h"
#include "LiquidCrystal_I2C.h"
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <MQ2.h>

/* LCD CONSTANTS*/

LiquidCrystal_I2C lcd(0x3F, 16, 2); // SDA : D2 ; SCL : D1

#define DHTPIN 0// D3 on NodeMCU is GPIO 0

#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
#define lcd_light 16 //backlight pin D0

/*MODE BUTTON*/

uint8_t mode = D7;
int count=0;

/* NTP TIME CONSTANTS*/

const char *ssid     = "disconnect";
const char *password = "danger440volts";
const long utcOffsetInSeconds = 19800;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP,"asia.pool.ntp.org",utcOffsetInSeconds);
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};


/*MQ2 sensor constants*/
int pin = A0;
int lpg, co, smoke;
MQ2 mq2(pin);


void setup() {
  
  Serial.begin(115200);
  
  mq2.begin();
  
  pinMode(mode,INPUT);
  
//  lcd.init();
  dht.begin();
  lcd.backlight();
  pinMode(lcd_light,OUTPUT);
  digitalWrite(lcd_light,HIGH);
  lcd.setCursor(0,0);
  
  lcd.print("Connecting WiFi");
  WiFi.begin(ssid, password);
  lcd.setCursor(0,1);
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
    
    lcd.print(".");
  }

  timeClient.begin();
  //+lcd.clear();

}
void time()
{
  timeClient.update();

  Serial.println(timeClient.getFormattedTime());

  lcd.setCursor(0,0);
  lcd.print("TIME :");
  lcd.print(timeClient.getFormattedTime());
  lcd.setCursor(0,1);
  //lcd.print("DATE : ");
  lcd.print(daysOfTheWeek[timeClient.getDay()]);
  if(int(timeClient.getHours())<7)
    digitalWrite(lcd_light,LOW);
  else
    digitalWrite(lcd_light,HIGH);
  delay(1000);
}
void temp()
{
  lcd.clear();
  float h = dht.readHumidity();
   float t = dht.readTemperature();
   float f = dht.readTemperature(true);
   if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    lcd.setCursor(0,0);
    lcd.print("Sensor Error");
    return;
   }
   Serial.println(F("Humidity: "));
   Serial.print(h);
   lcd.setCursor(0,0);
   lcd.print("HUM: ");
   lcd.print(h);
   lcd.print("%");
   Serial.print(F("%  Temperature: "));
   Serial.print(t);
   Serial.print(F("°C "));
   lcd.setCursor(0,1);
   lcd.print("TEM: ");
   lcd.print(t);
   lcd.print("C");
   delay(500);
}

void air_quality()
{
  float* values= mq2.read(true);
  lpg = mq2.readLPG();
  co = mq2.readCO();
  smoke = mq2.readSmoke();
  
  lcd.setCursor(0,0);
  lcd.print("CO: ");
  lcd.print(co);
  
  lcd.setCursor(0,1);
  lcd.print("Smoke: ");
  lcd.print(smoke);
  
  delay(3000);
}

void loop() {
   if(digitalRead(mode)==HIGH)
   {
      count+=1;
   }
   

   if (count==0)
   {
     lcd.clear();
     time();
   }
   
   else if(count==1)
   {
    lcd.clear();
    temp();
   }
   

   else if(count==2)
   {
    lcd.clear();
    air_quality();
    count=0;
   }

}
