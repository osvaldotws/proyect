# Mannequin Studio - Windows Dependency Installer
# Requiere PowerShell ejecutado como Administrador para algunas instalaciones

param(
    [switch]$SkipVcpkg,
    [switch]$NoBuild
)

$ErrorActionPreference = "Stop"
$INSTALL_DIR = "$PSScriptRoot\..\build\windows_deps"
$VCPKG_ROOT = $env:VCPKG_ROOT

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Mannequin Studio: Windows Installer  " -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan

# 1. Verificar Prerrequisitos
Write-Host "`n[1/6] Verificando prerrequisitos..." -ForegroundColor Yellow

# Verificar Git
if (!(Get-Command git -ErrorAction SilentlyContinue)) {
    Write-Error "Git no encontrado. Por favor instala Git desde https://git-scm.com/download/win"
    exit 1
}
Write-Host "  [OK] Git instalado" -ForegroundColor Green

# Verificar Visual Studio Build Tools (MSVC)
$msvcFound = $false
if (Test-Path "C:\Program Files (x86)\Microsoft Visual Studio\Installer\vs_where.exe") {
    $instances = & "C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe" -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationId
    if ($instances) { $msvcFound = $true }
}
if (!$msvcFound) {
    Write-Warning "Visual Studio C++ Tools no detectado completamente."
    Write-Host "  -> Asegúrate de tener 'Desarrollo para el escritorio con C++' instalado en Visual Studio." -ForegroundColor Yellow
} else {
    Write-Host "  [OK] MSVC Compiler detectado" -ForegroundColor Green
}

# 2. Gestión de Paquetes (vcpkg vs Manual)
if (!$SkipVcpkg) {
    if ($VCPKG_ROOT -and (Test-Path "$VCPKG_ROOT\vcpkg.exe")) {
        Write-Host "`n[2/6] Usando vcpkg existente en: $VCPKG_ROOT" -ForegroundColor Yellow
        $USE_VCPKG = $true
    } else {
        Write-Host "`n[2/6] vcpkg no encontrado. ¿Deseas instalarlo automáticamente? (Y/N)" -ForegroundColor Yellow
        $response = Read-Host
        if ($response -eq 'Y' -or $response -eq 'y') {
            Write-Host "  Clonando vcpkg..."
            git clone https://github.com/microsoft/vcpkg.git "$INSTALL_DIR\vcpkg"
            & "$INSTALL_DIR\vcpkg\bootstrap-vcpkg.bat"
            $env:VCPKG_ROOT = "$INSTALL_DIR\vcpkg"
            [Environment]::SetEnvironmentVariable("VCPKG_ROOT", $env:VCPKG_ROOT, "User")
            $USE_VCPKG = $true
            Write-Host "  [OK] vcpkg instalado y configurado" -ForegroundColor Green
        } else {
            Write-Host "  Saltando vcpkg. Se intentará usar winget o descargas manuales." -ForegroundColor Yellow
            $USE_VCPKG = $false
        }
    }
} else {
    $USE_VCPKG = $false
}

# 3. Instalación de Dependencias C++
Write-Host "`n[3/6] Instalando dependencias C++ (Vulkan, GLFW, GLM, Eigen, Assimp)..." -ForegroundColor Yellow

if ($USE_VCPKG) {
    Write-Host "  Instalando paquetes vía vcpkg (puede tardar varios minutos)..."
    & vcpkg install vulkan glfw3 glm eigen3 assimp --triplet x64-windows
    if ($LASTEXITCODE -ne 0) { throw "Error instalando paquetes vcpkg" }
    Write-Host "  [OK] Dependencias C++ instaladas" -ForegroundColor Green
} else {
    # Fallback a Winget si está disponible
    if (Get-Command winget -ErrorAction SilentlyContinue) {
        Write-Host "  Intentando instalar vía winget..."
        winget install Kitware.CMake --silent
        winget install Kitware.VisualStudio.CMake.Tools --silent
        # Nota: Las librerías específicas de C++ son difíciles sin vcpkg en Windows.
        Write-Warning "Sin vcpkg, asegúrate de tener las librerías en tu PATH o carpeta local."
    } else {
        Write-Error "Se requiere vcpkg o winget para instalar dependencias en Windows automáticamente."
        exit 1
    }
}

