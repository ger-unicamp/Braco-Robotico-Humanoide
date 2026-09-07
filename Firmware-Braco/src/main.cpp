#include <Arduino.h>
#include <GervoMotor.h>
#include <BracoUDP.h>
#include "appconfig.h"


// Terminais 1, 2, 3, 4, 5 e 6 do circuito
const uint8_t PIN_MOTOR[] = { 15, 4, 5, 19, 22, 23 };

// Botões SELECT, LEFT, RIGHT
const uint8_t PIN_BUTTON[] = { 27, 26, 14};
const int numButtons = 3;
bool buttonState[] = { false, false, false };

#define BUTTON_SELECT 0
#define BUTTON_LEFT 1
#define BUTTON_RIGHT 2

#define LED_ESP32 2
#define LED_PULSO 2

// Definições para os servos (MUDAR ESSES NOMES DEPOIS, ESTÁ DA ESQUERDA PARA A DIREITA)
GervoMotor servo1;
GervoMotor servo2;
GervoMotor servo3;
GervoMotor servo4;
GervoMotor servo5;

GervoMotor *servos[] = {&servo1, &servo2, &servo3, &servo4, &servo5};
GervoMotor *selectedServo = servos[0];
const int numServos = 5; // Mudar para 6 se for usar o último

bool pulseButton(uint8_t button_id);

void blink(uint8_t pin);

BracoPacket packetIn;

bool overwriteControls = false;

void setup()
{
    Serial.begin(115200);
    delay(500);
    Serial.println("Iniciando Setup!");
    Serial.flush();

    for (int i = 0; i < numButtons; i++)
    {
        pinMode(PIN_BUTTON[i], INPUT);
    }
    
    pinMode(LED_ESP32, OUTPUT);
    //pinMode(LED_PULSO, OUTPUT); no momento é o mesmo led do esp32
    
    for (int i = 0; i < numServos; i++)
    {
        servos[i]->attach(PIN_MOTOR[i], i+1);
    }

    digitalWrite(LED_ESP32, HIGH);
    delay(5000);
    

    beginWifiUDP(WIFI_SSID, WIFI_PASSWORD, UDP_PORT, LED_ESP32);

    Serial.flush();
}

void loop()
{
    if (isTherePacket(&packetIn, AUTH_PACKET))
    {
        Serial.println("Pacote recebido!");
        Serial.printf("Comando: %d | Ângulos: %.1f, %.1f, %.1f, %.1f, %.1f | Extras: %d, %d\n",
            packetIn.command,
            packetIn.angles[0], packetIn.angles[1], packetIn.angles[2], 
            packetIn.angles[3], packetIn.angles[4], 
            packetIn.extra1, packetIn.extra2);

        blink(LED_ESP32);
    }

    if (pulseButton(BUTTON_SELECT))
    {
        Serial.println("SELECT pressionado!");
        blink(LED_ESP32);
    }

    if (pulseButton(BUTTON_LEFT))
    {
        Serial.println("LEFT pressionado!");
        blink(LED_ESP32);
    }

    if (pulseButton(BUTTON_RIGHT))
    {
        Serial.println("RIGHT pressionado!");
        blink(LED_ESP32);
    }

    delay(50);
}

bool pulseButton(uint8_t button_id)
{
    int pin = PIN_BUTTON[button_id];

    if (buttonState[button_id] == false && digitalRead(pin))
    {
        delay(10); // Debounce
        if (digitalRead(pin))
        {
            buttonState[button_id] = true;
            return true;
        }
    }

    if (digitalRead(pin) == false)
        buttonState[button_id] = false;

    return false;
}

void blink(uint8_t pin)
{
    digitalWrite(pin, HIGH);
    delay(10);
    digitalWrite(pin, LOW);
}