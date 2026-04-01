#!/bin/bash
# Script de Instalación Automática para Mannequin Studio
# Soporta: Ubuntu/Debian, Fedora, Arch Linux, macOS

set -e  # Exit on error

echo "=========================================="
echo "  Mannequin Studio - Instalador Automático"
echo "=========================================="
echo ""

# Detectar sistema operativo
detect_os() {
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        if [ -f /etc/debian_version ]; then
            OS="debian"
            echo "✓ Sistema detectado: Debian/Ubuntu"
        elif [ -f /etc/fedora-release ]; then
            OS="fedora"
            echo "✓ Sistema detectado: Fedora"
        elif [ -f /etc/arch-release ]; then
            OS="arch"
            echo "✓ Sistema detectado: Arch Linux"
        else
            OS="unknown_linux"
            echo "⚠ Sistema Linux no reconocido"
        fi
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        OS="macos"
        echo "✓ Sistema detectado: macOS"
    else
        echo "✗ Sistema operativo no soportado"
        exit 1
    fi
}

# Instalar dependencias en Debian/Ubuntu
install_debian() {
    echo ""
    echo "📦 Instalando dependencias en Debian/Ubuntu..."
    
    sudo apt-get update
    
    # Herramientas de compilación
    sudo apt-get install -y \
        build-essential \
        cmake \
        git \
        pkg-config \
        wget
    
    # Vulkan
    sudo apt-get install -y \
        libvulkan-dev \
        vulkan-validationlayers-dev \
        vulkan-tools \
        mesa-vulkan-drivers
    
    # OpenGL y GLFW
    sudo apt-get install -y \
        libglfw3-dev \
        libgl1-mesa-dev \
        libglu1-mesa-dev \
        libxrandr-dev \
        libxinerama-dev \
        libxcursor-dev \
        libxi-dev
    
    # Librerías matemáticas
    sudo apt-get install -y \
        libglm-dev \
        libeigen3-dev
    
    # Assimp
    sudo apt-get install -y \
        libassimp-dev
    
    # Python
    sudo apt-get install -y \
        python3 \
        python3-dev \
        python3-pip
    
    # FFmpeg para exportación de video
    sudo apt-get install -y ffmpeg
    
    echo "✓ Dependencias instaladas correctamente"
}

# Instalar dependencias en Fedora
install_fedora() {
    echo ""
    echo "📦 Instalando dependencias en Fedora..."
    
    sudo dnf update -y
    
    sudo dnf install -y \
        gcc-c++ \
        cmake \
        git \
        pkg-config \
        wget \
        vulkan-loader-devel \
        vulkan-validation-layers \
        vulkan-tools \
        glfw-devel \
        mesa-libGL-devel \
        mesa-libGLU-devel \
        glm-devel \
        eigen3-devel \
        assimp-devel \
        python3 \
        python3-devel \
        python3-pip \
        ffmpeg
}

# Instalar dependencias en Arch Linux
install_arch() {
    echo ""
    echo "📦 Instalando dependencias en Arch Linux..."
    
    sudo pacman -Syu --noconfirm
    
    sudo pacman -S --noconfirm \
        base-devel \
        cmake \
        git \
        vulkan-icd-loader \
        vulkan-validation-layers \
        vulkan-tools \
        glfw-x11 \
        glu \
        glm \
        eigen \
        assimp \
        python \
        python-pip \
        ffmpeg
}

# Instalar dependencias en macOS
install_macos() {
    echo ""
    echo "📦 Instalando dependencias en macOS..."
    
    # Verificar Homebrew
    if ! command -v brew &> /dev/null; then
        echo "Instalando Homebrew..."
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    fi
    
    brew update
    
    brew install \
        cmake \
        git \
        vulkan-loader \
        vulkan-validation-layers \
        vulkan-tools \
        glfw \
        glm \
        eigen \
        assimp \
        python3 \
        ffmpeg
    
    # En macOS, necesitamos instalar el SDK de Vulkan
    echo ""
    echo "⚠ Nota: En macOS, Vulkan puede tener soporte limitado."
    echo "   Considere usar Metal o la implementación MoltenVK."
}

# Verificar Vulkan
verify_vulkan() {
    echo ""
    echo "🔍 Verificando instalación de Vulkan..."
    
    if command -v vulkaninfo &> /dev/null; then
        vulkaninfo --summary 2>/dev/null || echo "⚠ vulkaninfo ejecutado con advertencias"
        echo "✓ Vulkan instalado correctamente"
    else
        echo "⚠ Vulkan no encontrado o no funciona correctamente"
        echo "   Asegúrese de tener drivers actualizados para su GPU"
    fi
}

# Crear entorno virtual de Python
setup_python() {
    echo ""
    echo "🐍 Configurando entorno Python..."
    
    cd scripts
    
    if command -v python3 &> /dev/null; then
        python3 -m venv venv 2>/dev/null || true
        
        if [ -d "venv" ]; then
            source venv/bin/activate
            pip install --upgrade pip
            
            # Instalar paquetes Python necesarios
            pip install numpy pillow opencv-python-headless 2>/dev/null || true
            
            echo "✓ Entorno Python configurado"
        else
            echo "⚠ No se pudo crear el entorno virtual"
        fi
    fi
    
    cd ..
}

# Compilar proyecto
build_project() {
    echo ""
    echo "🔨 Compilando Mannequin Studio..."
    
    mkdir -p build
    cd build
    
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DBUILD_TESTS=OFF \
        -DUSE_VULKAN=ON
    
    make -j$(nproc)
    
    if [ -f "mannequin_app" ]; then
        echo "✓ Compilación exitosa!"
        echo ""
        echo "Ejecutable creado: $(pwd)/mannequin_app"
    else
        echo "✗ Error en la compilación"
        exit 1
    fi
    
    cd ..
}

# Mostrar instrucciones finales
show_instructions() {
    echo ""
    echo "=========================================="
    echo "  ¡Instalación Completada!"
    echo "=========================================="
    echo ""
    echo "Para ejecutar Mannequin Studio:"
    echo "  cd build && ./mannequin_app"
    echo ""
    echo "Controles:"
    echo "  - ESPACIO: Play/Pause animación"
    echo "  - CTRL+R: Iniciar/Detener grabación de video"
    echo "  - ESC: Salir"
    echo "  - Click izquierdo + arrastrar: Rotar cámara"
    echo "  - Rueda del mouse: Zoom"
    echo "  - Click derecho + arrastrar: Mover vista"
    echo ""
    echo "Formatos soportados:"
    echo "  Importar: .obj, .fbx, .gltf, .glb, .bvh"
    echo "  Exportar: .glb, .gltf, .fbx, .obj, .bvh, .mp4"
    echo ""
    echo "Documentación:"
    echo "  - README.md: Guía general"
    echo "  - docs/QUICKSTART.md: Inicio rápido"
    echo "  - docs/ARCHITECTURE.md: Arquitectura del sistema"
    echo ""
    echo "¡Disfruta creando personajes 3D!"
    echo ""
}

# Main
main() {
    detect_os
    
    case $OS in
        debian|ubuntu)
            install_debian
            ;;
        fedora)
            install_fedora
            ;;
        arch)
            install_arch
            ;;
        macos)
            install_macos
            ;;
        *)
            echo "✗ Sistema no soportado"
            exit 1
            ;;
    esac
    
    verify_vulkan
    setup_python
    build_project
    show_instructions
}

# Ejecutar
main "$@"
