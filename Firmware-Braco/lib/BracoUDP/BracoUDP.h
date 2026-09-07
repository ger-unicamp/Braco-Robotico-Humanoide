#pragma once

#include <Arduino.h>

// Pragma pack garante que o compilador não adicione bytes de alinhamento
#pragma pack(push, 1)

struct BracoPacket {
    int32_t auth;
    int32_t command;
    float angles[5];
    int32_t extra1;
    int32_t extra2;
};

#pragma pack(pop)

/// @brief Inicia a conexão WiFi e o UDP
/// @param ssid nome da rede WiFi
/// @param password senha da rede WiFi
/// @param udp_port porta UDP
/// @param led_con LED que piscará enquanto o ESP32 estiver tentando se conectar ao WiFi
void beginWifiUDP(const char* ssid, const char* password, uint16_t udp_port, int led_con);

/// @brief Verifica se há um pacote UDP disponível e válido, e o coloca em p_packet. Retorna true se houver um pacote válido, false caso contrário.
/// @param p_packet ponteiro para um strcut BracoPacket onde o pacote recebido será armazenado
/// @param auth_packet código de autenticação de pacote
/// @return true se houver um pacote válido, false caso contrário
bool isTherePacket(BracoPacket* p_packet, int32_t auth_packet);