#include "core/mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <fstream>
#include <iostream>

namespace mannequin {

Mesh::Mesh() {}

Mesh::~Mesh() {}

bool Mesh::loadFromFile(const std::string& filepath) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filepath,
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_PreTransformVertices);
    
    if (!scene || !scene->HasMeshes()) {
        std::cerr << "Failed to load mesh: " << filepath << std::endl;
        return false;
    }
    
    return importFromAssimp(scene, 0);
}

bool Mesh::importFromAssimp(const aiScene* scene, unsigned int meshIndex) {
    if (meshIndex >= scene->mNumMeshes) {
        return false;
    }
    
    aiMesh* assimpMesh = scene->mMeshes[meshIndex];
    
    vertices_.clear();
    indices_.clear();
    
    // Load vertices
    for (unsigned int i = 0; i < assimpMesh->mNumVertices; ++i) {
        Vertex vertex;
        
        // Position
        vertex.position = Eigen::Vector3f(
            assimpMesh->mVertices[i].x,
            assimpMesh->mVertices[i].y,
            assimpMesh->mVertices[i].z
        );
        
        // Normal
        if (assimpMesh->HasNormals()) {
            vertex.normal = Eigen::Vector3f(
                assimpMesh->mNormals[i].x,
                assimpMesh->mNormals[i].y,
                assimpMesh->mNormals[i].z
            );
        }
        
        // Texture coordinates
        if (assimpMesh->HasTextureCoords(0)) {
            vertex.texCoord = Eigen::Vector2f(
                assimpMesh->mTextureCoords[0][i].x,
                assimpMesh->mTextureCoords[0][i].y
            );
        }
        
        // Tangent
        if (assimpMesh->HasTangentsAndBitangents()) {
            vertex.tangent = Eigen::Vector4f(
                assimpMesh->mTangents[i].x,
                assimpMesh->mTangents[i].y,
                assimpMesh->mTangents[i].z,
                assimpMesh->mBitangents[i].x
            );
        }
        
        // Bone weights (if available)
        for (unsigned int j = 0; j < 4; ++j) {
            vertex.boneIDs[j] = 0.0f;
            vertex.boneWeights[j] = 0.0f;
        }
        
        vertices_.push_back(vertex);
    }
    
    // Load indices
    for (unsigned int i = 0; i < assimpMesh->mNumFaces; ++i) {
        aiFace face = assimpMesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j) {
            indices_.push_back(face.mIndices[j]);
        }
    }
    
    needsUpdate_ = true;
    return true;
}

void Mesh::applyBlendShapes() {
    if (blendShapes_.empty()) {
        return;
    }
    
    // Store original positions
    std::vector<Eigen::Vector3f> originalPositions(vertices_.size());
    std::vector<Eigen::Vector3f> originalNormals(vertices_.size());
    
    for (size_t i = 0; i < vertices_.size(); ++i) {
        originalPositions[i] = vertices_[i].position;
        originalNormals[i] = vertices_[i].normal;
    }
    
    // Apply each blend shape
    for (const auto& shape : blendShapes_) {
        if (shape.weight == 0.0f) continue;
        
        size_t count = std::min(shape.positionDeltas.size(), vertices_.size());
        for (size_t i = 0; i < count; ++i) {
            vertices_[i].position += shape.positionDeltas[i] * shape.weight;
            if (i < shape.normalDeltas.size()) {
                vertices_[i].normal += shape.normalDeltas[i] * shape.weight;
            }
        }
    }
    
    needsUpdate_ = true;
}

void Mesh::skinMesh(const std::vector<Bone>& skeleton) {
    // This would be implemented with GPU skinning in shaders
    // For now, we'll do CPU skinning as a fallback
    
    for (auto& vertex : vertices_) {
        Eigen::Vector3f tempPos(0, 0, 0);
        Eigen::Vector3f tempNormal(0, 0, 0);
        
        for (int i = 0; i < 4; ++i) {
            float weight = vertex.boneWeights[i];
            if (weight <= 0.0f) continue;
            
            int boneID = static_cast<int>(vertex.boneIDs[i]);
            if (boneID < 0 || boneID >= static_cast<int>(skeleton.size())) continue;
            
            const auto& bone = skeleton[boneID];
            Eigen::Affine3f transform = bone.globalTransform * bone.inverseBindMatrix;
            
            tempPos += weight * (transform * vertex.position);
            tempNormal += weight * (transform.linear() * vertex.normal);
        }
        
        vertex.position = tempPos;
        vertex.normal = tempNormal.normalized();
    }
    
    needsUpdate_ = true;
}

