## Sobre o `appconfig.h`

O arquivo `appconfig.h` contém constantes importantes para o funcionamento da aplicação e está no `.gitignore` por questões de segurança. Ele tem a **estruttura abaixo** e deve ser configurado individualmente para cada caso.


```cpp
#pragma once

// Configurações de Rede
const char* WIFI_SSID = "NOME_DA_SUA_REDE";
const char* WIFI_PASSWORD = "SENHA_DA_SUA_REDE";

// código de autenticação de pacote recebido
const int32_t AUTH_PACKET = 1234;

// Configurações UDP
const uint16_t UDP_PORT = 1234;
```