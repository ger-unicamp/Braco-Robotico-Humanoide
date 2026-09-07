# Guia de Comunicação WiFi Direta (Python ➔ ESP32) para o Braço GER V2

Desistir do micro-ROS para focar em uma solução customizada de sockets é uma decisão excelente e muito comum em equipes de robótica de bancada. O micro-ROS traz uma sobrecarga de compilação significativa, enquanto sockets brutos dão total controle sobre os pacotes e a latência de transmissão.

Para o Braço GER V2 (5 DoF), estabelecemos abaixo as melhores práticas de arquitetura de rede e de dados para garantir controle suave, seguro e livre de travamentos.

---

## 1. Melhores Práticas de Comunicação WiFi em Robótica

### A. Escolha do Protocolo: UDP vs. TCP
*   **UDP (Recomendado para Trajetórias):** O fluxo de trajetórias do MoveIt 2 envia novos ângulos de junta de forma contínua em frequências constantes (ex: 20Hz a 50Hz). No UDP, os dados são enviados sem confirmação de recebimento. Se um pacote com os ângulos do frame 10 for perdido na rede WiFi local, o robô simplesmente receberá o frame 11 logo em seguida. 
*   **O Risco do TCP:** O TCP garante a entrega ordenando e retransmitindo pacotes perdidos. Se a rede WiFi flutuar por 100ms, o TCP travará o recebimento e depois entregará todos os pacotes acumulados de uma vez (*Head-of-Line Blocking*). Isso faz com que o braço dê trancos mecânicos e engasgue violentamente em bancada.
*   **Decisão:** Use **UDP** para o envio contínuo de ângulos das juntas e **TCP** apenas se precisar de envio de comandos críticos isolados (ex: salvar um ponto, calibrar sensores).

### B. Estrutura dos Dados: Serialização Binária (`struct`)
*   **Por que NÃO usar JSON/CSV:** Enviar strings como `"{base: 1.2, ombro: 0.5...}"` exige que o processador do ESP32 realize conversões pesadas de texto para número (`strtof`, `strtok` ou alocação dinâmica com ArduinoJson). Isso gera sobrecarga de CPU e pode causar fragmentação da memória RAM (Heap).
*   **A Solução (Gold Standard):** Enviar os dados em **formato binário puro**. Em Python, empacotamos os 5 floats (4 bytes cada) usando a biblioteca `struct`. No ESP32 (C++), lemos esses bytes e os mapeamos diretamente na memória do chip para uma estrutura idêntica de dados.
*   **Eficiência:** O pacote final possui **exatamente 20 bytes** (5 floats × 4 bytes). A decodificação consome **zero tempo de processamento** no ESP32, sendo resolvida por um simples mapeamento físico (`memcpy`).

### C. Watchdog de Segurança (Crucial!)
Como redes WiFi locais podem sofrer instabilidades, o ESP32 deve implementar um mecanismo de segurança ativa (*Watchdog*). Se o microcontrolador não receber nenhum comando de juntas válido por mais de **500ms**, ele deve desarmar os canais PWM ou parar de enviar novos ângulos para evitar que o braço continue se movendo ou trave em uma posição perigosa se o computador perder o sinal.

---

## 2. Implementação no ESP32 (C++ / PlatformIO)

Este código configura um servidor UDP na porta `8888` que escuta pacotes binários de exatamente 20 bytes (5 floats em padrão de rede/Big-Endian).

