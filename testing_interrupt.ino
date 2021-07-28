//const byte interruptPin = 11;
//int buttonstate
//void setup() {
//
//  Serial.begin(9600);
//  pinMode(interruptPin, INPUT_PULLUP);
//  attachInterrupt(digitalPinToInterrupt(interruptPin), user_button, LOW);
//}
//
//void loop() {
//
//}
//
//void user_button() {
//  static unsigned long last_interrupt_time = 0;
//  unsigned long interrupt_time = millis();
//  // If interrupts come faster than 200ms, assume it's a bounce and ignore
//buttonState = digitalRead(interruptPin) ;
//  if ((interrupt_time - last_interrupt_time > 200) && buttonstate== LOW))
//  {
//
// int a = buttonstate
//Serial.println(a);
//  }
//  last_interrupt_time = interrupt_time;
//  
//  }









#include <SPI.h>
#include <LoRa.h>
String sig1 = "$GNRMC";
String sig2 = "$GNGGA";
boolean stringComplete = false;
String NEWLINE= "\n"; 
int myid= 1;
int packet_cnt = 0;
String inputString, gpsstatus, UTC, STATUS, lorainput,LATITUDE,NS,LONGITUDE,SPEED,DATE,bigpacket;
int load1 = A0;
int load2 = A1;
int load3 = A2;
int load4 = A3;
int load5 = A4;

const byte interruptPin = 11;
int buttonstate ;


float loadVolt1,loadVolt2,loadVolt3,loadVolt4,loadVolt5;
float weight1, weight2,weight3, weight4, weight5, weightTotal ;
float loadOffset1 = 0.0, loadOffset2 = 0.0,loadOffset3 = 0.0, loadOffset4 = 0.0, loadOffset5 = 0.0;


void setup() {
  Serial.begin(9600);
  Serial1.begin(19200);
  while (!Serial);
  pinMode(RFM_TCX_ON,OUTPUT);
  pinMode(RFM_SWITCH,OUTPUT);
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(RFM_SWITCH,1);
  LoRa.setPins(SS,RFM_RST,RFM_DIO0);
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

       
  float total1 = 0.0, total2 = 0.0, total3 = 0.0, total4 = 0.0,total5 = 0.0;
    
  for(int i = 0; i < 100; i++){
    total1 += analogRead(load1)/1024.0 * 3.3;
    total2 += analogRead(load2)/1024.0 * 3.3;
    total3 += analogRead(load3)/1024.0 * 3.3;
    total4 += analogRead(load4)/1024.0 * 3.3;
    total5 += analogRead(load5)/1024.0 * 3.3;
  }

  
  loadOffset1 = total1 / 100.0;
  loadOffset2 = total2 / 100.0;
  loadOffset3 = total3 / 100.0;
  loadOffset4 = total4 / 100.0; 
  loadOffset5 = total5 / 100.0;


 pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), user_button, LOW);
}


