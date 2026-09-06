# Guia de Bolso: Iniciando um Projeto ROS 2 Jazzy Jalisco do Zero

Este guia prático foi desenvolvido para o **GER Unicamp** como referência rápida para configurar um Workspace ROS 2 Jazzy do zero (assumindo que o ROS 2 Jazzy já esteja instalado no sistema operacional Ubuntu 24.04).

---

## 1. Configurando o Workspace do Zero

Toda a arquitetura e os pacotes do ROS 2 vivem dentro de um diretório central chamado **Workspace**. Siga a sequência abaixo para criá-lo e inicializá-lo:

```bash
# 1. Carregue as variáveis de ambiente globais do ROS 2 Jazzy
source /opt/ros/jazzy/setup.bash

# 2. Crie a estrutura física de diretórios (a pasta 'src' é obrigatória)
mkdir -p ~/ros2_ws/src
cd ~/ros2_ws

# 3. Rode uma compilação inicial para gerar as pastas do sistema (install, build, log)
colcon build

# 4. Carregue o workspace local criado no seu terminal atual
source install/setup.bash
```

*Nota: Recomenda-se adicionar o comando `source /opt/ros/jazzy/setup.bash` ao final do seu arquivo `~/.bashrc` (ou `~/.zshsh` se usar ZSH) para carregar o ROS automaticamente em todo terminal aberto.*

---

## 2. Criando um Pacote C++ / Descrição (`ament_cmake`)

Os pacotes baseados em CMake são o padrão do ecossistema ROS para **configuração de hardware, simulações e pacotes de descrição de robôs (URDF/XACRO)** que contenham arquivos estáticos que precisam ser organizados e instalados no sistema.

### Como criar:
Entre na pasta `src` do seu workspace e execute:
```bash
cd ~/ros2_ws/src
ros2 pkg create --build-type ament_cmake braco_ger_description --dependencies urdf xacro
```

### Arquivos estruturais gerados:
*   **`package.xml`**: Declara os metadados do pacote (versão, autor, licença) e suas dependências de compilação e execução (como `urdf`, `xacro`).
*   **`CMakeLists.txt`**: O arquivo de configuração principal para o compilador CMake. Nele, você declara quais executáveis C++ compilar e, crucialmente para robótica, **quais diretórios estáticos copiar para o sistema** (como as pastas `launch`, `rviz`, e `assets`/`meshes` contendo os arquivos STL exportados).

#### Exemplo de instalação de pastas no `CMakeLists.txt`:
```cmake
install(
  DIRECTORY
    rviz
    launch
    assets
  DESTINATION
    share/${PROJECT_NAME}/
)
```

---

## 3. Criando um Pacote Python (`ament_python`)

Pacotes em Python são ideais para **lógica de controle de alto nível, scripts de comportamento, interfaces gráficas simples e prototipagem rápida**. Como o Python é uma linguagem interpretada, ele dispensa processos de compilação em C++.

### Como criar:
Entre na pasta `src` do seu workspace e execute (opcionalmente definindo um nó inicial):
```bash
cd ~/ros2_ws/src
ros2 pkg create --build-type ament_python --node-name controlador_no teste_controle
```

### Arquivos estruturais gerados:
*   **`package.xml`**: Assim como no CMake, declara as dependências de sistema (ex: `<exec_depend>rclpy</exec_depend>`).
*   **`setup.py`**: Substitui o arquivo do CMake. Utiliza a biblioteca nativa `setuptools` do Python para mapear o pacote. É nele que você define os **pontos de entrada (entry points)** para que o ROS saiba quais scripts Python rodar como nós.
*   **`setup.cfg`**: Configurações de diretórios internos para a instalação do Python.

#### Exemplo de mapeamento de nós no `setup.py`:
```python
entry_points={
    'console_scripts': [
        'controlador_no = teste_controle.controlador_no:main',
        'talker_basico = teste_controle.talker_basico:main',
    ],
},
```

---

## 4. Comparativo de Arquitetura: CMake vs. Python

| Característica | `ament_cmake` (CMake) | `ament_python` (Python) |
| :--- | :--- | :--- |
| **Linguagem Principal** | C++ (ou híbrido) | Python Puro |
| **Compilação** | Sim (gera binários otimizados) | Não (interpretado em tempo de execução) |
| **Desempenho** | Altíssimo (ideal para algoritmos de controle rígidos de tempo real) | Médio/Alto (ideal para coordenação de tarefas) |
| **Gerenciador de Build** | `CMakeLists.txt` | `setup.py` & `setup.cfg` |
| **Caso de Uso no GER** | Guardar o URDF, as malhas 3D (`assets/`), os nós C++ do `ros2_control` e arquivos de launch. | Criar a biblioteca em Python para controle manual do braço por teclado/GUI. |

---

## 5. Fluxo Diário de Compilação e Execução

Sempre que criar um pacote novo, adicionar dependências ou alterar códigos, você precisará atualizar o sistema:

```bash
# 1. Retorne à raiz do workspace
cd ~/ros2_ws

# 2. Atualize dependências do sistema de forma automatizada (se necessário)
rosdep update
rosdep install --from-paths src --ignore-src -y

# 3. Compile pacotes novos ou alterados
colcon build --packages-select nome_do_seu_pacote

# 4. Recarregue o Workspace local (OBRIGATÓRIO em cada novo terminal)
source install/setup.bash

# 5. Execute o seu nó no ROS 2
ros2 run nome_do_seu_pacote nome_do_executavel
```

*Dica de desenvolvimento:* Use a flag `colcon build --symlink-install` para pacotes Python. Ela cria links simbólicos para os seus scripts, permitindo que você altere o código Python e o execute imediatamente sem precisar compilar com `colcon build` de novo a cada alteração pequena!
