#include "BracoUDP.h"
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>

WiFiUDP udp;

void beginWifiUDP(const char* ssid, const char* password, uint16_t udp_port, int led_con)
{
    WiFi.begin(ssid, password);

    Serial.print("Conectando ao WiFi");

    while (WiFi.status() != WL_CONNECTED) {
        digitalWrite(led_con, !digitalRead(led_con));
        delay(500);
        Serial.print(".");
    }

    Serial.print("\nConectado! IP do ESP32: ");
    Serial.println(WiFi.localIP());

    for (int i = 0; i < 5; i++)
    {
        digitalWrite(led_con, HIGH);
        delay(200);
        digitalWrite(led_con, LOW);
        delay(200);
    }

    udp.begin(udp_port);
}

bool isTherePacket(BracoPacket* p_packet, int32_t auth_packet)
{
    int packetSize = udp.parsePacket();

    if (packetSize == sizeof(BracoPacket)) {
        udp.read((char*)p_packet, sizeof(BracoPacket));

        if (p_packet->auth != auth_packet)
        {
            Serial.println("[AVISO] Pacote com autenticação ERRADA recebido, cancelando recebimento!");
            return false;
        }

        return true;
    }

    return false;
}