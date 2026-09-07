import socket
from braco_packet import braco_packet

# Configurações de rede
ESP32_IP = "192.168.15.43"
UDP_PORT = 3030

def main():

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    packet = braco_packet(auth=13579, command=39, angles=[12.5, 45.0, 90.0, 180.0, 0.0], extra1=100, extra2=200)
    packed_data = packet.get_packed_data()

    sock.sendto(packed_data, (ESP32_IP, UDP_PORT))
    print(f"Enviado pacote de {len(packed_data)} bytes para {ESP32_IP}:{UDP_PORT}")


if __name__ == "__main__":
    main()