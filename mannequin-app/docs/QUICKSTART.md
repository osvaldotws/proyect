# Guía de Inicio Rápido - Mannequin Studio

## Instalación en Ubuntu/Debian

### 1. Instalar dependencias del sistema

```bash
sudo apt update
sudo apt install -y \
    cmake \
    build-essential \
    git \
    libvulkan-dev \
    vulkan-validationlayers-dev \
    spirv-tools \
    libglm-dev \
    libeigen3-dev \
    libassimp-dev \
    libglfw3-dev \
    libpython3-dev \
    python3-pip \
    libgtk-3-dev
```

### 2. Clonar el repositorio

```bash
cd /workspace
git clone <repository-url> mannequin-app
cd mannequin-app
```

### 3. Compilar

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### 4. Ejecutar

```bash
./mannequin_app
```

## Instalación en Windows

### 1. Instalar Visual Studio 2022
- Con workload "Desarrollo para el escritorio con C++"

### 2. Instalar vcpkg

```powershell
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg integrate install
```

### 3. Instalar dependencias

```powershell
.\vcpkg install vulkan glm eigen3 assimp glfw3 --triplet x64-windows
```

### 4. Compilar con CMake

```powershell
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

## Primeros Pasos

### Interfaz Principal

```
┌────────────────────────────────────────────────────────────┐
│  Menu Bar  File  Edit  View  Animation  Export  Help      │
├──────────┬───────────────────────────────────┬────────────┤
│          │                                   │            │
│ Property │                                   │  Timeline  │
│ Panel    │        3D VIEWPORT                │  Controls  │
│          │                                   │            │
│ • Height │     [Personaje 3D Wireframe]      │  [Play]    │
│ • Weight │                                   │  [Stop]    │
│ • Muscle │                                   │            │
│ • Chest  │                                   │  Keyframes │
│ • Hips   │                                   │  ● ●   ●   │
│          │                                   │            │
├──────────┴───────────────────────────────────┴────────────┤
│  Timeline ████████████●━━━━━━━━━━━━━━━  0:05 / 0:30       │
└────────────────────────────────────────────────────────────┘
```

### Crear tu Primer Personaje

1. **Ajustar Proporciones Básicas**
   - En el panel izquierdo, usa los sliders:
     - Height: 1.75m (altura promedio)
     - Weight: Ajusta según contextura
     - Muscle Mass: 0.3-0.7 para definición

2. **Refinar Anatomía**
   - Chest Size: Volumen del torso
   - Hip Size: Ancho de cadera
   - Waist Size: Cintura
   - Arm/Leg Length: Proporción de extremidades

3. **Seleccionar Modo de Vista**
   - Wireframe: Ver topología (como tu imagen de referencia)
   - Shaded: Vista con iluminación
   - X-Ray: Ver through el modelo

4. **Animar**
   - Click en timeline para añadir keyframes
   - O importa animación BVH: `File → Import → Motion Capture`
   - Preview con Space (play/pause)

5. **Exportar**
   - `Export → Model` para .glb/.fbx
   - `Export → Video` para .mp4
   - Configura resolución y FPS

## Atajos de Teclado

| Tecla | Acción |
|-------|--------|
| `Espacio` | Play/Pause animación |
| `Ctrl+R` | Iniciar/Detener grabación video |
| `ESC` | Salir |
| `W` | Modo Wireframe |
| `S` | Modo Shaded |
| `X` | Modo X-Ray |
| `F` | Frame selected bone |
| `Del` | Borrar keyframe seleccionado |
| `Ctrl+S` | Guardar proyecto |
| `Ctrl+Z` | Deshacer |
| `Ctrl+Y` | Rehacer |

## Navegación en Viewport

| Acción | Control |
|--------|---------|
| Rotar cámara | Click izquierdo + arrastrar |
| Zoom | Rueda del mouse |
| Pan | Click derecho + arrastrar |
| Orbit around selection | Alt + Click izquierdo |
| Reset camera | Home |

## Exportación de Video

### Configuración Recomendada

```
Formato: MP4 (H.264)
Resolución: 1920x1080
FPS: 30 o 60
Bitrate: 10-20 Mbps
Duración: Según timeline
```

### Pasos:

1. Posiciona cámara en ángulo deseado
2. Selecciona rango en timeline (start/end)
3. `Export → Video`
4. Configura settings
5. Click en "Start Render"
6. Espera a que complete (barra de progreso)

## Scripting con Python

### Ejemplo Básico

```python
from mannequin import MannequinAPI

# Inicializar
api = MannequinAPI()

# Crear personaje
api.create_character(height=1.80, weight=75, muscle_mass=0.6)

# Cargar animación
api.load_animation("walk.bvh")

# Configurar vista wireframe
api.set_view_mode("wireframe")

# Exportar video
api.export_video(
    "output.mp4",
    fps=30,
    duration=5.0,
    width=1920,
    height=1080
)
```

### Automatización de Renders

```python
# render_batch.py
import os
from mannequin import MannequinAPI

characters = [
    {"height": 1.60, "weight": 55},
    {"height": 1.75, "weight": 70},
    {"height": 1.90, "weight": 85},
]

api = MannequinAPI()
api.load_animation("run_cycle.bvh")

for i, char in enumerate(characters):
    api.create_character(**char)
    api.export_video(f"render_{i}.mp4", duration=3.0)
    
print("Batch render complete!")
```

## Solución de Problemas

### Vulkan no detectado

```bash
# Verificar instalación
vulkaninfo | head

# Si falla, reinstalar drivers
sudo apt install mesa-vulkan-drivers
```

### Error de compilación con GLM

```bash
# Asegurar versión correcta
sudo apt install libglm-dev
# O compilar desde source si se necesita versión específica
```

### Bajo rendimiento

- Reducir LOD en Export Settings
- Desactivar sombras en tiempo real
- Usar viewport resolution más baja
- Cerrar otras aplicaciones GPU-intensive

### Crash al exportar video

- Verificar espacio en disco
- Check codecs instalados: `ffmpeg -codecs`
- Probar formato alternativo (PNG sequence)

## Recursos Adicionales

- **Documentación completa**: `/docs/ARCHITECTURE.md`
- **API Python**: `/scripts/mannequin_api.py`
- **Ejemplos**: `/examples/`
- **Reportar bugs**: GitHub Issues

## Soporte

Para ayuda técnica:
- Email: support@mannequin.studio
- Discord: discord.gg/mannequin-studio
- Forum: forum.mannequin.studio

---

**Próximo paso**: Lee el tutorial interactivo en `Help → Tutorial` dentro de la aplicación.
