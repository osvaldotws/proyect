# Guía de Instalación - Mannequin Studio

## Resumen Rápido

| Plataforma | Script | Requiere Admin |
|------------|--------|----------------|
| **Ubuntu/Debian** | `./scripts/install_dependencies.sh` | ✅ (sudo) |
| **Fedora/RHEL** | `./scripts/install_dependencies.sh` | ✅ (sudo) |
| **Arch/Manjaro** | `./scripts/install_dependencies.sh` | ✅ (sudo) |
| **macOS** | `./scripts/install_dependencies.sh` | ❌ (solo Homebrew) |
| **Windows** | `.\scripts\install_dependencies.ps1` | ✅ (Admin) |

---

## Windows - Instalación Detallada

### Paso 1: Prerrequisitos

Antes de ejecutar el script, asegúrate de tener:

1. **Visual Studio 2022** (Community, Professional o Enterprise)
   - Descarga: https://visualstudio.microsoft.com/downloads/
   - Durante la instalación, marca: **"Desarrollo para el escritorio con C++"**
   
2. **Git para Windows**
   - Descarga: https://git-scm.com/download/win

3. **Python 3.8+** (opcional, el script puede instalarlo)
   - Descarga: https://www.python.org/downloads/

### Paso 2: Clonar Repositorio

```powershell
git clone https://github.com/tu-usuario/mannequin-studio.git
cd mannequin-studio
```

### Paso 3: Ejecutar Script de Instalación

```powershell
# Abrir PowerShell como Administrador (Click derecho -> Ejecutar como administrador)

# Permitir ejecución de scripts en esta sesión
Set-ExecutionPolicy -Scope Process -Unrestricted

# Ejecutar instalador
.\scripts\install_dependencies.ps1
```

### Opciones del Script Windows

```powershell
# Solo instalar dependencias, sin compilar
.\scripts\install_dependencies.ps1 -NoBuild

# Si ya tienes vcpkg configurado, saltar su instalación
.\scripts\install_dependencies.ps1 -SkipVcpkg

# Ambas opciones
.\scripts\install_dependencies.ps1 -SkipVcpkg -NoBuild
```

### ¿Qué hace el script?

1. **Verifica prerrequisitos**: Git, MSVC Compiler
2. **Gestiona vcpkg**: 
   - Si no existe, ofrece instalarlo automáticamente
   - vcpkg es un gestor de paquetes C++ de Microsoft
3. **Instala librerías C++** vía vcpkg:
   - Vulkan SDK
   - GLFW3 (ventanas)
   - GLM (matemáticas)
   - Eigen (álgebra lineal)
   - Assimp (import/export 3D)
4. **Descarga FFmpeg** para exportación de video
5. **Crea entorno Python** virtual con dependencias
6. **Genera solución Visual Studio** (.sln) lista para compilar

### Paso 4: Compilar en Visual Studio

Después de ejecutar el script:

1. Abre Visual Studio 2022
2. Archivo -> Abrir -> Proyecto/Solución
3. Navega a: `build/win_build/MannequinStudio.sln`
4. Selecciona configuración: **Release** | **x64**
5. Compilar -> Compilar solución (Ctrl+Shift+B)
6. El ejecutable estará en: `build/win_build/Release/mannequin-studio.exe`

### Solución de Problemas Windows

#### Error: "vcpkg no se pudo instalar"
- Verifica conexión a internet
- Ejecuta manualmente:
  ```powershell
  git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
  C:\vcpkg\bootstrap-vcpkg.bat
  [Environment]::SetEnvironmentVariable("VCPKG_ROOT", "C:\vcpkg", "User")
  ```

#### Error: "MSVC no encontrado"
- Reinstala Visual Studio asegurándote de marcar "Desarrollo para el escritorio con C++"
- Verifica en: `C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe`

#### Error: "CMake no se encuentra"
- El script debería instalarlo vía vcpkg
- Alternativa: descargar de https://cmake.org/download/

