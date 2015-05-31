#include <SPI.h>
#include <Ethernet.h>
#include <VirtualWire.h>
#include <OneWire.h>
OneWire ds(2);
int b = 0;

//20182 Датчик температуры
//20501 Датчик движения
//20516 Датчик света

#define pirPin 5 //Устанавливаем пин к которому подключен датчик движения
int send_data;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //MAC
byte ip[] = { 192, 168, 0, 20 }; //IP
byte server[] = { 192, 168, 0, 3 }; //Server MDM
byte gateway[] = { 192, 168, 0, 1 }; //Gateway
byte subnet[] = { 255, 255, 255, 0 }; //Mask
char buf[80];

const unsigned int calibrationTime = 60000;
const int ledBlinkTime = 500;

EthernetClient rclient;

// Функция отправки HTTP-запроса на сервер
void sendHTTPRequest() {
  Serial.println(buf); 
  if (rclient.connect(server, 80)) { 
   Serial.println("OK"); 
   rclient.println(buf);
   rclient.println("Host: 192.168.0.3");
   rclient.println();   
   delay(100);
   rclient.stop();
   Refresh();
  } else {
   Serial.println("FAILED");     
  }
}

void setup()
{
  Ethernet.begin(mac, ip, gateway, subnet);
  Serial.begin(9600);
  pinMode(pirPin, INPUT); //Устанавливаем режим пина. Вход\Выход
  pinMode(13, OUTPUT);
  
  for (unsigned int i=0; i<calibrationTime; i+=ledBlinkTime*2) {
    digitalWrite(13, HIGH);
    delay(ledBlinkTime);
    digitalWrite(13, LOW);
    delay(ledBlinkTime);
  }
  
  Refresh();
}

void loop() { //Запускаем основной цикл
  b = b + 1;
  Serial.println(b);
  if (b == 60)
  {
    DS18B20();
    MtS();
    FR();
    b = 0;
  }
  else
  {
    MtS();
  }
  delay(1000);
}

//DS18B20 CODE--------------------------------------------------------------------------------
  int DS18B20(){
    byte data[2];
    ds.reset();
    ds.write(0xCC);
    ds.write(0x44);
    delay(750);
    ds.reset();
    ds.write(0xCC);
    ds.write(0xBE);
    data[0] = ds.read();
    data[1] = ds.read();
    int Temp = (data[1]<< 8)+data[0];
    Temp = Temp>>4;
    if (Temp < 60 && Temp > -50) {
      send_data = Temp + 1;
    }
    //sprintf(buf, "GET /objects/?object=TempSensor1&op=set&p=Value&v=%i HTTP/1.0", (int)send_data);
    sprintf(buf, "GET /objects/?script=easyRF&did=20182&dest=0&pid=0&c=0&d=%i HTTP/1.0", (int)send_data);
    sendHTTPRequest();
  }
  //DS18B20 END CODE-----------------------------------------------------------------------------
  
    //Фоторезистор---------------------------------------------------------------------------------
  int FR(){
    send_data = analogRead(A0) - 600; //Присваиваем считаное значение с Аналогового пина(Фоторезистор)
    //sprintf(buf, "GET /objects/?object=LightSensor1&op=set&p=Value&v=%i HTTP/1.0", (int)send_data);
    sprintf(buf, "GET /objects/?script=easyRF&did=20516&dest=0&pid=0&c=0&d=%i HTTP/1.0", (int)send_data);
    sendHTTPRequest();
  }
  //Фоторезистор---------------------------------------------------------------------------------
  
    //Датчик движения------------------------------------------------------------------------------
  int MtS(){
    int pirVal = digitalRead(pirPin); //Присваеваем считаное значение с датчика движения
    if(pirVal == HIGH) //Если обнаружили движение
    {
      send_data = 1;
      //sprintf(buf, "GET /objects/?object=PIRSensor1&op=set&p=Value&v=%i HTTP/1.0", (int)send_data);
      sprintf(buf, "GET /objects/?script=easyRF&did=20501&dest=0&pid=0&c=0&d=%i HTTP/1.0", (int)send_data);
    }
    else //Если же движения нет
    {
      send_data = 0;
      //sprintf(buf, "GET /objects/?object=PIRSensor1&op=set&p=Value&v=%i HTTP/1.0", (int)send_data);
      sprintf(buf, "GET /objects/?script=easyRF&did=20501&dest=0&pid=0&c=0&d=%i HTTP/1.0", (int)send_data);
    }
    sendHTTPRequest();
  }
  //Датчик движения------------------------------------------------------------------------------
  
int Refresh(){
    digitalWrite(13, HIGH);
    send_data=0;
    digitalWrite(13, LOW);
  }
