import sys
if sys.prefix == '/home/rafachoyos/.local/share/pipx/venvs/colcon-core':
    sys.real_prefix = sys.prefix
    sys.prefix = sys.exec_prefix = '/home/rafachoyos/Documents/Facul/GER/BracoRobotico/Braco-Robotico-Humanoide/ros2_ws/install/teste_comunica'
