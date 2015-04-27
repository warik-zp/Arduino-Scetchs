#include <VirtualWire.h>
#include <OneWire.h>
OneWire ds(2);

#define pirPin 5 //Устанавливаем пин к которому подключен датчик движения


struct SEND_DATA_STRUCTURE{
  unsigned int device_id;
  unsigned int destination_id;  
  long packet_id;
  byte command;
  int data;
};

SEND_DATA_STRUCTURE mydata;

void setup() {
  Serial.begin(9600); //Устанавливаем скорость порта
  pinMode(pirPin, INPUT); //Устанавливаем режим пина. Вход\Выход
  pinMode(13, OUTPUT);
  Refresh();
}

void loop() { //Запускаем основной цикл
DS18B20();
MtS();
FR();
delay(5000);
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
      mydata.data = Temp + 1;
      mydata.device_id=20182;
    }
    SendData();
  }
  //DS18B20 END CODE-----------------------------------------------------------------------------
  
  //Датчик движения------------------------------------------------------------------------------
  int MtS(){
    int pirVal = digitalRead(pirPin); //Присваеваем считаное значение с датчика движения
    if(pirVal == HIGH) //Если обнаружили движение
    {
      mydata.data = 1;
      mydata.device_id=20501;
    }
    else //Если же движения нет
    {
      mydata.data = 0;
      mydata.device_id=20501;
    }
    SendData();
  }
  //Датчик движения------------------------------------------------------------------------------
  
  
  //Фоторезистор---------------------------------------------------------------------------------
  int FR(){
    mydata.data = analogRead(A0); //Присваиваем считаное значение с Аналогового пина(Фоторезистор)
    mydata.device_id=20516;
    SendData();
  }
  //Фоторезистор---------------------------------------------------------------------------------

  //Функция отправки данных----------------------------------------------------------------------
    int SendData(){
    Serial.print("P:");
    Serial.print(mydata.packet_id);
    Serial.print(";F:");        
    Serial.print(mydata.device_id);
    Serial.print(";T:");
    Serial.print(mydata.destination_id);
    Serial.print(";C:");
    Serial.print(mydata.command);
    Serial.print(";D:");
    Serial.print(mydata.data);
    Serial.println(";");
    Refresh();
  }
  
  int Refresh(){
    digitalWrite(13, HIGH);
    mydata.packet_id=random(40000, 50000);
    mydata.device_id=20000;
    mydata.destination_id=40003;
    mydata.command=0;
    mydata.data=0;
    digitalWrite(13, LOW);
  }
  //Функция отправки данных----------------------------------------------------------------------
