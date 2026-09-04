#include <Arduino.h>
#include <GervoMotor.h>

#define PORTA_1 15
#define PORTA_2 4
#define PORTA_3 5
#define PORTA_4 19
#define PORTA_5 22
#define PORTA_6 23

#define BUTTON_SELECT 27
#define BUTTON_LEFT 26
#define BUTTON_RIGHT 14

#define LED_PULSO 2

// Definições para os servos (MUDAR ESSES NOMES DEPOIS, EST[Á DA ESQUERDA PARA A DIREITA)
GervoMotor servo1;
GervoMotor servo2;
GervoMotor servo3;
GervoMotor servo4;
GervoMotor servo5;

GervoMotor* servos[] = {&servo1, &servo2, &servo3, &servo4, &servo5};
GervoMotor * selectedServo = servos[0];
int selectedIndex = 0;
int numServos = 5;  // Mudar para 6 se for usar o útimo

bool pulseButtonSelect();

bool buttonSelectState = false;

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_SELECT, INPUT);
  pinMode(BUTTON_LEFT, INPUT);
  pinMode(BUTTON_RIGHT, INPUT);

  pinMode(LED_PULSO, OUTPUT);

  servo1.attach(PORTA_1, 1);
  servo2.attach(PORTA_2, 2);
  servo3.attach(PORTA_3, 3);
  servo4.attach(PORTA_4, 4);
  servo5.attach(PORTA_5, 5);

  Serial.println("TESTE SERVOS COM BOTOES");
}

int positions[] = {0, 90, 180, 90};
int posIndex = 0;
int numPositions = sizeof(positions) / sizeof(positions[0]);

void loop() {
  /*if (pulseButtonSelect())
  {
    Serial.println("SELECT!");

    digitalWrite(LED_PULSO, true);
    delay(100);
    digitalWrite(LED_PULSO, false);
    
    servoPulso.writeAngle(positions[posIndex]);
    posIndex = (posIndex + 1) % numPositions;
  }*/

  if (pulseButtonSelect())
  {
    Serial.println("SELECT!");

    digitalWrite(LED_PULSO, true);
    delay(100);
    digitalWrite(LED_PULSO, false);
    
    selectedIndex = (selectedIndex + 1) % numServos;
    selectedServo = servos[selectedIndex];
  }

  if (digitalRead(BUTTON_LEFT))
  {
    selectedServo->writeAngle(selectedServo->getCurrentAngle() + 5);
    delay(25);
  }
  else if (digitalRead(BUTTON_RIGHT))
  {
    selectedServo->writeAngle(selectedServo->getCurrentAngle() - 5);
    delay(25);
  }
}

bool pulseButtonSelect() {
  if (buttonSelectState == false && digitalRead(BUTTON_SELECT)) {
    delay(10); // Debounce
    if (digitalRead(BUTTON_SELECT)) {
      buttonSelectState = true;
      return true;
    }
  }

  if (digitalRead(BUTTON_SELECT) == false)
    buttonSelectState = false;

  return false;
}