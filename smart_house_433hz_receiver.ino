/*
SimpleReceive
This sketch displays text strings received using VirtualWire
Connect the Receiver data pin to Arduino pin 11
*/
#include virtualwire .h//название библиотеки должно быть заключено в угловые скобки <>
byte message[VW_MAX_MESSAGE_LEN]; // a buffer to store the incoming messages
byte messageLength = VW_MAX_MESSAGE_LEN; // the size of the message
int relayPin = 7;

void setup()
{
pinMode(relayPin, OUTPUT);
digitalWrite(relayPin, HIGH);

Serial.begin(9600);
Serial.println("Device is ready");
// Initialize the IO and ISR
vw_setup(2000); // Bits per sec
vw_rx_start(); // Start the receiver
}
void loop()
{
if (vw_get_message(message, &messageLength)) // Non-blocking

{
Serial.print("Received: ");
for (int i = 0; i < messageLength; i++)
{
char c = message[i];
Serial.print(c);

char on = 'on';
if(c == on)
{
digitalWrite(relayPin, LOW);
}
char off = 'of';

if(c == off)
{
digitalWrite(relayPin, HIGH);
}

}
Serial.println();
}
}