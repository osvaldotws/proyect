// Vertex Shader para Skinning y Wireframe
#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec4 inTangent;
layout(location = 4) in vec4 inBoneIDs;
layout(location = 5) in vec4 inBoneWeights;

layout(location = 0) out vec3 fs_inNormal;
layout(location = 1) out vec3 fs_inPosition;
layout(location = 2) out vec2 fs_inTexCoord;
layout(location = 3) out flat int fs_instanceID;

// Uniform Buffer Objects
layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    vec4 clipPlane;
    int useClipPlane;
} ubo;

layout(binding = 1) uniform ModelBufferObject {
    mat4 model;
    mat4 boneTransforms[100];
    vec4 color;
    int boneCount;
} model;

// Wireframe settings
layout(push_constant) uniform PushConstants {
    int renderMode; // 0=shaded, 1=wireframe, 2=xray
    float wireframeThickness;
    vec3 wireframeColor;
} push;

void main() {
    // GPU Skinning
    mat4 skinTransform = mat4(0.0);
    for(int i = 0; i < 4; i++) {
        int boneIndex = int(inBoneIDs[i]);
        if(boneIndex >= 0 && boneIndex < model.boneCount) {
            skinTransform += inBoneWeights[i] * model.boneTransforms[boneIndex];
        }
    }
    
    // Apply skinning
    vec4 skinnedPosition = skinTransform * vec4(inPosition, 1.0);
    vec4 worldPosition = model.model * skinnedPosition;
    
    // Transform normal
    mat3 normalMatrix = transpose(inverse(mat3(model.model * skinTransform)));
    vec3 skinnedNormal = normalize(normalMatrix * inNormal);
    
    // Pass to fragment shader
    fs_inNormal = skinnedNormal;
    fs_inPosition = worldPosition.xyz;
    fs_inTexCoord = inTexCoord;
    
    // Clip plane (for cross-section view)
    if(ubo.useClipPlane == 1) {
        float clipDistance = dot(worldPosition.xyz, ubo.clipPlane.xyz) + ubo.clipPlane.w;
        gl_ClipDistance[0] = clipDistance;
    }
    
    // Final position
    gl_Position = ubo.proj * ubo.view * worldPosition;
    
    // X-Ray mode: disable depth test in fragment shader via layer
    if(push.renderMode == 2) { // X-Ray
        gl_Position.z = gl_Position.z * 0.99; // Move slightly forward
    }
}
