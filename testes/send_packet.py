import math
import socket
from braco_packet import braco_packet

# Configurações de rede
ESP32_IP = "10.50.228.150"
UDP_PORT = 3030

def main():

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    angles_degree = [0, 90, 90, 90, 45]

    packet = braco_packet(auth=13579, command=39, angles=[math.radians(a) for a in angles_degree], extra1=100, extra2=200)
    packed_data = packet.get_packed_data()

    sock.sendto(packed_data, (ESP32_IP, UDP_PORT))
    print(f"Enviado pacote de {len(packed_data)} bytes para {ESP32_IP}:{UDP_PORT}")


if __name__ == "__main__":
    main()