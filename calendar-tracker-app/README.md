# Calendar Tracker App

Aplicación multiplataforma (Android y Windows) para marcar días en un calendario y gestionar tareas.

## Características

### Página 1: Calendario
- Visualización mensual del calendario
- Haz clic o toca un día para marcarlo en verde
- Los días marcados se agregan automáticamente a la lista
- Navegación entre meses
- Contador de días marcados

### Página 2: Lista de Días y Tareas
- Lista de todos los días marcados
- Agrega tareas específicas para cada día
- Marca tareas como completadas
- Elimina tareas
- Vista expandida/contraída para cada día
- Estadísticas de tareas completadas

## Tecnologías

- **Kotlin Multiplatform**: Código compartido entre plataformas
- **Jetpack Compose**: UI moderna y declarativa
- **Compose Desktop**: Para aplicación de escritorio (Windows)
- **Compose Android**: Para aplicación móvil

## Estructura del Proyecto

```
calendar-tracker-app/
├── shared/              # Código común (lógica y UI)
│   └── src/commonMain/
│       └── kotlin/
│           └── com/example/calendartracker/
│               ├── DataModels.kt       # Modelos de datos
│               ├── CalendarViewModel.kt # Lógica de negocio
│               ├── CalendarScreen.kt   # Pantalla del calendario
│               ├── TasksScreen.kt      # Pantalla de tareas
│               └── App.kt              # App principal
├── androidApp/          # Aplicación Android
│   └── src/main/
│       ├── AndroidManifest.xml
│       └── java/com/example/calendartracker/
│           └── MainActivity.kt
├── desktopApp/          # Aplicación de Escritorio
│   └── src/jvmMain/
│       └── kotlin/com/example/calendartracker/
│           └── Main.kt
└── build.gradle.kts     # Configuración principal
```

## Requisitos

- Android Studio Arctic Fox o superior
- JDK 17
- Android SDK 24+
- Para Desktop: IntelliJ IDEA con soporte para Compose

## Cómo Construir

### Android
```bash
./gradlew :androidApp:assembleDebug
```

### Desktop (Windows/Mac/Linux)
```bash
./gradlew :desktopApp:run
```

Para crear distributable:
```bash
./gradlew :desktopApp:packageDistributionForCurrentOS
```

## Uso

1. **Marcar días**: En la pantalla del calendario, toca/clic en cualquier día para marcarlo en verde
2. **Ver lista**: Presiona el botón "Ver Lista de Días" para ir a la segunda página
3. **Agregar tareas**: Expande un día y escribe una nueva tarea en el campo de texto
4. **Completar tareas**: Toca el checkbox para marcar una tarea como completada
5. **Eliminar tareas**: Presiona el ícono de basura para eliminar una tarea
6. **Navegar**: Usa las flechas para cambiar de mes o el botón/ícono para volver al calendario

## Licencia

MIT License