void loop() {  
          
  if (Serial1.available() > 0) {  // If anything comes in Serial1 (pins 0 & 1) 
    inputString = Serial1.readStringUntil(0x0a);
    Serial1.flush();
    String sub1 = inputString.substring(0,6);
        
    if(sub1 == sig1){
      String no_header = inputString.substring(7, inputString.length());      
      String Data[100];
      int index = 0;
      // Convert from String Object to String.
      char buf[no_header.length()];
      no_header.toCharArray(buf, sizeof(buf));
      char *p = buf;
      char *str;
      
      while ((str = strtok_r(p, ",", &p)) != NULL)
      { // delimiter is the comma
        String tmp(str);
        //Serial.println(str);
        Data[index] = tmp;
        index++;
      }
      
      UTC = Data[0];
      LATITUDE = Data[2];
      LONGITUDE = Data[4];
      SPEED = Data[6];
      DATE = Data[7];
    }
      else if(sub1 == sig2){
        String no_header = inputString.substring(7, inputString.length());      
        String Data[100];
        int index = 0;
        // Convert from String Object to String.
        char buf[no_header.length()];
        no_header.toCharArray(buf, sizeof(buf));
        char *p = buf;
        char *str;
        
        while ((str = strtok_r(p, ",", &p)) != NULL)
        { // delimiter is the comma
          String tmp(str);
          //Serial.println(str);
          Data[index] = tmp;
          index++;
        }
        STATUS = Data[5];
      }
  }  
  loadVolt1 = analogRead(load1)/1024.0 * 3.3;
  loadVolt2 = analogRead(load2)/1024.0 * 3.3;
  loadVolt3 = analogRead(load3)/1024.0 * 3.3;
  loadVolt4 = analogRead(load4)/1024.0 * 3.3;
  loadVolt5 = analogRead(load5)/1024.0 * 3.3;
    
  
  weight1 = ((loadVolt1 - loadOffset1) * 100.0) / (.8 * 3.3);
  weight2 = ((loadVolt2 - loadOffset2) * 100.0) / (.8 * 3.3);
  weight3 = ((loadVolt3 - loadOffset3) * 100.0) / (.8 * 3.3);
  weight4 = ((loadVolt4 - loadOffset4) * 100.0) / (.8 * 3.3);
  weight5 = ((loadVolt5 - loadOffset5) * 100.0) / (.8 * 3.3);
  weightTotal=weight1+weight2+weight3+weight4+weight5;

  digitalWrite(RFM_SWITCH,1);
  //lorapacket();
  Lorareceived();
    

if((weight1>0) && (weight1<100)){
  char LS1 =0b10000000;
}
if((weight2>0) && (weight2<100)){
  char LS2= 0b01000000;
}
if((weight3>0) && (weight3<100)){
  char LS3= 0b00100000;
}

if((weight4>0) && (weight4<100)){
  char LS4= 0b00010000;
}

if((weight5>0) && (weight5<100)){
    char LS5= 0b0000100;
}
char abc = LS1+LS2+LS3+LS4+LS5
}

char statusbitebinary= 0b11111100;

    
void lorapacket(){
  digitalWrite(RFM_SWITCH,0);
  String weightString = String(int(weightTotal));
  char pkt_cnt_nd_status[2] = {packet_cnt, '0'};
  String tmp(pkt_cnt_nd_status);
  bigpacket=  tmp + weightString + "," + UTC + "," + STATUS + ","+LATITUDE+","+ LONGITUDE + "," + DATE + "," + SPEED +  NEWLINE;
   
  Serial.println("sending data");
  Serial.println(bigpacket);

  LoRa.beginPacket();
  LoRa.println(bigpacket);
  LoRa.endPacket();
  digitalWrite(RFM_SWITCH,1);
}   
    
  
   
void Lorareceived(){
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    while (LoRa.available()) {
      lorainput = LoRa.readStringUntil('\n');  
      String Data[100];
      int index = 0;
      // Convert from String Object to String.
      char buf[lorainput.length()];
      lorainput.toCharArray(buf, sizeof(buf));
      char *p = buf;
      char *str;
      
      while ((str = strtok_r(p, ",", &p)) != NULL)
      { // delimiter is the comma
        String tmp(str);
        Serial.println(int(str[0]));
        Data[index] = tmp;
        index++;
      }

      Serial.println("Done receving data.");
      
      int wb = Data[1][0];
      // Serial.println(lorainput);
      //  Serial.println(lorainput);
      
      if(wb==myid){ 
        packet_cnt = int(lorainput[lorainput.length() - 1]);
        Serial.println(packet_cnt);
        lorapacket(); 
      }
    }
  }      
}



void user_button() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  buttonstate = digitalRead(interruptPin) ;
  if ((interrupt_time - last_interrupt_time > 200) && buttonstate== LOW)
  {
// lorapacket();
 int a = buttonstate ;
 Serial.println(a);
 if (buttonstate=0){
  char bs=0b00000100;
 }
  }
  last_interrupt_time = interrupt_time;
  
  }