void Mesh::addBlendShape(const BlendShape& shape) {
    blendShapes_.push_back(shape);
    needsUpdate_ = true;
}

void Mesh::updateVertexBuffer() {
    // This would update the GPU vertex buffer
    // Implementation depends on the rendering backend (Vulkan/OpenGL)
    needsUpdate_ = false;
}

// Skeleton implementation
Skeleton::Skeleton() {}

int Skeleton::addBone(const std::string& name, int parentID) {
    Bone bone;
    bone.id = static_cast<int>(bones_.size());
    bone.name = name;
    bone.parentID = parentID;
    bone.localTransform = Eigen::Affine3f::Identity();
    bone.globalTransform = Eigen::Affine3f::Identity();
    bone.inverseBindMatrix = Eigen::Affine3f::Identity();
    
    bones_.push_back(bone);
    boneMap_[name] = bone.id;
    
    return bone.id;
}

Bone& Skeleton::getBone(int id) {
    return bones_[id];
}

const Bone& Skeleton::getBone(int id) const {
    return bones_[id];
}

Bone* Skeleton::findBone(const std::string& name) {
    auto it = boneMap_.find(name);
    if (it != boneMap_.end()) {
        return &bones_[it->second];
    }
    return nullptr;
}

void Skeleton::updateBoneTransform(int boneID, const Eigen::Affine3f& transform) {
    if (boneID >= 0 && boneID < static_cast<int>(bones_.size())) {
        bones_[boneID].localTransform = transform;
    }
}

void Skeleton::calculateGlobalTransforms() {
    for (auto& bone : bones_) {
        if (bone.parentID == -1) {
            bone.globalTransform = bone.localTransform;
        } else {
            bone.globalTransform = bones_[bone.parentID].globalTransform * bone.localTransform;
        }
    }
}

bool Skeleton::loadFromBVH(const std::string& filepath) {
    // BVH loading implementation
    // This is a simplified version - full implementation would parse BVH format
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Parse BVH file (simplified)
    std::string line;
    while (std::getline(file, line)) {
        // Parse hierarchy and motion data
        // Full implementation would be more comprehensive
    }
    
    return true;
}

// AnimationClip implementation
AnimationClip::AnimationClip(const std::string& name) : name_(name) {}

void AnimationClip::addKeyframe(int boneID, const Keyframe& keyframe) {
    boneKeyframes_[boneID].push_back(keyframe);
    duration_ = std::max(duration_, keyframe.time);
}

void AnimationClip::sample(float time, std::vector<Eigen::Affine3f>& boneTransforms) {
    // Interpolate between keyframes
    for (const auto& [boneID, keyframes] : boneKeyframes_) {
        if (keyframes.empty()) continue;
        
        // Find surrounding keyframes
        Keyframe kf1 = keyframes[0];
        Keyframe kf2 = keyframes.back();
        
        for (size_t i = 0; i < keyframes.size() - 1; ++i) {
            if (time >= keyframes[i].time && time <= keyframes[i + 1].time) {
                kf1 = keyframes[i];
                kf2 = keyframes[i + 1];
                break;
            }
        }
        
        // Interpolate
        float t = 0.0f;
        if (kf2.time > kf1.time) {
            t = (time - kf1.time) / (kf2.time - kf1.time);
        }
        
        Eigen::Vector3f pos = kf1.position * (1 - t) + kf2.position * t;
        Eigen::Quaternionf rot = kf1.rotation.slerp(t, kf2.rotation);
        Eigen::Vector3f scale = kf1.scale * (1 - t) + kf2.scale * t;
        
        Eigen::Affine3f transform = Eigen::Affine3f::Identity();
        transform.translate(pos);
        transform.rotate(rot);
        transform.scale(scale);
        
        if (boneID < static_cast<int>(boneTransforms.size())) {
            boneTransforms[boneID] = transform;
        }
    }
}

bool AnimationClip::loadFromBVH(const std::string& filepath) {
    // Load animation data from BVH file
    // Similar to Skeleton::loadFromBVH but extracts motion data
    return true;
}

} // namespace mannequin
