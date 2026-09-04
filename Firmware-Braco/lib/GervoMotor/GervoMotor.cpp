#include "GervoMotor.h"

// Parâmetros fixos do PWM
#define PWM_FREQ 50
#define PWM_RESOLUTION 16

GervoMotor::GervoMotor() {
    // O construtor está vazio por enquanto, a lógica fica no attach()
}

// Anexa o servo a um pino e a um canal PWM
void GervoMotor::attach(int pin, int pwmChannel, int minPulse, int maxPulse) {
    _channel = pwmChannel;
    _minPulse = minPulse;
    _maxPulse = maxPulse;

    ledcSetup(_channel, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(pin, _channel);
    _isAttached = true;
}

// Move o servo para um ângulo específico entre 0 e 180 graus
void GervoMotor::writeAngle(int angle) {
    if (!_isAttached) {
        return; // Não faz nada se o attach() não foi chamado
    }
    
    // Garante que o ângulo esteja nos limites 0-180
    angle = constrain(angle, 0, 180);

    long pulseUs = map(angle, 0, 180, _minPulse, _maxPulse);
    uint32_t dutyCycle = (uint32_t)((65536 * pulseUs) / 20000);
    ledcWrite(_channel, dutyCycle);

    _currentAngle = angle;
}

int GervoMotor::getCurrentAngle() {
    return _currentAngle;
}