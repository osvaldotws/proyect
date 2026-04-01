#!/bin/bash
# Mannequin Studio - Cross-Platform Dependency Installer
# Soporta: Ubuntu/Debian, Fedora/RHEL, Arch Linux, macOS
# Uso: ./install_dependencies.sh [--skip-build]

set -e  # Salir en caso de error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build"
SKIP_BUILD=false

# Colores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Parsear argumentos
while [[ $# -gt 0 ]]; do
    case $1 in
        --skip-build)
            SKIP_BUILD=true
            shift
            ;;
        *)
            echo "Uso: $0 [--skip-build]"
            exit 1
            ;;
    esac
done

echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}  Mannequin Studio: Install Script     ${NC}"
echo -e "${CYAN}========================================${NC}"

# Detectar SO
detect_os() {
    if [[ "$OSTYPE" == "darwin"* ]]; then
        echo "macos"
    elif [[ -f /etc/os-release ]]; then
        . /etc/os-release
        if [[ "$ID" == "ubuntu" || "$ID" == "debian" || "$ID" == "linuxmint" ]]; then
            echo "debian"
        elif [[ "$ID" == "fedora" || "$ID" == "rhel" || "$ID" == "centos" ]]; then
            echo "fedora"
        elif [[ "$ID" == "arch" || "$ID" == "manjaro" ]]; then
            echo "arch"
        else
            echo "unknown"
        fi
    else
        echo "unknown"
    fi
}

OS=$(detect_os)
echo -e "\n${YELLOW}[1/5] Detectando sistema operativo...${NC}"
echo "  Sistema detectado: $OS"

# Funciones de instalación por plataforma
install_deps_debian() {
    echo -e "${YELLOW}Instalando dependencias para Debian/Ubuntu...${NC}"
    sudo apt-get update
    sudo apt-get install -y \
        git \
        cmake \
        build-essential \
        libvulkan-dev \
        vulkan-tools \
        libglfw3-dev \
        libglm-dev \
        libeigen3-dev \
        libassimp-dev \
        python3 \
        python3-pip \
        python3-venv \
        ffmpeg \
        libwayland-dev \
        libxkbcommon-dev \
        libxrandr-dev \
        libxi-dev \
        libxcursor-dev \
        libxinerama-dev
}

install_deps_fedora() {
    echo -e "${YELLOW}Instalando dependencias para Fedora/RHEL...${NC}"
    sudo dnf install -y \
        git \
        cmake \
        gcc-c++ \
        vulkan-loader-devel \
        vulkan-headers \
        glfw-devel \
        glm-devel \
        eigen3-devel \
        assimp-devel \
        python3 \
        python3-pip \
        python3-virtualenv \
        ffmpeg \
        ffmpeg-devel
}

install_deps_arch() {
    echo -e "${YELLOW}Instalando dependencias para Arch/Manjaro...${NC}"
    sudo pacman -S --noconfirm \
        git \
        cmake \
        base-devel \
        vulkan-icd-loader \
        vulkan-headers \
        glfw-x11 \
        glm \
        eigen \
        assimp \
        python \
        python-pip \
        python-virtualenv \
        ffmpeg
}

install_deps_macos() {
    echo -e "${YELLOW}Instalando dependencias para macOS...${NC}"
    if ! command -v brew &> /dev/null; then
        echo "Homebrew no encontrado. Instalando..."
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    fi
    brew install \
        git \
        cmake \
        vulkan-loader \
        vulkan-headers \
        vulkan-validation-layers \
        glfw \
        glm \
        eigen \
        assimp \
        python@3.11 \
        ffmpeg
}

# Instalar dependencias según SO
echo -e "\n${YELLOW}[2/5] Instalando dependencias del sistema...${NC}"
case $OS in
    debian)
        install_deps_debian
        ;;
    fedora)
        install_deps_fedora
        ;;
    arch)
        install_deps_arch
        ;;
    macos)
        install_deps_macos
        ;;
    *)
        echo -e "${RED}Sistema operativo no soportado automáticamente.${NC}"
        echo "Por favor instala manualmente: CMake, Vulkan SDK, GLFW, GLM, Eigen, Assimp, Python3, FFmpeg"
        exit 1
        ;;
esac
echo -e "${GREEN}  [OK] Dependencias del sistema instaladas${NC}"

# Configurar entorno Python
echo -e "\n${YELLOW}[3/5] Configurando entorno Python...${NC}"
cd "$PROJECT_ROOT"
if [ ! -d "venv" ]; then
    python3 -m venv venv
fi
source venv/bin/activate
pip install --upgrade pip
pip install numpy PyQt5 PyOpenGL
echo -e "${GREEN}  [OK] Entorno Python configurado${NC}"

# Descargar Vulkan SDK si es necesario (solo Linux/macOS sin paquetes)
if [[ "$OS" != "debian" ]] || ! dpkg -l | grep -q libvulkan-dev; then
    echo -e "\n${YELLOW}[4/5] Verificando Vulkan SDK...${NC}"
    if command -v vulkaninfo &> /dev/null; then
        echo -e "${GREEN}  [OK] Vulkan SDK ya está instalado${NC}"
    else
        echo "  Vulkan SDK no encontrado. Puedes instalarlo desde:"
        echo "  - Linux: https://vulkan.lunarg.com/sdk/home#linux"
        echo "  - macOS: brew install vulkan-loader vulkan-headers"
        echo "  - Windows: https://vulkan.lunarg.com/sdk/home#windows"
    fi
fi

# Compilar proyecto (opcional)
if [ "$SKIP_BUILD" = false ]; then
    echo -e "\n${YELLOW}[5/5] Compilando proyecto...${NC}"
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    # Detectar generador de CMake
    CMAKE_GENERATOR=""
    if [[ "$OS" == "macos" ]]; then
        CMAKE_GENERATOR="-G \"Unix Makefiles\""
    else
        CMAKE_GENERATOR="-G \"Unix Makefiles\""
    fi
    
    cmake $CMAKE_GENERATOR ..
    make -j$(nproc)
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}  [OK] Proyecto compilado exitosamente${NC}"
        echo -e "  Binario generado: $BUILD_DIR/mannequin-studio"
    else
        echo -e "${RED}  [ERROR] La compilación falló. Revisa los mensajes anteriores.${NC}"
        exit 1
    fi
else
    echo -e "\n${YELLOW}[5/5] Saltando compilación (--skip-build activado)${NC}"
fi

echo -e "\n${GREEN}========================================${NC}"
echo -e "${GREEN}  ¡Instalación Completada!             ${NC}"
echo -e "${GREEN}========================================${NC}"

echo -e "\n${CYAN}Siguientes pasos:${NC}"
echo "1. Activa el entorno Python: source venv/bin/activate"
if [ "$SKIP_BUILD" = false ]; then
    echo "2. Ejecuta la aplicación: ./build/mannequin-studio"
else
    echo "2. Compila manualmente: cd build && cmake .. && make"
    echo "3. Ejecuta: ./build/mannequin-studio"
fi
echo "3. Para scripting Python: python scripts/mannequin_api.py"
echo ""
echo -e "${YELLOW}Nota para Windows:${NC}"
echo "  En Windows, usa el script PowerShell: scripts/install_dependencies.ps1"
echo "  Requiere PowerShell ejecutado como Administrador."