```cpp
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <GervoMotor.h> // Biblioteca nativa do GER

// Configurações de Rede (Substitua pelas credenciais da bancada do GER)
const char* ssid = "NOME_DA_REDE_WIFI";
const char* password = "SENHA_DA_REDE";
const unsigned int localPort = 8888; // Porta de escuta UDP

WiFiUDP udp;

// Estrutura de dados empacotada correspondente ao struct.pack('!5f') do Python
// ! = Network Byte Order (Big-Endian)
// 5f = 5 floats de 32 bits (4 bytes cada) = 20 bytes totais
struct __attribute__((packed)) JointPacket {
  float base;
  float ombro;
  float cotovelo;
  float pulso;
  float garra;
};

// Pinos físicos do GER V2
#define PIN_BASE     15
#define PIN_OMBRO    4
#define PIN_COTOVELO 5
#define PIN_PULSO    19
#define PIN_GARRA    22

GervoMotor servo_base;
GervoMotor servo_ombro;
GervoMotor servo_cotovelo;
GervoMotor servo_pulso;
GervoMotor servo_garra;

// Controle do Watchdog de Segurança
unsigned long lastPacketTime = 0;
const unsigned long watchdogTimeout = 500; // ms

// Helper para converter radianos (ROS 2) para graus (Servo)
int radToDeg(float rad) {
  return (int)(rad * 180.0 / PI);
}

// Inverte a ordem dos bytes de um float de Big-Endian (Rede) para Little-Endian (ESP32)
float ntohf(float net_float) {
  union {
    float f;
    uint32_t i;
  } u;
  u.f = net_float;
  u.i = __builtin_bswap32(u.i); // Swap de bytes por instrução de hardware ultra-rápida
  return u.f;
}

void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT); // LED interno indicador

  // Inicializa servos da biblioteca GervoMotor
  servo_base.attach(PIN_BASE, 1);
  servo_ombro.attach(PIN_OMBRO, 2);
  servo_cotovelo.attach(PIN_COTOVELO, 3);
  servo_pulso.attach(PIN_PULSO, 4);
  servo_garra.attach(PIN_GARRA, 5);

  // Conexão Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(2, !digitalRead(2)); // Pisca o LED durante a conexão
  }
  
  digitalWrite(2, HIGH); // LED aceso indica conexão WiFi estabelecida
  Serial.println("\nWiFi Conectado!");
  Serial.print("IP do ESP32: ");
  Serial.println(WiFi.localIP());

  // Inicializa a escuta UDP
  udp.begin(localPort);
  lastPacketTime = millis();
}

void loop() {
  int packetSize = udp.parsePacket();
  
  if (packetSize == sizeof(JointPacket)) {
    JointPacket raw_packet;
    
    // Lê os bytes brutos diretamente para a memória do struct
    udp.read((char*)&raw_packet, sizeof(JointPacket));
    lastPacketTime = millis(); // Reseta watchdog

    // Converte floats recebidos em Big-Endian (Rede) para Little-Endian (ESP32)
    float pos_base     = ntohf(raw_packet.base);
    float pos_ombro    = ntohf(raw_packet.ombro);
    float pos_cotovelo = ntohf(raw_packet.cotovelo);
    float pos_pulso    = ntohf(raw_packet.pulso);
    float pos_garra    = ntohf(raw_packet.garra);

    // Converte radianos para graus e escreve instantaneamente nos servos
    servo_base.writeAngle(radToDeg(pos_base));
    servo_ombro.writeAngle(radToDeg(pos_ombro));
    servo_cotovelo.writeAngle(radToDeg(pos_cotovelo));
    servo_pulso.writeAngle(radToDeg(pos_pulso));
    servo_garra.writeAngle(radToDeg(pos_garra));

    // Pisca rapidamente o LED indicando pacote recebido com sucesso
    digitalWrite(2, !digitalRead(2));
  }

  // Watchdog de segurança
  if (millis() - lastPacketTime > watchdogTimeout) {
    // Caso perca conexão, pode desativar comandos ou sinalizar erro no LED
    digitalWrite(2, LOW); // Apaga LED indicando perda de sinal de trajetória
  }
}
```

---

## 3. Implementação do Emissor em Python (Nó do Computador Host)

Este script Python lê comandos e envia o buffer binário compacto de 20 bytes via UDP. Ele serve de molde para o nó adaptador que vocês integrarão ao ROS 2.

```python
import socket
import struct
import math
import time

# Configurações de Conexão (Substitua pelo IP real impresso na serial do ESP32)
ESP32_IP = "192.168.1.50" 
ESP32_PORT = 8888

# Inicializa Socket UDP do Python
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

def send_joint_commands(base_rad, ombro_rad, cotovelo_rad, pulso_rad, garra_rad):
    """
    Empacota os 5 ângulos em floats binários (Padrão de rede Big-Endian) 
    e os despacha via UDP para o ESP32 em bancada.
    """
    # Formato "!5f"
    # ! = Network Byte Order (Big-Endian)
    # 5f = Cinco valores do tipo float (4 bytes cada)
    payload = struct.pack('!5f', base_rad, ombro_rad, cotovelo_rad, pulso_rad, garra_rad)
    
    # Envia os 20 bytes resultantes
    sock.sendto(payload, (ESP32_IP, ESP32_PORT))

# Exemplo de Teste de Bancada: Geração de Trajetória Senoidal Suave
print(f"Iniciando transmissão de trajetórias para {ESP32_IP}:{ESP32_PORT}...")
try:
    t = 0.0
    while True:
        # Gera trajetórias senoidais suaves de teste (-45 graus a +45 graus em radianos)
        angulo_base = 0.78 * math.sin(t)
        angulo_ombro = 0.5 * math.sin(t * 0.5)
        angulo_cotovelo = 0.6 * math.sin(t * 0.8)
        angulo_pulso = 0.0 # Mantém centrado
        angulo_garra = 0.0 # Garra fechada

        send_joint_commands(angulo_base, angulo_ombro, angulo_cotovelo, angulo_pulso, angulo_garra)
        
        print(f"Enviando [Base: {math.degrees(angulo_base):.1f}° | Cotovelo: {math.degrees(angulo_cotovelo):.1f}°]")
        
        # Envia a ~20Hz (a cada 50ms), ideal para suavização mecânica
        time.sleep(0.05)
        t += 0.05

except KeyboardInterrupt:
    print("\nTransmissão encerrada pelo usuário.")
finally:
    sock.close()
```
