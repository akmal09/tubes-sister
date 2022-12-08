//Include the library files
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <MQ2.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>


//Constants
#define DHTPIN 32     
#define DHTTYPE DHT22   
#define sensorGas 26
#define sensorApi 25
#define buzzer 35
#define sensorPir 33
#define led 27
LiquidCrystal_I2C lcd(0x27, 16, 2);

MQ2 mq2(sensorGas);
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino
double humidityValue;  //Stores humidity value
double temperatureValue; 
int lpgValue;
bool firesensorDigitalValue = false;
int pirSensorDigitalValue = 0;
String isPeopleInKitchen = "Tidak Ada Orang";
String fireAlert = "Tidak Ada Api";
String lpgAlert = "Tidak Ada Gas";

// Auth token
char auth[] = "pIerif1ixJzNQRJlyTBoL8DYWgAngj9j";
BlynkTimer timer; 

//Enter your WIFI SSID and password
char ssid[] = "Kosan Maba";
char pass[] = "12345678";
unsigned long lastMillis;


void myTimer() 
{
  Blynk.virtualWrite(V14, temperatureValue);
  Blynk.virtualWrite(V32, humidityValue);
  Blynk.virtualWrite(V26, lpgAlert);
  Blynk.virtualWrite(V25, fireAlert);
  Blynk.virtualWrite(V33, isPeopleInKitchen);
  mq2.begin();
}

void setup(){
  // Debug console
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  pinMode(sensorApi, INPUT);
  pinMode(sensorPir, INPUT);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  timer.setInterval(1000L, myTimer); 
  dht.begin();
  lcd.init();
  lcd.backlight();
}

void loop(){
  checkPeople();
  humidityValue = dht.readHumidity();
  temperatureValue = dht.readTemperature();
  lpgValue = mq2.readLPG();
  firesensorDigitalValue = digitalRead(sensorApi);
  pirSensorDigitalValue = digitalRead(sensorPir);

  if(lpgValue !=0 ){
    lpgAlert = "Gas Bocor";
    if(firesensorDigitalValue == HIGH){
      tone(buzzer, 293);
    }
  }else{
    lpgAlert = "Gas Aman";
  }

  if (millis() - lastMillis >= 1*60*1000UL) 
  {
   if(pirSensorDigitalValue == 0){
    isPeopleInKitchen = "Tidak Ada Orang";
   }
   lastMillis = millis();  //get ready for the next iteration
  }

  if(pirSensorDigitalValue == 1){
    isPeopleInKitchen = "Ada Orang";
  }

  if(firesensorDigitalValue){
    fireAlert = "Ada Api";
  }else{
    fireAlert = "Tidak Ada Api";          
  }

  lcd.setCursor(0, 0);
  lcd.print("Suhu : ");
  lcd.print(temperatureValue);
  lcd.setCursor(0, 1); 
  lcd.print("Humidity : ");
  lcd.print(humidityValue);
  

  Blynk.run();
  timer.run(); 
}

void checkPeople(){
  if(isPeopleInKitchen = "Ada Orang"){
    digitalWrite(led, HIGH);    
  }else{
    digitalWrite(led, LOW);    
  }
}

BLYNK_WRITE(V27) 
  {
    int pinValue = param.asInt();
    Serial.print("pin value nya");
    Serial.println(pinValue);
    if(pinValue == 1){
      digitalWrite(led, HIGH);
    }else{
      digitalWrite(led, LOW);      
    }
  }  