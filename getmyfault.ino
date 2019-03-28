#include<Ethernet.h>
#include<SPI.h> 
#include <stdio.h>

// EmonLibrary examples openenergymonitor.org, Licence GNU GPL V3
#include "EmonLib.h"                   // Include Emon Library
EnergyMonitor emon1;                   // Create an instance
EnergyMonitor emon2;                   // Create an instance
EnergyMonitor emon3;                   // Create an instance


int sensorVoltPina = A0;    // select the input pin for the ZMPT
int sensorVoltPinb = A1;    // select the input pin for the ZMPT
int sensorVoltPinc = A2;    // select the input pin for the ZMPT
//int sensorCurPina = A3;
//int sensorCurPinb = A4;
//int sensorCurPinc = A5;
float voltagea = 0;  // variable to store the value coming from the sensor
float voltageb = 0;
float voltagec = 0;
float sensorVoltagea = 0;
float sensorVoltageb = 0;
float sensorVoltagec = 0;
float volta = 0;
float voltb = 0;
float voltc = 0;
unsigned long Time;

IPAddress server(192,168,1,100);  //ip Address of the server you will connect to

String location1 = "/ajax/getdata1.php HTTP/1.0";
String location2 = "/ajax/getdata2.php HTTP/1.0";
String location3 = "/ajax/getdata3.php HTTP/1.0";
String location4 = "/ajax/getdata4.php HTTP/1.0";
String location5 = "/ajax/getdata5.php HTTP/1.0";
String location6 = "/ajax/getdata6.php HTTP/1.0";
String location7 = "/ajax/getdata7.php HTTP/1.0";


// if need to change the MAC address (Very Rare)
byte mac[] = { 0x50, 0xb7, 0xc3, 0x83, 0x72, 0x5f }; 

EthernetClient client;

char inString[32]; // string for incoming serial data
int stringPos = 0; // string index counter
boolean startRead = false; // is reading?

//float currenta=0;
//float currentb=0;
//float currentc=0;

//float lamp;
//float buzzer;
float fault;
int led_red = 2;
int buzzer_pin = 4;
int led_yellow = 7;
int led_green = 8;


void setup(){
  pinMode(led_red, OUTPUT);
  pinMode(buzzer_pin, OUTPUT);
  pinMode(led_yellow, OUTPUT);
  pinMode(led_green, OUTPUT);
  digitalWrite(led_green,HIGH);
  Ethernet.begin(mac);
  Serial.begin(9600);
  emon1.current(3, 60);
  emon2.current(4, 60);
  emon3.current(5, 60);
}

void loop(){
  //delay(1000);
  // read the value from the sensor:
  digitalWrite(led_green,HIGH);
  sensorVoltagea = analogRead(sensorVoltPina);
  sensorVoltageb = analogRead(sensorVoltPinb);
  sensorVoltagec = analogRead(sensorVoltPinc);
  double currenta=emon1.calcIrms(1480);
  double currentb=emon2.calcIrms(1480);
  double currentc=emon3.calcIrms(1480);
  volta = map(sensorVoltagea, 0, 1023, -220, 220);
  voltagea = (volta*4.965*sqrt(2));
  voltb = map(sensorVoltageb, 0, 1023, -220, 220);
  voltageb = (voltb*4.965*sqrt(2));
  voltc = map(sensorVoltagec, 0, 1023, -220, 220);
  voltagec = (voltc*4.965*sqrt(2));
  Time = millis();

  if (client.connect(server, 80)) {
    Serial.println("-> Connected");
    if(client.connected()){
    digitalWrite(led_yellow, HIGH);
    client.print( "GET /input_data.php?");
    
    client.print("avolt=");
    client.print(voltagea);
    client.print("&bvolt=");
    client.print(voltageb);
    client.print("&cvolt=");
    client.print(voltagec);
    client.print("&aarus=");
    client.print(currenta);
    client.print("&barus=");
    client.print(currentb);
    client.print("&carus=");
    client.print(currentc);

    client.println( " HTTP/1.1");
    client.println( "Host: 192.168.1.100" );
    client.println( "Connection: close" );
    client.println();
    client.println();
    client.stop();
    delay(100);
    
  }
  }
  else {
 
    Serial.println("--> connection failed/n");
    digitalWrite(led_yellow, LOW);
  }
 /* String pageValue1 = connectAndRead(location1); 
  fault = strToFloat(pageValue1);
  if (fault==1) //If fault happens
  {
    Serial.println("Apapun");
    digitalWrite(led_red, HIGH);
    digitalWrite(buzzer_pin, HIGH);
    delay(1000);
    digitalWrite(led_red, LOW);
    digitalWrite(buzzer_pin, LOW);
    delay(1000);
  }
  else
  {
    digitalWrite(led_red, LOW); //No Faults, or fault has been cleared
    digitalWrite(buzzer_pin, LOW);
  }*/
  //Serial.println("There is a Fault"); 
}

String connectAndRead(String loc){

  if (client.connect(server, 80)) {
    //Serial.println("connected");
    client.print("GET ");
    client.println(loc);
    return readPage(); 

  }else{
    return "connection failed";
  }

}
float strToFloat(String str) {
  char carray[str.length() + 1];           
  str.toCharArray(carray, sizeof(carray)); 
  return atof(carray);
}
String readPage(){
  
  stringPos = 0;
  memset( &inString, 0, 32 ); 

  while(true){

    if (client.available()) {
      char c = client.read();

      if (c == '<' ) { 
        startRead = true; 
      }else if(startRead){

        if(c != '>'){ 
          inString[stringPos] = c;
          stringPos ++;
        }else{
          //selesai mengambil data
          startRead = false;
          client.stop();
          //client.flush();
          
          return inString;

        }

      }
    }

  }

}
