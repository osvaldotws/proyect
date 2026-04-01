// Fragment Shader con soporte para MatCap, Wireframe y X-Ray
#version 450

layout(location = 0) in vec3 fs_inNormal;
layout(location = 1) in vec3 fs_inPosition;
layout(location = 2) in vec2 fs_inTexCoord;
layout(location = 3) in flat int fs_instanceID;

layout(location = 0) out vec4 outColor;

// Uniform Buffer Objects
layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    vec4 clipPlane;
    int useClipPlane;
} ubo;

layout(binding = 2) uniform MaterialBufferObject {
    vec4 baseColor;
    float metallic;
    float roughness;
    int renderMode; // 0=shaded, 1=wireframe, 2=xray
    vec3 wireframeColor;
    float wireframeThickness;
    vec3 xRayColor;
    float xRayOpacity;
} material;

// Textures
layout(binding = 0) uniform sampler2D diffuseTexture;
layout(binding = 1) uniform sampler2D matCapTexture;
layout(binding = 3) uniform sampler2D normalMap;

void main() {
    vec3 normal = normalize(fs_inNormal);
    vec3 viewDir = normalize(-fs_inPosition);
    
    vec4 color = material.baseColor;
    
    if(material.renderMode == 0) {
        // === SHADED MODE con MatCap ===
        
        // Calcular coordenadas para MatCap
        vec3 reflectDir = reflect(-viewDir, normal);
        vec2 matcapUV = reflectDir.xy * 0.5 + 0.5;
        
        // Sample MatCap
        vec3 matcapColor = texture(matCapTexture, matcapUV).rgb;
        
        // Aplicar iluminación ambiental suave
        vec3 ambient = vec3(0.1);
        color.rgb = matcapColor + ambient;
        
        // Alpha de base
        color.a = 1.0;
        
    } else if(material.renderMode == 1) {
        // === WIREFRAME MODE ===
        
        // Usar barycentric coordinates para dibujar wireframe
        // Esto se calcularía en geometry shader, aquí simplificamos
        
        // Color base oscuro
        color.rgb = vec3(0.1, 0.1, 0.15);
        
        // Patrones de wireframe basados en UV
        float gridX = abs(fract(fs_inTexCoord.x * 50.0) - 0.5);
        float gridY = abs(fract(fs_inTexCoord.y * 50.0) - 0.5);
        
        float lineX = smoothstep(0.5 - material.wireframeThickness * 0.01, 
                                 0.5, gridX);
        float lineY = smoothstep(0.5 - material.wireframeThickness * 0.01, 
                                 0.5, gridY);
        
        float wireframe = max(lineX, lineY);
        color.rgb = mix(color.rgb, material.wireframeColor, wireframe);
        color.a = 1.0;
        
    } else if(material.renderMode == 2) {
        // === X-RAY MODE ===
        
        // Color semi-transparente
        color.rgb = material.xRayColor;
        color.a = material.xRayOpacity;
        
        // Fresnel effect para bordes brillantes
        float fresnel = pow(1.0 - abs(dot(viewDir, normal)), 3.0);
        color.rgb += fresnel * vec3(0.3, 0.6, 1.0);
        
        // Patrón de rejilla sutil
        float grid = sin(fs_inPosition.x * 10.0) * sin(fs_inPosition.y * 10.0);
        color.rgb *= 0.8 + 0.2 * grid;
    }
    
    // Aplicar textura difusa si existe (modo shaded)
    if(material.renderMode == 0 && fs_inTexCoord.x > 0.0 && fs_inTexCoord.y > 0.0) {
        vec4 texColor = texture(diffuseTexture, fs_inTexCoord);
        if(texColor.a > 0.01) {
            color.rgb *= texColor.rgb;
        }
    }
    
    // Gamma correction
    color.rgb = pow(color.rgb, vec3(1.0/2.2));
    
    outColor = color;
}
