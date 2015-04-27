/*
SimpleSend
This sketch transmits a short text message using the VirtualWire library
connect the Transmitter data pin to Arduino pin 12
*/

#include virtualwire.h //название библиотеки должно быть заключено в угловые скобки <>

int switchPin = 8;
boolean lastButton = LOW;
boolean currentButton = LOW;
int check = 0;

void setup()
{
    pinMode(switchPin, INPUT);

    // Initialize the IO and ISR
    vw_setup(2000); // Bits per sec
}

boolean debounce(boolean last)
{
    boolean current = digitalRead(switchPin);
    if (last != current)
    {
        delay(5);
        current = digitalRead(switchPin);
    }
    return current;
}

void loop()
{
    currentButton = debounce(lastButton);
    if (lastButton == LOW && currentButton == HIGH && check == 0)
    {
        send("on");
        delay(500);
        check = 1;
    }
    else

    if (lastButton == LOW && currentButton == HIGH && check == 1)
    {
        send("off");
        delay(500);
        check = 0;
    }
}

void send (char *message)
{
    vw_send((uint8_t *)message, strlen(message));
    vw_wait_tx(); // Wait until the whole message is gone
}