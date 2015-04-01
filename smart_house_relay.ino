#include <Ethernet.h>
#include <SPI.h>
#include <Arduino.h>
#include "WebServer.h" // Webduino (https://github.com/sirleech/Webduino)
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xE4, 0xDE, 0x35 }; // MAC-адрес нашего устройства
byte ip[] = { 192, 168, 0, 21 };
byte subnet[] = { 255, 255, 255, 0 };
byte gateway[] = { 192, 168, 0, 1 };
byte dns_server[] = { 192, 168, 0, 1 };
// ip-адрес удалённого сервера
byte rserver[] = { 192, 168, 0, 3 };
// Настройки выходов
int startPin=3;
int endPin=6;
// Pin controller for connection data pin DS18S20
#define VERSION_STRING "0.1"
#define COMPILE_DATE_STRING "2015-02-02"
P(Page_info) = "<html><head><title>Relay controller</title></head><body>\n";
P(location_info) = "underground server room";
P(pin_info) = "D2 - 1-wire (many DS18S20)<br>D3-light undergroung";
P(version_info) = VERSION_STRING ". Compile date: " COMPILE_DATE_STRING;
String url = String(25);
int maxLength=25; // Максимальная длинна строки запроса
#define MAX_COMMAND_LEN (10)
#define MAX_PARAMETER_LEN (10)
#define COMMAND_TABLE_SIZE (8)
#define PREFIX ""
WebServer webserver(PREFIX, 80);
#define NAMELEN 32
#define VALUELEN 32
char gCommandBuffer[MAX_COMMAND_LEN + 1];
char gParamBuffer[MAX_PARAMETER_LEN + 1];
long gParamValue;
typedef struct {
char const *name;
void (*function)(WebServer &server);
} command_t;
command_t const gCommandTable[COMMAND_TABLE_SIZE] = {
// {"LED", commandsLed, },
{"ON", commandsOn, }, // Устанавливает "1" на заданном цифровом порту (вызов http://xx.xx.xx.xx/command?8=ON )
{"OFF", commandsOff, }, // Устанавливает "0" на заданном цифровом порту (вызов http://xx.xx.xx.xx/command?8=OFF )
{"STATUS", commandsStatus, }, // Получить состояние цифрового порта (1 или 0) (вызов http://xx.xx.xx.xx/command?8=STATUS ),
// если вместо номера порта передать ALL (вызов http://xx.xx.xx.xx/command?ALL=STATUS ), то получим состояние всех портов (Пример вывода P3=0;P4=0;P5=0;P6=0;P7=0;P8=1;P9=1;)
{"HELP", commandsHelp, }, // Выводит список комманд (вызов http://xx.xx.xx.xx/command?8=HELP )
{NULL, NULL }
};
/**********************************************************************************************************************
*
* Function: cliProcessCommand
*
* Description: Look up the command in the command table. If the
* command is found, call the command's function. If the
* command is not found, output an error message.
*
* Notes:
*
* Returns: None.
*
**********************************************************************/
void cliProcessCommand(WebServer &server)
{
int bCommandFound = false;
int idx;
gParamValue = strtol(gParamBuffer, NULL, 0); // Convert the parameter to an integer value. If the parameter is empty, gParamValue becomes 0.
for (idx = 0; gCommandTable[idx].name != NULL; idx++) { // Search for the command in the command table until it is found or the end of the table is reached. If the command is found, break out of the loop.
if (strcmp(gCommandTable[idx].name, gCommandBuffer) == 0) {
bCommandFound = true;
break;
}
}
if (bCommandFound == true) { // Если команда найдена (в массиве команд), то выполняем ее. Если нет - игнорируем
(*gCommandTable[idx].function)(server);
}
else { // Command not found
server.print("ERROR: Command not found");
}
}
/**********************************************************************************************************************/
/* Обработчики команд */
void commandsOn(WebServer &server) {
if (gParamValue>=startPin && gParamValue<=endPin) {
digitalWrite(gParamValue,LOW);
} else ErrorMessage(server);
}
void commandsOff(WebServer &server) {
if (gParamValue>=startPin && gParamValue<=endPin) {
digitalWrite(gParamValue,HIGH);
} else ErrorMessage(server);
}
void commandsStatus(WebServer &server) {
if (strcmp(gParamBuffer, "ALL") == 0) { // выдать состояние всех пинов
for(int i=startPin;i<=endPin;i++) {
int st=digitalRead(i);
char my_st[5];
itoa(st,my_st,10);
server.print("Pin");
server.print(i);
server.print("=");
server.print(my_st);
server.print("&");
}
} else { // выдать состояние только 1 пина
if (gParamValue>=startPin && gParamValue<=endPin) {
server.print("Pin");
server.print(gParamValue);
server.print("=");
server.print(digitalRead(gParamValue));
} else ErrorMessage(server);
}
}
void commandsHelp(WebServer &server) {
int idx;
for (idx = 0; gCommandTable[idx].name != NULL; idx++) {
server.print(gCommandTable[idx].name);
server.print("<br>");
}
}
/**********************************************************************************************************************/
void ErrorMessage(WebServer &server) {
server.print("ERROR: This Pin is not configured for I/O");
}
/**********************************************************************************************************************
* Разбор запроса
**/
void parsedRequest(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
URLPARAM_RESULT rc;
char name[NAMELEN];
char value[VALUELEN];
server.httpSuccess(); // this line sends the standard "we're all OK" headers back to the browser
/* if we're handling a GET or POST, we can output our data here.
For a HEAD request, we just stop after outputting headers. */
if (type == WebServer::HEAD)
return;
if (strlen(url_tail))
{
while (strlen(url_tail)) // Разбор URI на составные части (выборка параметров)
{
rc = server.nextURLparam(&url_tail, name, NAMELEN, value, VALUELEN);
if (rc == URLPARAM_EOS) {
// server.printP(Params_end);
}
else // Получили параметр (name) и его значение (value)
{
// Выполняем команды
strcpy (gCommandBuffer, value); // параметры (значение)
strcpy (gParamBuffer, name); // команда
cliProcessCommand(server);
}
}
}
}
void stateRequest(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
strcpy (gParamBuffer, "ALL");
commandsStatus(server);
}
/**********************************************************************************************************************
* Генерация и вывод информации об устройстве
**/
void infoRequest(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
server.printP(Page_info);
server.print("IP:");
server.print(Ethernet.localIP());
server.print("<br>Location:");
server.printP(location_info);
server.print("<hr>Pin info:<br>");
server.printP(pin_info);
server.print("<hr>Pin current state: <br>");
strcpy (gParamBuffer, "ALL");
commandsStatus(server);
server.print("<hr>Commands:<br>");
commandsHelp(server);
server.print("<hr><br>Version info: ");
server.printP(version_info);
}
/**********************************************************************************************************************/
void setup() {
// Для дебага будем выводить отладочные сообщения в консоль
//TODO Убрать вывод в консоль "за дабаг" (т.е. вывод только если скимпилированно с поддержкой дебага)
Serial.begin(9600);
Serial.println("Start");
Ethernet.begin(mac, ip, dns_server, gateway, subnet); // Инициализируем Ethernet Shield
webserver.setDefaultCommand(&infoRequest); // дефолтная страница вывода (информация о контроллере)
webserver.addCommand("command", &parsedRequest); // команды
webserver.addCommand("state", &stateRequest); // выдать состояния всех устройств
webserver.begin();
Serial.print("server is at ");
Serial.println(Ethernet.localIP());
// Настройка портов на вывод
for (int thisPin = startPin; thisPin <=endPin; thisPin++) {
pinMode(thisPin, OUTPUT);
}
digitalWrite(3,HIGH);
digitalWrite(4,HIGH);
digitalWrite(5,HIGH);
digitalWrite(6,HIGH);
}
/**********************************************************************************************************************/
void loop() {
char buff[64];
int len = 64;
webserver.processConnection(buff, &len); // process incoming connections one at a time forever
}
