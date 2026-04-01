# Documentación de Arquitectura - Mannequin Studio

## Visión General del Sistema

```
┌─────────────────────────────────────────────────────────────────┐
│                         APLICACIÓN                              │
│  ┌─────────────┐  ┌──────────────┐  ┌─────────────────────┐   │
│  │   UI Layer  │  │ Python API   │  │   Plugin System     │   │
│  │  (ImGui)    │◄─┤              │  │                     │   │
│  └──────┬──────┘  └──────┬───────┘  └─────────────────────┘   │
│         │                │                                     │
│  ┌──────▼────────────────▼───────────────────────────────┐     │
│  │                  ENGINE CORE                           │     │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────────────┐    │     │
│  │  │   Mesh   │  │Skeleton  │  │   Animation      │    │     │
│  │  │  System  │  │  System  │  │   System         │    │     │
│  │  └──────────┘  └──────────┘  └──────────────────┘    │     │
│  └────────────────────────┬──────────────────────────────┘     │
│                           │                                    │
│  ┌────────────────────────▼───────────────────────────────┐     │
│  │              RENDERER (Vulkan)                         │     │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────────────┐    │     │
│  │  │ Pipeline │  │  Shader  │  │  Resource        │    │     │
│  │  │ Manager  │  │ Compiler │  │  Manager         │    │     │
│  │  └──────────┘  └──────────┘  └──────────────────┘    │     │
│  └────────────────────────────────────────────────────────┘     │
└─────────────────────────────────────────────────────────────────┘
```

## Flujo de Datos

### 1. Creación de Personaje

```
Usuario → UI Sliders → BodyParams → BlendShapes → Mesh Deformation → GPU Buffer
```

**Detalle:**
1. Usuario ajusta sliders en PropertyPanel
2. Se actualiza la estructura `BodyParams`
3. Los cambios se traducen a weights de BlendShapes
4. `Mesh::applyBlendShapes()` calcula nuevas posiciones de vértices
5. `Mesh::updateVertexBuffer()` envía datos a GPU
6. Renderer dibuja el frame con la nueva geometría

### 2. Sistema de Animación

```
BVH File / Keyframes → AnimationClip → Skeleton Pose → Skinning → Render
```

**Proceso de Skinnging:**
```cpp
// Para cada vértice:
finalPosition = Σ(weight_i × boneTransform_i × bindPose_i^-1 × vertexPosition)
```

### 3. Pipeline de Renderizado (Vulkan)

```
Vertex Input → Vertex Shader → Tessellation → Geometry → 
Fragment Shader → Color Attachment → Present
```

**Shaders Principales:**

#### Vertex Shader (mesh.vert.glsl)
```glsl
#version 450
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec4 inTangent;
layout(location = 4) in vec4 inBoneIDs;
layout(location = 5) in vec4 inBoneWeights;

layout(push_constant) uniform PushConstants {
    mat4 modelMatrix;
    mat4 boneTransforms[MAX_BONES];
} push;

void main() {
    // Skinning
    mat4 skinTransform = mat4(0.0);
    for(int i = 0; i < 4; i++) {
        skinTransform += inBoneWeights[i] * 
                        push.boneTransforms[int(inBoneIDs[i])];
    }
    
    vec4 skinnedPos = skinTransform * vec4(inPosition, 1.0);
    gl_Position = ubo.proj * ubo.view * push.modelMatrix * skinnedPos;
}
```

#### Fragment Shader (matcap.frag.glsl)
```glsl
#version 450
layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D matCapTexture;

void main() {
    vec3 normal = normalize(fs_in.normal);
    vec3 viewDir = normalize(-fs_in.worldPos);
    vec3 reflectDir = reflect(-viewDir, normal);
    
    vec2 uv = reflectDir.xy * 0.5 + 0.5;
    outColor = texture(matCapTexture, uv);
}
```

## Sistema de Archivos

### Formatos Soportados