# 4. Instalación de FFmpeg (Para exportación de video)
Write-Host "`n[4/6] Configurando FFmpeg para exportación de video..." -ForegroundColor Yellow
if (!(Get-Command ffmpeg -ErrorAction SilentlyContinue)) {
    Write-Host "  FFmpeg no encontrado en PATH. Descargando versión portable..."
    $ffmpegUrl = "https://www.gyan.dev/ffmpeg/builds/ffmpeg-release-essentials.zip"
    $zipPath = "$INSTALL_DIR\ffmpeg.zip"
    New-Item -ItemType Directory -Force -Path $INSTALL_DIR | Out-Null
    
    # Descargar (simulado, en real usaría Invoke-WebRequest)
    try {
        Invoke-WebRequest -Uri $ffmpegUrl -OutFile $zipPath
        Expand-Archive -Path $zipPath -DestinationPath "$INSTALL_DIR\ffmpeg" -Force
        $ffmpegBin = Get-ChildItem -Path "$INSTALL_DIR\ffmpeg" -Directory | Where-Object {$_.Name -like "*ffmpeg*"} | Select-Object -First 1
        $ffmpegPath = "$($ffmpegBin.FullName)\bin"
        
        # Añadir al PATH de la sesión actual
        $env:Path = "$ffmpegPath;" + $env:Path
        Write-Host "  [OK] FFmpeg descargado y añadido al PATH temporal" -ForegroundColor Green
        Write-Host "  -> Para hacerlo permanente, añade '$ffmpegPath' a las Variables de Entorno del Sistema." -ForegroundColor Cyan
    } catch {
        Write-Warning "No se pudo descargar FFmpeg automáticamente. Descárgalo manualmente de https://ffmpeg.org/download.html"
    }
} else {
    Write-Host "  [OK] FFmpeg ya está disponible" -ForegroundColor Green
}

# 5. Entorno Python
Write-Host "`n[5/6] Configurando entorno Python..." -ForegroundColor Yellow
if (!(Get-Command python -ErrorAction SilentlyContinue)) {
    Write-Error "Python no encontrado. Instala Python desde https://www.python.org/downloads/"
    exit 1
}
Write-Host "  Creando entorno virtual en 'venv'..."
python -m venv "$PSScriptRoot\..\venv"
& "$PSScriptRoot\..\venv\Scripts\Activate.ps1"
pip install --upgrade pip
pip install numpy PyQt5
Write-Host "  [OK] Entorno Python listo" -ForegroundColor Green

# 6. Compilación (Opcional)
if (!$NoBuild) {
    Write-Host "`n[6/6] Preparando compilación..." -ForegroundColor Yellow
    
    $BUILD_DIR = "$PSScriptRoot\..\build\win_build"
    New-Item -ItemType Directory -Force -Path $BUILD_DIR | Out-Null
    
    Write-Host "  Ejecutando CMake..."
    Set-Location $BUILD_DIR
    
    $CMAKE_ARGS = @()
    if ($USE_VCPKG) {
        $CMAKE_ARGS += "-DCMAKE_TOOLCHAIN_FILE=$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
    }
    $CMAKE_ARGS += "-G", "Visual Studio 17 2022", "-A", "x64", "..\.."
    
    # Intentar generar solución VS
    try {
        cmake @CMAKE_ARGS
        Write-Host "  [OK] Solución de Visual Studio generada en $BUILD_DIR" -ForegroundColor Green
        Write-Host "  -> Abre 'MannequinStudio.sln' en Visual Studio para compilar." -ForegroundColor Cyan
    } catch {
        Write-Warning "CMake falló. Asegúrate de tener Visual Studio 2022 instalado."
    }
}

Write-Host "`n========================================" -ForegroundColor Green
Write-Host "  ¡Instalación Completada!             " -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host "`nSiguientes pasos:"
Write-Host "1. Si usaste vcpkg, reinicia tu terminal para aplicar variables."
Write-Host "2. Abre Visual Studio y carga el archivo .sln en build/win_build/"
Write-Host "3. Compila la solución en modo Release."
Write-Host "4. Ejecuta el binario generado."
