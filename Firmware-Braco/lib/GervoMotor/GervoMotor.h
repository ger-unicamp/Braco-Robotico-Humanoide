#ifndef GERVOMOTOR_H
#define GERVOMOTOR_H

#include <Arduino.h> // Essencial para incluir tipos e funções do framework

/**
 * @class GervoMotor
 * @brief Classe para controlar um servo motor usando PWM em um ESP32.
 *
 * Esta classe permite anexar um servo motor a um pino específico e canal PWM,
 * além de controlar o ângulo do servo por meio de pulsos PWM.
 *
 * Métodos principais:
 * - attach: Configura o servo em um pino e canal PWM, com pulsos mínimos e máximos (paralelo 0-180 graus).
 * - writeAngle: Move o servo para um ângulo desejado.
 * 
 */
class GervoMotor {
public:
    
    /// @brief Construtor padrão
    GervoMotor();

    /// @brief Anexa o servo a um pino e a um canal PWM
    /// @param pin pino do motor
    /// @param channel canal pwm para usar
    /// @param minPulse (opcional) pulso mínimo em microssegundos equivalente a 0 graus (padrão 500us)
    /// @param maxPulse (opcional) pulso máximo em microssegundos equivalente a 180 graus (padrão 2500us)
    void attach(int pin, int channel, int minPulse = 500, int maxPulse = 2500);

    /// @brief Move o servo para um ângulo específico
    /// @param angle ângulo desejado (entre 0 e 180 graus)
    void writeAngle(int angle);

    /// @brief Retorna o ângulo atual do servo
    /// @return ângulo atual em graus, ou -1 se for desconhecido
    int getCurrentAngle();

private:
    int _channel;
    int _minPulse;
    int _maxPulse;
    bool _isAttached = false; // Flag para saber se o servo foi configurado
    int _currentAngle = -1; // Armazena o ângulo atual do servo
};

#endif