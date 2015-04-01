#include <VirtualWire.h>
#include <OneWire.h>
OneWire ds(9);

#define pirPin 4 //Устанавливаем пин к которому подключен датчик движения


struct SEND_DATA_STRUCTURE{
  unsigned int device_id;
  unsigned int destination_id;  
  unsigned int packet_id;
  byte command;
  int data;
};

SEND_DATA_STRUCTURE mydata;

void setup() {
  Serial.begin(9600); //Устанавливаем скорость порта
  pinMode(pirPin, INPUT); //Устанавливаем режим пина. Вход\Выход
}

void loop() { //Запускаем основной цикл
  delay(2000);
  mydata.packet_id = analogRead(A0); //Присваиваем считаное значение с Аналогового пина
  int pirVal = digitalRead(pirPin); //Присваеваем считаное значение с датчика движения
  
  if(pirVal == HIGH) //Если обнаружили движение
  {
    mydata.command = 1;
  }
  else //Если же движения нет
  {
    mydata.command = 0;
  }
  
  //DS18B20 CODE-------------
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
  mydata.destination_id = Temp;


  //DS18B20 END CODE-------------
  Serial.print("P:");
  Serial.print(mydata.packet_id);
  Serial.print(";F:");        
  Serial.print(mydata.device_id);
  Serial.print(";T:"); //Выводим...
  Serial.print(mydata.destination_id); //считаную информацию
  Serial.print(";C:");
  Serial.print(mydata.command);
  Serial.print(";D:");
  Serial.print(mydata.data);
  Serial.println(";");
}