| Formato | Uso | Librería |
|---------|-----|----------|
| .obj | Mallas estáticas | Assimp |
| .fbx | Mallas + animación | Assimp |
| .gltf/.glb | Web/Real-time | Assimp |
| .bvh | Motion capture | Custom parser |
| .mp4 | Video export | FFmpeg |

### Estructura de Escena

```json
{
  "scene": {
    "character": {
      "mesh": "base_mesh.glb",
      "blendShapes": [
        {"name": "height", "weight": 0.8},
        {"name": "muscle", "weight": 0.5}
      ],
      "skeleton": "human_rig.bvh",
      "animation": {
        "clip": "walk_cycle.bvh",
        "startTime": 0.0,
        "endTime": 2.5,
        "loop": true
      }
    },
    "camera": {
      "position": [0, 1.5, 3],
      "target": [0, 1, 0],
      "fov": 45
    },
    "lighting": {
      "ambient": [0.1, 0.1, 0.1],
      "directional": {
        "color": [1, 1, 0.9],
        "intensity": 1.0,
        "direction": [0.5, -1, 0.3]
      }
    }
  }
}
```

## Optimizaciones Implementadas

### 1. GPU Skinning
- Los cálculos de deformación se hacen en el vertex shader
- Reduce transferencia CPU→GPU
- Máximo 100 huesos por personaje

### 2. Level of Detail (LOD)
```cpp
enum LODLevel {
    LOD_HIGH = 0,    // ~50,000 tris
    LOD_MEDIUM = 1,  // ~15,000 tris
    LOD_LOW = 2,     // ~5,000 tris
    LOD_PROXY = 3    // ~500 tris
};
```

### 3. Frustum Culling
- Solo se renderiza lo visible en cámara
- Bounding box testing por componente

### 4. Multi-threading
```
Thread Principal: UI, Input, Lógica
Thread Render: Vulkan commands, GPU sync
Thread IO: Asset loading, Export
```

## Sistema de Plugins

### Interfaz Base
```cpp
class IPlugin {
public:
    virtual const char* getName() = 0;
    virtual bool initialize(Engine* engine) = 0;
    virtual void shutdown() = 0;
    virtual void update(float deltaTime) = 0;
};
```

### Ejemplo: Plugin de Ropa
```cpp
class ClothingPlugin : public IPlugin {
    std::vector<ClothSim> simulations_;
    
    void simulatePhysics(float dt) {
        for(auto& cloth : simulations_) {
            cloth.verletIntegration(dt);
            cloth.handleCollisions();
        }
    }
};
```

## Integración con IA (Futuro)

### Generación de Poses
```python
from transformers import pipeline

pose_generator = pipeline("text-to-pose")

def generate_pose_from_text(prompt: str):
    result = pose_generator(prompt)
    # Resultado: rotaciones por hueso
    return result["bone_rotations"]
```

### Auto-Rigging
- Detección automática de topología
- Asignación de pesos por proximidad
- Ajuste fino con gradient descent

## Testing Strategy

### Unit Tests (Catch2)
```cpp
TEST_CASE("Skeleton calculates global transforms correctly") {
    Skeleton skeleton;
    int root = skeleton.addBone("Root");
    int child = skeleton.addBone("Child", root);
    
    skeleton.updateBoneTransform(root, translate(1, 0, 0));
    skeleton.calculateGlobalTransforms();
    
    REQUIRE(skeleton.getBone(child).globalTransform.translation().x() == 1.0f);
}
```

### Integration Tests
- Load character → Animate → Export → Verify file
- UI interaction → State change → Render verification

### Performance Benchmarks
- Frame time < 16ms (60 FPS)
- Memory usage < 2GB para escena típica
- Export time < 5s para video 1080p 30fps 10s

## Roadmap

### Fase 1 (Actual)
- [x] Core engine architecture
- [x] Mesh deformation system
- [x] Basic animation
- [ ] Vulkan renderer complete
- [ ] UI implementation

### Fase 2
- [ ] Video export
- [ ] Python bindings
- [ ] IK solver
- [ ] Physics simulation

### Fase 3
- [ ] Multi-character support
- [ ] Cloud rendering
- [ ] VR/AR support
- [ ] Real-time collaboration
