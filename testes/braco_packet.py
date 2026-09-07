import struct

class braco_packet:
    def __init__(self, auth, command, angles, extra1, extra2):
        self.auth = auth
        self.command = command
        self.angles = angles
        self.extra1 = extra1
        self.extra2 = extra2

    def get_packed_data(self):
        import struct
        # Formato:
        # '<' = Little-Endian
        # '2i' = 2 inteiros de 32 bits (auth e command) (4 bytes cada = 8 bytes)
        # '5f' = 5 floats (angles) (4 bytes cada = 20 bytes)
        # '2i' = 2 inteiros de 32 bits (extra1 e extra2) (4 bytes cada = 8 bytes)
        # Total = 36 bytes
        return struct.pack('<2i5f2i', self.auth, self.command, *self.angles, self.extra1, self.extra2)

