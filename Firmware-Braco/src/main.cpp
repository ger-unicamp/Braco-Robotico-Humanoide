#include <Arduino.h>
#include <GervoMotor.h>
#include <BracoUDP.h>
#include "appconfig.h"


// Terminais 1, 2, 3, 4, 5 e 6 do circuito
const uint8_t PINOS_MOTORES[] = { 15, 4, 5, 19, 22, 23 };

#define BUTTON_SELECT 27
#define BUTTON_LEFT 26
#define BUTTON_RIGHT 14

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

bool buttonSelectState = false;
bool pulseButtonSelect();

BracoPacket packetIn;

void setup()
{
    Serial.begin(115200);

    pinMode(BUTTON_SELECT, INPUT);
    pinMode(BUTTON_LEFT, INPUT);
    pinMode(BUTTON_RIGHT, INPUT);

    pinMode(LED_ESP32, OUTPUT);
    //pinMode(LED_PULSO, OUTPUT); no momento é o mesmo led do esp32

    for (int i = 0; i < numServos; i++)
    {
        servos[i]->attach(PINOS_MOTORES[i], i+1);
    }

    beginWifiUDP(WIFI_SSID, WIFI_PASSWORD, UDP_PORT, LED_ESP32);
}

void loop()
{
    if (isTherePacket(&packetIn, AUTH_PACKET))
    {
        Serial.println("Pacote recebido!");
        Serial.printf("Ângulos: %.1f, %.1f, %.1f, %.1f, %.1f | Extras: %d, %d\n",
            packetIn.angles[0], packetIn.angles[1], packetIn.angles[2], 
            packetIn.angles[3], packetIn.angles[4], 
            packetIn.extra1, packetIn.extra2);
    }
}

bool pulseButtonSelect()
{
    if (buttonSelectState == false && digitalRead(BUTTON_SELECT))
    {
        delay(10); // Debounce
        if (digitalRead(BUTTON_SELECT))
        {
            buttonSelectState = true;
            return true;
        }
    }

    if (digitalRead(BUTTON_SELECT) == false)
        buttonSelectState = false;

    return false;
}


/*
int positions[] = {0, 90, 180, 90};
int posIndex = 0;
int numPositions = sizeof(positions) / sizeof(positions[0]);

int selectedIndex = 0; // Servos

void loopAntigo()
{

    // Passos grandes pra um motor
    /*if (pulseButtonSelect())
    {
      Serial.println("SELECT!");

      digitalWrite(LED_PULSO, true);
      delay(100);
      digitalWrite(LED_PULSO, false);

      servoPulso.writeAngle(positions[posIndex]);
      posIndex = (posIndex + 1) % numPositions;
    }/

    // Alterna motores, controle "fluído"
    /*if (pulseButtonSelect())  // alterna motor
    {
      Serial.println("SELECT!");

      digitalWrite(LED_PULSO, true);
      delay(100);
      digitalWrite(LED_PULSO, false);

      selectedIndex = (selectedIndex + 1) % numServos;
      selectedServo = servos[selectedIndex];
    }

    // controle "fluído" do motor selecionado
    if (digitalRead(BUTTON_LEFT))
    {
      selectedServo->writeAngle(selectedServo->getCurrentAngle() + 5);
      delay(25);
    }
    else if (digitalRead(BUTTON_RIGHT))
    {
      selectedServo->writeAngle(selectedServo->getCurrentAngle() - 5);
      delay(25);
    }/
}
*/
