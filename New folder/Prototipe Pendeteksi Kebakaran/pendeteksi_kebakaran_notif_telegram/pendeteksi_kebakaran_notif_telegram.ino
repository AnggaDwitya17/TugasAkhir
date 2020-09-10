#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include "CTBot.h"
#include "DHT.h"

#define pinIR 19
#define relay 18
#define buzzer 5
#define led 17
#define dhtPin 16
#define DHTTYPE DHT11
#define sensorMQ 36
#define rasp 23

int lcdColumns = 16;
int lcdRows = 2;
int adcMQ = 0;
int api;
int kondisi;
int bacaKamera;

LiquidCrystal_I2C lcd(0x27,lcdColumns,lcdRows);
CTBot myBot;
DHT dht(dhtPin, DHTTYPE);

String ssid = "angga";     // REPLACE mySSID WITH YOUR WIFI SSID
String pass = "12345678"; // REPLACE myPassword YOUR WIFI PASSWORD, IF ANY
String token = "1287181320:AAHBtzewedVLlj-jtxLceoMH5lV_mi_wwps";   // REPLACE myToken WITH YOUR TELEGRAM BOT TOKEN						

void setup() {
	// initialize the Serial
  pinMode(pinIR, INPUT_PULLUP);
  pinMode(sensorMQ, INPUT);
  pinMode(relay, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(rasp, INPUT);

  dht.begin();
  
	Serial.begin(115200);
	Serial.println("Starting TelegramBot...");

	// connect the ESP8266 to the desired access point
	myBot.wifiConnect(ssid, pass);

	// set the telegram bot token
	myBot.setTelegramToken(token);

	// check if all things are ok
	if (myBot.testConnection())
		Serial.println("\ntestConnection OK");
	else
		Serial.println("\ntestConnection NOK");

  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
  
  lcd.setCursor(0,0);
  lcd.print("Assalamualaikum");
  delay(2000);
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("Deteksi");
  lcd.setCursor(2,1);
  lcd.print("Kebakaran");
  delay(2000);             
  lcd.clear();

}

int naive_bayes(float suhu, int api, float asap, int kamera);

void loop() {
    TBMessage msg;
    
	// a variable to store telegram message data
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  Serial.print("Suhu = ");
  Serial.print(t);
  Serial.print("\t");
  delay(100);
  
  adcMQ = analogRead(sensorMQ); 
  Serial.print("Asap = ");
  Serial.print(adcMQ);
  Serial.print("\t");
  delay(100);

  api = digitalRead(pinIR);
  Serial.print("Api = ");
  Serial.print(api);
  Serial.print("\t");
  delay(100);

  bacaKamera = digitalRead(rasp);
  Serial.print("cam = ");
  Serial.println(bacaKamera);
  delay(100);
  
	// if there is an incoming message...
	if (myBot.getNewMessage(msg)) {

		if (msg.text.equalsIgnoreCase("Cek")) { 
		    kondisi = naive_bayes(t,api,adcMQ, bacaKamera);
		    if (kondisi == 1){
            lcd.clear();
            digitalWrite(relay, HIGH);
            delay(50);
            digitalWrite(buzzer, HIGH);
            delay(50);
            digitalWrite(led, HIGH);
            delay(50);
            lcd.setCursor(3,0);
            lcd.print("Bahaya");
            
            myBot.sendMessage(msg.sender.id, "Bahaya, Ada indikasi Kebakaran");
            delay(500);
      
        }else {
            lcd.clear();
            digitalWrite(relay, LOW);
            digitalWrite(buzzer, LOW);
            digitalWrite(led, LOW);
            lcd.setCursor(3,0);
            lcd.print("Aman");
            myBot.sendMessage(msg.sender.id, "Aman Terkendali");
            delay(500);     
        }
			
		}
		else {                                                    // otherwise...
			// generate the message for the sender
			String reply;
			reply = (String)"Selamat datang " + (String)" Angga" + (String)" di Alat Pendeteksi Kebakaran, Silahkan tulis Cek untuk mengetahui kondisi rumah";
			myBot.sendMessage(msg.sender.id, reply);             // and send it
		}
	} else {
        kondisi = naive_bayes(t,api,adcMQ, bacaKamera);
        if (kondisi == 1){
            lcd.clear();
            digitalWrite(relay, HIGH);
            delay(50);
            digitalWrite(buzzer, HIGH);
            delay(50);
            digitalWrite(led, HIGH);
            delay(50);
            lcd.setCursor(3,0);
            lcd.print("Bahaya");
            
            myBot.sendMessage(msg.sender.id, "Bahaya, Ada indikasi Kebakaran");
            delay(500);
      
        }else {
            lcd.clear();
            digitalWrite(relay, LOW);
            digitalWrite(buzzer, LOW);
            digitalWrite(led, LOW);
            lcd.setCursor(3,0);
            lcd.print("Aman");
            delay(100);
        }
	  
	  }
}

int naive_bayes(float suhu, int api, float asap, int kamera){
  //Aman
    float suhuRuangan = 5.714;
    float suhuSedang = 2.857;
    float suhuTinggi = 1.428;

    float adaApi = 1.428;
    float tApi = 8.571;

    float tAsap = 5.714;
    float asapSedang = 1.818;
    float asapTinggi = 1.428;

    float kameraDetek = 1.428;
    float kameraTdetek = 8.571;

    // Bahaya
    float suhu_ruangan = 1.818;
    float suhu_sedang = 3.636;
    float suhu_tinggi = 4.545;

    float ada_api = 6.363;
    float t_api = 3.636;

    float t_asap = 0.909;
    float asap_sedang = 3.636;
    float asap_tinggi = 5.454;

    float kamera_detek = 6.363;
    float kamera_tdetek = 3.636;

    float psuhuA, psuhuB;
    float papiA, papiB;
    float pasapA, pasapB;
    float kameraA, kameraB;
    float PXA, PXB;

    if (suhu <=30){
      psuhuA = suhuRuangan;
      psuhuB = suhu_ruangan;
    } else if (suhu >30 && suhu <=35){
      psuhuA = suhuSedang;
      psuhuB = suhu_sedang;
    } else if (suhu >35){
      psuhuA = suhuTinggi;
      psuhuB = suhu_tinggi;
    }

    if (api == LOW){
      papiA = adaApi;
      papiB = ada_api;
    } else {
      papiA = tApi;
      papiB = t_api;
    }

    if (asap <= 800){
      pasapA = tAsap;
      pasapB = t_asap;
    }else if (asap > 800 && asap <= 1000){
      pasapA = asapSedang;
      pasapB = asap_sedang;
    } else if (asap > 1000){
      pasapA = asapTinggi;
      pasapB = asap_tinggi;
    }

    if (kamera == HIGH){
      kameraA = kamera_detek;
      kameraB = kamera_tdetek;
    } else {
      kameraA = kameraDetek;
      kameraB = kameraTdetek;
    }

    PXA = psuhuA * papiA * pasapA * kameraA;
    PXB = psuhuB * papiB * pasapB * kameraB;

    Serial.print(psuhuA);
    Serial.print("\t");
    Serial.print(psuhuB);
    Serial.print("\t");
    Serial.print(pasapA);
    Serial.print("\t");
    Serial.print(pasapB);
    Serial.print("\t");
    Serial.print(papiA);
    Serial.print("\t");
    Serial.print(papiB);
    Serial.print("\t");
    Serial.print(PXA);
    Serial.print("\t");
    Serial.println(PXB);

    if (PXA > PXB){
      return(0);
    } else {
      return(1);
    }
}
