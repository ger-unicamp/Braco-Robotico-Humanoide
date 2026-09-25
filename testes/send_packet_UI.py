import math
import socket
import tkinter as tk
from tkinter import ttk
from braco_packet import braco_packet

# Configurações de rede
ESP32_IP = "10.50.228.150"
UDP_PORT = 3030

class BracoControllerApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Controle do Braço Robótico")
        self.root.geometry("420x300")
        self.root.resizable(False, False)

        # Socket UDP (mantido aberto durante a execução)
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

        # Configurações iniciais dos ângulos
        initial_angles = [0, 90, 90, 90, 45]
        labels_juntas = ["Junta 1 (Base)", "Junta 2 (Ombro)", "Junta 3 (Cotovelo)", "Junta 4 (Pulso)", "Junta 5 (Garra)"]
        
        self.angle_vars = []

        # Construção da interface gráfica
        for i in range(5):
            frame = ttk.Frame(root)
            frame.pack(fill="x", padx=15, pady=8)

            # Rótulo do nome da junta
            lbl = ttk.Label(frame, text=labels_juntas[i], width=16)
            lbl.pack(side="left")

            # Variável e Slider (0° a 180°)
            var = tk.IntVar(value=initial_angles[i])
            self.angle_vars.append(var)

            slider = ttk.Scale(
                frame,
                from_=0,
                to=180,
                value=initial_angles[i],
                command=lambda val, idx=i: self._on_slider_move(idx, val)
            )
            slider.pack(side="left", fill="x", expand=True, padx=10)

            # Mostrador numérico do ângulo atual
            val_lbl = ttk.Label(frame, textvariable=var, width=4)
            val_lbl.pack(side="right")

        # Status do envio
        self.status_var = tk.StringVar(value="Pronto")
        status_bar = ttk.Label(root, textvariable=self.status_var, relief="sunken", anchor="w")
        status_bar.pack(side="bottom", fill="x", padx=5, pady=5)

        # Envia o pacote com os valores iniciais na inicialização
        self.send_packet()

    def _on_slider_move(self, index, value):
        # Atualiza a variável inteira e envia o pacote
        self.angle_vars[index].set(int(float(value)))
        self.send_packet()

    def send_packet(self):
        # Obtém a lista com os 5 ângulos atuais
        angles = [math.radians(var.get()) for var in self.angle_vars]

        # Monta e empacota a estrutura
        packet = braco_packet(
            auth=13579, 
            command=39, 
            angles=angles, 
            extra1=100, 
            extra2=200
        )
        packed_data = packet.get_packed_data()

        # Envia via UDP
        try:
            self.sock.sendto(packed_data, (ESP32_IP, UDP_PORT))
            self.status_var.set(f"Enviado: {angles} ({len(packed_data)} bytes)")
        except Exception as e:
            self.status_var.set(f"Erro ao enviar: {e}")

    def on_close(self):
        self.sock.close()
        self.root.destroy()

if __name__ == "__main__":
    root = tk.Tk()
    app = BracoControllerApp(root)
    root.protocol("WM_DELETE_WINDOW", app.on_close)
    root.mainloop()
