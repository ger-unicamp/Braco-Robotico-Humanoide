#include <Arduino.h>
#include <GervoMotor.h>
#include <BracoUDP.h>
#include "appconfig.h"


// BOTÕES 
const uint8_t PIN_BUTTON[] = { 27, 26, 14}; // (SELECT, LEFT, RIGHT)
const int numButtons = 3;
bool buttonState[] = { false, false, false };
bool clickedJustNow[] = { false, false, false };
#define BUTTON_SELECT 0
#define BUTTON_LEFT 1
#define BUTTON_RIGHT 2


// LEDS
#define LED_ESP32 2


// MOTORES
// Mapeamento dos pinos dos motores por id (de 0 a 4)
const uint8_t PIN_MOTOR[] = { 15, 4, 5, 19, 22, 23 };   // Terminais 1, 2, 3, 4, 5 e 6 do circuito
// O circuito tem 6 terminais mas só estamos usando 5 motores
// Definições para os servos (MUDAR ESSES NOMES DEPOIS, ESTÁ DA ESQUERDA PARA A DIREITA)
GervoMotor servo1;
GervoMotor servo2;
GervoMotor servo3;
GervoMotor servo4;
GervoMotor servo5;
// Mapeamento dos objetos servos por ids correspondentes aos pinos (de 0 a 4)
GervoMotor *servos[] = {&servo1, &servo2, &servo3, &servo4, &servo5};
GervoMotor *selectedServo = servos[0];
const int numServos = 5;


// DECLARAÇÕES DE FUNÇÕES
void blink(uint8_t pin = LED_ESP32);
void spinButtons();
void spinUDPControl();
void spinOverrideSystem();
void checkOverrideSystemChange();

BracoPacket packetIn;

bool overrideSystemIsActive = false;

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
    // Lógica que identifica quais botões etão/foram clicados neste "frame"
    spinButtons();
    
    // Normalmente queremos receber comandos do PC (UDPControl)
    if (overrideSystemIsActive) spinOverrideSystem();
    else spinUDPControl();

    // Checa a combinação de botões para ativar o modo override
    checkOverrideSystemChange();

    delay(50);
}

void blink(uint8_t pin)
{
    digitalWrite(pin, HIGH);
    delay(10);
    digitalWrite(pin, LOW);
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
            blink();
            return true;
        }
    }

    if (digitalRead(pin) == false)
        buttonState[button_id] = false;

    return false;
}

void spinButtons()
{
    for (int i = 0; i < numButtons; i++)
    {
        clickedJustNow[i] = pulseButton(i);
    }
}

void printPacketIn()
{
    Serial.println("Pacote recebido!");
    Serial.printf("Comando: %d | Ângulos: %.1f, %.1f, %.1f, %.1f, %.1f | Extras: %d, %d\n",
        packetIn.command,
        packetIn.angles[0], packetIn.angles[1], packetIn.angles[2], 
        packetIn.angles[3], packetIn.angles[4], 
        packetIn.extra1, packetIn.extra2);
}

void spinUDPControl()
{
    // Checa se há pacote chegando, se não houver, não tem nada pra fazer
    if (!isTherePacket(&packetIn, AUTH_PACKET))
    {
        return;
    }

    // Chegou pacote! Está armazenado no objeto 'packetIn'

    // Print do pacote para debug
    printPacketIn();

    // Manda os valores dos ângulos recebidos para os motores:
    for (int i = 0; i < numServos; i++)
    {
        // O valor vem em radianos e o servo pede graus
        float rads = packetIn.angles[i];
        int degrees = (int)round(rads * 180.0f / PI);
        servos[i]->writeAngle(degrees);
    }

    blink();
}

unsigned long _overrideCombinationStart;
bool _wasOverrideCombinationOnLastFrame;

void checkOverrideSystemChange()
{
    if ( ! (buttonState[BUTTON_SELECT] && buttonState[BUTTON_RIGHT] && buttonState[BUTTON_LEFT]) )
    {
        // Combinação de override não está feita
        _wasOverrideCombinationOnLastFrame = false;
        return;
    }

    if (_wasOverrideCombinationOnLastFrame == false)
    {
        _wasOverrideCombinationOnLastFrame = true;
        _overrideCombinationStart = millis();
        return;
    }
    
    if (millis() - _overrideCombinationStart > 5)
    {
        // Hora de alternar o modo
        overrideSystemIsActive = !overrideSystemIsActive;
        _wasOverrideCombinationOnLastFrame = false; // segurança para não ficar alternando loucamente após os 5 segundos

        // Aviso de troca do sistema
        Serial.print("Override System atualizado para: ");
        Serial.println(overrideSystemIsActive);
        for (int i = 0; i < 10; i++)
        {
            digitalWrite(LED_ESP32, HIGH);
            delay(100);
            digitalWrite(LED_ESP32, LOW);
            delay(100);
        }
    }
}

void spinOverrideSystem()
{
    // TODO
    // TODO
    // TODO
}