---

## Linux/macOS - Instalación Detallada

### Paso 1: Clonar Repositorio

```bash
git clone https://github.com/tu-usuario/mannequin-studio.git
cd mannequin-studio
```

### Paso 2: Ejecutar Script

```bash
# Dar permisos de ejecución
chmod +x scripts/install_dependencies.sh

# Ejecutar instalador
./scripts/install_dependencies.sh

# Opción: solo dependencias, sin compilar
./scripts/install_dependencies.sh --skip-build
```

### ¿Qué hace el script?

1. **Detecta tu distribución** (Ubuntu, Fedora, Arch, macOS)
2. **Instala dependencias nativas**:
   - Ubuntu/Debian: `apt-get install ...`
   - Fedora: `dnf install ...`
   - Arch: `pacman -S ...`
   - macOS: `brew install ...`
3. **Configura entorno Python** virtual
4. **Verifica Vulkan SDK**
5. **Compila el proyecto** (si no usas --skip-build)

### Compilación Manual (si usaste --skip-build)

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Ejecutar
./mannequin-studio
```

### Solución de Problemas Linux/macOS

#### Error: "Vulkan no encontrado"
- Ubuntu: `sudo apt install libvulkan-dev vulkan-tools`
- Fedora: `sudo dnf install vulkan-loader-devel`
- macOS: `brew install vulkan-loader vulkan-headers`
- Verifica: `vulkaninfo --summary`

#### Error: "GLFW no encontrado"
- Ubuntu: `sudo apt install libglfw3-dev`
- macOS: `brew install glfw`

#### Error de permisos en macOS
```bash
sudo xcode-select --install
sudo xcodebuild -license accept
```

---

## Instalación de Dependencias Individuales

Si prefieres instalar cada componente manualmente:

### Vulkan SDK
- **Windows/macOS/Linux**: https://vulkan.lunarg.com/sdk/home

### CMake
- **Todos**: https://cmake.org/download/
- **Windows (winget)**: `winget install Kitware.CMake`
- **macOS (brew)**: `brew install cmake`

### Python Dependencies
```bash
python -m pip install numpy PyQt5 PyOpenGL
```

### FFmpeg
- **Windows**: https://ffmpeg.org/download.html (añadir bin al PATH)
- **Ubuntu**: `sudo apt install ffmpeg`
- **macOS**: `brew install ffmpeg`

---

## Verificación de Instalación

Después de instalar, verifica que todo esté correcto:

```bash
# Verificar Vulkan
vulkaninfo --summary  # Debe mostrar información de GPU

# Verificar CMake
cmake --version  # Debe mostrar 3.16+

# Verificar Python
python --version  # Debe mostrar 3.8+
pip list  # Debe mostrar numpy, PyQt5

# Verificar FFmpeg
ffmpeg -version  # Debe mostrar versión 4.4+
```

En Windows PowerShell:
```powershell
# Verificar compilador MSVC
cl  # Debe mostrar versión de MSVC

# Verificar vcpkg
vcpkg list  # Debe mostrar paquetes instalados
```

---

## Espacio en Disco Requerido

| Componente | Tamaño |
|------------|--------|
| Código fuente | ~50 MB |
| Dependencias C++ | ~500 MB |
| Vulkan SDK | ~1 GB |
| Entorno Python | ~200 MB |
| Build directory | ~300 MB |
| **Total estimado** | **~2 GB** |

---

## Tiempo Estimado de Instalación

| Escenario | Tiempo |
|-----------|--------|
| Script automático (todo incluido) | 10-20 min |
| Con dependencias ya instaladas | 3-5 min |
| Instalación manual completa | 30-60 min |

---

## Soporte

Si encuentras problemas:

1. Revisa los mensajes de error del script
2. Consulta: `docs/TROUBLESHOOTING.md`
3. Abre un issue en GitHub: https://github.com/tu-usuario/mannequin-studio/issues
4. Incluye: SO, versión, y log completo del error
