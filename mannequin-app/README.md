# Mannequin Studio - Advanced 3D Character Creation Tool

![Version](https://img.shields.io/badge/version-1.0.0-blue)
![License](https://img.shields.io/badge/license-MIT-green)
![C++](https://img.shields.io/badge/C%2B%2B-17-orange)
![Vulkan](https://img.shields.io/badge/Vulkan-1.2-red)

## 📖 Descripción General

**Mannequin Studio** es una aplicación profesional de código abierto para crear, animar y exportar modelos 3D de personajes humanos/maniquíes con movimiento realista y exportación directa a video. Diseñado para artistas 3D, desarrolladores de juegos, y profesionales de la animación.

### ✨ Características Principales

- **Creación de Personajes Paramétrica**: Ajusta altura, peso, proporciones corporales con sliders intuitivos
- **Sistema de Blend Shapes**: Modifica rasgos faciales y corporales sin romper la topología
- **Rigging Profesional**: Esqueleto humano completo con 54+ huesos y soporte IK/FK
- **Animación en Tiempo Real**: Timeline con keyframes, interpolación suave y loops
- **Importación Mocap**: Soporte nativo para archivos BVH de captura de movimiento
- **Renderizado Multi-Modo**: Wireframe limpio, MatCap shading, X-Ray vision
- **Exportación Versátil**: GLB, FBX, OBJ, BVH, y video MP4 (H.264)
- **Scripting Python**: Automatiza flujos de trabajo con API completa

---

## 🛠️ Stack Tecnológico

### Lenguajes Principales
| Lenguaje | Uso | Versión |
|----------|-----|---------|
| **C++** | Motor principal, renderizado, física | C++17 |
| **Python** | Scripting, automatización, IA | 3.8+ |
| **GLSL** | Shaders de renderizado | 4.5+ |

### Librerías y APIs
| Librería | Propósito | Versión |
|----------|-----------|---------|
| **Vulkan** | API gráfica de bajo nivel | 1.2+ |
| **OpenGL** | Fallback gráfico | 4.6 |
| **GLM** | Matemáticas gráficas | 0.9.9+ |
| **Eigen** | Álgebra lineal avanzada | 3.4+ |
| **Assimp** | Import/export 3D | 5.0+ |
| **Dear ImGui** | Interfaz de usuario | 1.89+ |
| **GLFW** | Gestión de ventanas | 3.3+ |
| **FFmpeg** | Codificación de video | 4.4+ |

---

## 📦 Formatos Soportados

### Importación
| Formato | Extensión | Uso | Notas |
|---------|-----------|-----|-------|
| Wavefront OBJ | `.obj` | Mallas estáticas | Con .mtl |
| Autodesk FBX | `.fbx` | Mallas + animación | Binary/ASCII |
| glTF/GLB | `.gltf`, `.glb` | Web/Real-time | Recomendado |
| Biovision Hierarchy | `.bvh` | Captura de movimiento | Solo animación |
| Collada | `.dae` | Intercambio 3D | |

### Exportación
| Formato | Extensión | Incluye | Tamaño |
|---------|-----------|---------|--------|
| **glTF Binary** | `.glb` | Mesh, skeleton, anim, blend shapes | ⭐ Recomendado |
| glTF | `.gltf` | Mesh, skeleton, anim | JSON + binario |
| Autodesk FBX | `.fbx` | Todo + metadata | Compatible |
| Wavefront OBJ | `.obj` | Solo malla | Estático |
| BVH | `.bvh` | Solo animación | Para mocap |
| **Video MP4** | `.mp4` | Render H.264 | 1080p/4K |

---

## 🚀 Instalación Rápida

### Métodos de Instalación por Plataforma

| Plataforma | Script Automático | Comando |
|------------|------------------|---------|
| **Linux** (Ubuntu/Debian/Fedora/Arch) | ✅ Bash | `./scripts/install_dependencies.sh` |
| **macOS** | ✅ Bash + Homebrew | `./scripts/install_dependencies.sh` |
| **Windows** | ✅ PowerShell | `.\scripts\install_dependencies.ps1` |

---

### Método Automático (Recomendado)

#### Linux / macOS

```bash
# Clonar repositorio
git clone https://github.com/tu-usuario/mannequin-studio.git
cd mannequin-studio

# Dar permisos de ejecución
chmod +x scripts/install_dependencies.sh

# Ejecutar instalador automático
./scripts/install_dependencies.sh

# Opción: saltar compilación automática
./scripts/install_dependencies.sh --skip-build
```

El script de Linux/macOS:
1. ✅ Detecta tu sistema operativo (Ubuntu, Debian, Fedora, Arch, macOS)
2. ✅ Instala todas las dependencias necesarias (Vulkan, GLFW, GLM, Eigen, Assimp, FFmpeg)
3. ✅ Configura entorno virtual Python con numpy, PyQt5, PyOpenGL
4. ✅ Verifica instalación de Vulkan SDK
5. ✅ Compila el proyecto automáticamente (opcional con --skip-build)

#### Windows

```powershell
# Clonar repositorio
git clone https://github.com/tu-usuario/mannequin-studio.git
cd mannequin-studio

# Ejecutar instalador PowerShell (como Administrador)
Set-ExecutionPolicy -Scope Process -Unrestricted
.\scripts\install_dependencies.ps1

# Opciones avanzadas:
.\scripts\install_dependencies.ps1 -SkipVcpkg    # Si ya tienes vcpkg configurado
.\scripts\install_dependencies.ps1 -NoBuild      # Solo instalar dependencias
```

El script de Windows:
1. ✅ Verifica prerrequisitos (Git, Visual Studio C++ Tools)
2. ✅ Ofrece instalar **vcpkg** para gestión de paquetes C++
3. ✅ Instala dependencias: Vulkan, GLFW, GLM, Eigen, Assimp vía vcpkg
4. ✅ Descarga FFmpeg portable si no está en PATH
5. ✅ Crea entorno virtual Python e instala dependencias
6. ✅ Genera solución de Visual Studio 2022 (.sln)

> **⚠️ Nota para Windows**: El script requiere ejecutarse como **Administrador** para instalar algunas dependencias. Si no tienes permisos de administrador, instala manualmente vcpkg y las librerías.

---

### Requisitos Previos por Plataforma

#### Windows
| Software | Versión | Enlace |
|----------|---------|--------|
| Visual Studio 2022 | Community o superior | [Descargar](https://visualstudio.microsoft.com/) |
| Componente requerido | "Desarrollo para el escritorio con C++" | Incluido en VS Installer |
| Git para Windows | 2.x+ | [Descargar](https://git-scm.com/download/win) |
| Python | 3.8+ | [Descargar](https://www.python.org/downloads/) |

#### Linux (Ubuntu/Debian)
```bash
# Mínimo recomendado: Ubuntu 20.04+ o Debian 11+
sudo apt update
sudo apt install -y git curl build-essential
```

#### macOS
| Software | Versión | Enlace |
|----------|---------|--------|
| Xcode Command Line Tools | Latest | `xcode-select --install` |
| Homebrew | Latest | [Instalar](https://brew.sh/) |

### Instalación Manual (Alternativa)

Si prefieres no usar los scripts automáticos o tienes una configuración especial:

#### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential cmake git pkg-config \
    libvulkan-dev vulkan-validationlayers-dev vulkan-tools \
    libglfw3-dev libglm-dev libeigen3-dev libassimp-dev \
    python3-dev python3-pip ffmpeg
```

#### Fedora
```bash
sudo dnf install -y \
    gcc-c++ cmake git vulkan-loader-devel \
    glfw-devel glm-devel eigen3-devel assimp-devel \
    python3-devel ffmpeg
```

#### Arch Linux
```bash
sudo pacman -S base-devel cmake vulkan-icd-loader \
    glfw-x11 glm eigen assimp python ffmpeg
```

#### macOS (con Homebrew)
```bash
brew install cmake vulkan-loader glfw glm eigen assimp python ffmpeg
```

---

## 🔨 Compilación

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Ejecutar
./mannequin_app
```

### Opciones de CMake
| Opción | Valor por defecto | Descripción |
|--------|------------------|-------------|
| `USE_VULKAN` | ON | Usar Vulkan (OFF = OpenGL) |
| `BUILD_TESTS` | OFF | Compilar tests unitarios |
| `CMAKE_BUILD_TYPE` | Release | Debug/Release/RelWithDebInfo |

---

## 🎮 Uso e Interfaz

### Controles del Viewport 3D

| Acción | Control |
|--------|---------|
| Rotar cámara | Click izquierdo + arrastrar |
| Mover vista (Pan) | Click derecho + arrastrar |
| Zoom | Rueda del mouse |
| Orbita libre | Alt + click izquierdo |
| Resetear cámara | Doble click en viewport |

### Atajos de Teclado

| Tecla | Función |
|-------|---------|
| `ESPACIO` | Play/Pause animación |
| `CTRL + R` | Iniciar/Detener grabación de video |
| `ESC` | Salir de la aplicación |
| `F` | Enfocar selección |
| `W` | Toggle wireframe |
| `X` | Toggle X-Ray mode |
| `M` | Toggle MatCap shading |
| `1-5` | Cambiar LOD (Level of Detail) |

### Paneles de la Interfaz

#### 1. Viewport Central
- Visualización 3D en tiempo real
- Selector de modo: Shaded / Wireframe / X-Ray
- Información de FPS y conteo de polígonos

#### 2. Property Panel (Izquierda)
- **Human Scale**: Altura (1.4-2.1m), Peso (40-150kg)
- **Limb Proportions**: Largo de brazos, piernas, torso
- **Anatomy**: Masa muscular, pecho, cadera, cintura, cabeza
- **Blend Shapes**: Sliders para expresiones faciales

#### 3. Animation Timeline (Inferior)
- Controls: Play, Pause, Stop, Rewind
- Scrubber de tiempo
- Editor de keyframes por hueso
- Importar/exportar animaciones

#### 4. Export Hub (Derecha)
- Selector de formato (GLB, FBX, OBJ, BVH, MP4)
- Level of Detail (LOD 0-4)
- Opciones: animación, esqueleto, blend shapes
- Configuración de video (FPS, resolución)

---

## 📁 Estructura del Proyecto

```
mannequin-studio/
├── CMakeLists.txt              # Configuración de compilación
├── README.md                   # Esta documentación
├── LICENSE                     # Licencia MIT
├── scripts/
│   ├── install_dependencies.sh # Instalador Linux/macOS (Bash)
│   ├── install_dependencies.ps1 # Instalador Windows (PowerShell)
│   ├── README_INSTALL.md       # Guía detallada de instalación
│   └── mannequin_api.py        # API Python con ejemplos
├── src/
│   ├── main.cpp                # Entry point
│   ├── core/
│   │   ├── engine.cpp/.h       # Motor principal
│   │   ├── mesh.cpp/.h         # Sistema de mallas
│   │   └── skeleton.h          # Rigging y huesos
│   ├── render/
│   │   ├── renderer.cpp/.h     # Renderizador Vulkan
│   │   ├── shader.cpp          # Shaders
│   │   └── camera.cpp          # Cámara 3D
│   ├── ui/
│   │   ├── interface.cpp/.h    # UI con Dear ImGui
│   │   ├── viewport.cpp        # Viewport 3D
│   │   ├── timeline.cpp        # Timeline de animación
│   │   └── panels.cpp          # Paneles de propiedades
│   └── utils/
│       └── python_bridge.cpp   # Integración Python
├── assets/
│   ├── meshes/                 # Modelos base humanos
│   ├── textures/               # MatCaps y texturas
│   └── shaders/                # Shaders GLSL
│       ├── mesh.vert.glsl      # Vertex shader (GPU skinning)
│       └── mesh.frag.glsl      # Fragment shader
├── docs/
│   ├── ARCHITECTURE.md         # Arquitectura detallada
│   ├── QUICKSTART.md           # Guía de inicio rápido
│   └── API_REFERENCE.md        # Documentación de API
└── build/                      # Directorio de compilación
```

---

## 💻 Ejemplos de Uso

### Script Python: Crear Pose Personalizada

```python
#!/usr/bin/env python3
# scripts/create_pose.py

import sys
sys.path.append('build')
import mannequin

# Obtener instancia del motor
engine = mannequin.get_engine()

# Cargar personaje base
engine.load_character("assets/meshes/base_human.glb")

# Crear pose de saludo
skeleton = engine.get_skeleton()
skeleton.set_rotation("Arm_R", [1.5, 0.3, 0])  # Brazo derecho arriba
skeleton.set_rotation("Forearm_R", [-0.5, 0, 0])  # Antebrazo doblado
skeleton.set_rotation("Hand_R", [0, 0, 0.3])  # Mano abierta

# Ajustar parámetros corporales
engine.set_body_param("height", 1.80)
engine.set_body_param("muscle_mass", 0.7)

# Renderizar y guardar
engine.render_frame("output/salute_pose.png")

# Exportar personaje en pose
engine.export("output/salute_character.glb", format="glb")

print("¡Pose creada exitosamente!")
```

### Script Python: Animación con Mocap

```python
#!/usr/bin/env python3
# scripts/apply_mocap.py

import mannequin

engine = mannequin.get_engine()
engine.load_character("assets/meshes/athlete.glb")

# Importar animación de captura de movimiento
engine.load_animation("mocap/walk_cycle.bvh")

# Configurar loop
animation = engine.get_animation()
animation.set_loop(True)

# Exportar video de la animación
engine.start_video_recording(
    "output/walk_cycle.mp4",
    fps=60,
    width=1920,
    height=1080
)

# Reproducir y capturar frames
engine.play()
for i in range(180):  # 3 segundos a 60fps
    engine.update(1/60)
    engine.render()

engine.stop_video_recording()
print("Video exportado: output/walk_cycle.mp4")
```

### Batch Processing con Python

```python
#!/usr/bin/env python3
# scripts/batch_export.py

import os
import mannequin

engine = mannequin.get_engine()

# Lista de personajes para procesar
characters = [
    {"file": "male_01.glb", "height": 1.75, "weight": 75},
    {"file": "female_01.glb", "height": 1.65, "weight": 60},
    {"file": "male_02.glb", "height": 1.85, "weight": 90},
]

for char in characters:
    engine.load_character(f"assets/{char['file']}")
    engine.set_body_param("height", char["height"])
    engine.set_body_param("weight", char["weight"])
    
    # Exportar en múltiples formatos
    name = char["file"].split(".")[0]
    engine.export(f"output/{name}.glb", format="glb")
    engine.export(f"output/{name}.fbx", format="fbx")
    
    print(f"✓ Procesado: {name}")

print("Batch completado!")
```

---

## 🎯 Flujo de Trabajo Típico

### 1. Creación del Personaje
1. Abrir Mannequin Studio
2. En **Property Panel**, ajustar:
   - Altura y peso
   - Proporciones de extremidades
   - Masa muscular y anatomía
3. Refinar con **Blend Shapes** para rasgos únicos

### 2. Animación
1. Ir al **Timeline** inferior
2. Opciones:
   - **Keyframe manual**: Posar hueso por hueso
   - **Importar BVH**: Cargar captura de movimiento
3. Ajustar curva de interpolación
4. Previsualizar en tiempo real

### 3. Renderizado y Exportación
1. Seleccionar modo de visualización (Wireframe recomendado para estilo técnico)
2. En **Export Hub**:
   - Elegir formato (.glb recomendado)
   - Configurar LOD según necesidad
   - Marcar opciones (animación, skeleton, etc.)
3. Click en **Export** o grabar video con `CTRL+R`

---

## 🔧 Solución de Problemas

### Vulkan no se inicializa
```bash
# Verificar instalación
vulkaninfo --summary

# Instalar drivers (NVIDIA)
sudo apt install nvidia-driver-525

# Instalar drivers (AMD)
sudo apt install mesa-vulkan-drivers
```

### Error de compilación: Eigen no encontrado
```bash
# Ubuntu/Debian
sudo apt install libeigen3-dev

# Verificar ruta
pkg-config --cflags eigen3
```

### Bajo rendimiento en viewport
- Reducir LOD en Export Hub
- Desactivar sombras en settings
- Usar modo Wireframe en lugar de MatCap

### FFmpeg no disponible para video
```bash
sudo apt install ffmpeg
# Verificar
ffmpeg -version
```

---

## 📚 Recursos Adicionales

- **Documentación Completa**: `docs/ARCHITECTURE.md`
- **Guía de Inicio Rápido**: `docs/QUICKSTART.md`
- **Referencia de API Python**: `docs/API_REFERENCE.md`
- **Ejemplos de Scripts**: `scripts/`

### Tutoriales Externos
- [Introducción a Vulkan](https://vulkan-tutorial.com/)
- [Blender para preparación de assets](https://www.blender.org/)
- [Formato glTF especificación](https://www.khronos.org/gltf/)

---

## 🤝 Contribuciones

Las contribuciones son bienvenidas! Por favor:

1. Fork el repositorio
2. Crea una rama (`git checkout -b feature/nueva-caracteristica`)
3. Commit tus cambios (`git commit -m 'Añadir nueva característica'`)
4. Push a la rama (`git push origin feature/nueva-caracteristica`)
5. Abre un Pull Request

Lee `CONTRIBUTING.md` para detalles del código de conducta y proceso de submit.

---

## 📄 Licencia

Este proyecto está bajo la licencia **MIT** - ver el archivo [LICENSE](LICENSE) para detalles.

---

## 👥 Autores y Reconocimientos

- **Motor Principal**: Equipo de Desarrollo Mannequin
- **Inspiración**: ZBrush, Character Creator, MakeHuman
- **Contribuidores**: [Ver lista completa](../../graphs/contributors)

---

## 🗺️ Roadmap

### Versión 1.0 (Actual)
- ✅ Creación paramétrica de personajes
- ✅ Sistema de animación básico
- ✅ Exportación a múltiples formatos
- ✅ Grabación de video

### Versión 1.1 (Próximamente)
- [ ] Biblioteca de prendas y accesorios
- [ ] Física de tela en tiempo real
- [ ] Auto-rigging con IA
- [ ] Soporte para VR

### Versión 2.0 (Futuro)
- [ ] Renderizado en la nube
- [ ] Colaboración multi-usuario
- [ ] Plugin system
- [ ] Marketplace de assets

---

## 📞 Soporte y Contacto

- **Issues**: [GitHub Issues](../../issues)
- **Discusiones**: [GitHub Discussions](../../discussions)
- **Email**: support@mannequin-studio.dev

---

<div align="center">

**¿Te gusta Mannequin Studio?** ¡Danos una estrella! ⭐

[![Star History](https://api.star-history.com/svg?repos=tu-usuario/mannequin-studio&type=Date)](https://star-history.com/#tu-usuario/mannequin-studio&Date)

</div>
