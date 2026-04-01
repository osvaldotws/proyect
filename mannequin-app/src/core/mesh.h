#pragma once

#include <vector>
#include <string>
#include <memory>
#include <Eigen/Dense>
#include <assimp/scene.h>

namespace mannequin {

// Vertex structure for mesh data
struct Vertex {
    Eigen::Vector3f position;
    Eigen::Vector3f normal;
    Eigen::Vector2f texCoord;
    Eigen::Vector4f tangent;
    std::array<float, 4> boneIDs;
    std::array<float, 4> boneWeights;
};

// Bone structure for skeleton
struct Bone {
    std::string name;
    int id;
    int parentID;
    Eigen::Affine3f inverseBindMatrix;
    Eigen::Affine3f localTransform;
    Eigen::Affine3f globalTransform;
};

// Keyframe for animation
struct Keyframe {
    float time;
    Eigen::Vector3f position;
    Eigen::Quaternionf rotation;
    Eigen::Vector3f scale;
};

// Blend Shape for morph targets
struct BlendShape {
    std::string name;
    std::vector<Eigen::Vector3f> positionDeltas;
    std::vector<Eigen::Vector3f> normalDeltas;
    float weight = 0.0f;
};

// Main Mesh class
class Mesh {
public:
    Mesh();
    ~Mesh();

    // Load mesh from file
    bool loadFromFile(const std::string& filepath);
    
    // Import from Assimp scene
    bool importFromAssimp(const aiScene* scene, unsigned int meshIndex);
    
    // Apply blend shapes
    void applyBlendShapes();
    
    // Skin mesh to skeleton
    void skinMesh(const std::vector<Bone>& skeleton);
    
    // Get vertices
    const std::vector<Vertex>& getVertices() const { return vertices_; }
    std::vector<Vertex>& getVertices() { return vertices_; }
    
    // Get indices
    const std::vector<unsigned int>& getIndices() const { return indices_; }
    
    // Add blend shape
    void addBlendShape(const BlendShape& shape);
    
    // Update vertex buffer (for GPU)
    void updateVertexBuffer();

private:
    std::vector<Vertex> vertices_;
    std::vector<unsigned int> indices_;
    std::vector<BlendShape> blendShapes_;
    std::string name_;
    bool needsUpdate_ = true;
};

// Skeleton class
class Skeleton {
public:
    Skeleton();
    
    // Add bone
    int addBone(const std::string& name, int parentID = -1);
    
    // Get bone by ID
    Bone& getBone(int id);
    const Bone& getBone(int id) const;
    
    // Get bone by name
    Bone* findBone(const std::string& name);
    
    // Update bone transforms
    void updateBoneTransform(int boneID, const Eigen::Affine3f& transform);
    
    // Calculate global transforms
    void calculateGlobalTransforms();
    
    // Load from BVH file
    bool loadFromBVH(const std::string& filepath);
    
    // Get all bones
    const std::vector<Bone>& getBones() const { return bones_; }
    std::vector<Bone>& getBones() { return bones_; }

private:
    std::vector<Bone> bones_;
    std::map<std::string, int> boneMap_;
};

// Animation Clip
class AnimationClip {
public:
    AnimationClip(const std::string& name = "");
    
    // Add keyframe to bone
    void addKeyframe(int boneID, const Keyframe& keyframe);
    
    // Sample animation at time
    void sample(float time, std::vector<Eigen::Affine3f>& boneTransforms);
    
    // Get duration
    float getDuration() const { return duration_; }
    void setDuration(float duration) { duration_ = duration; }
    
    // Load from BVH
    bool loadFromBVH(const std::string& filepath);

private:
    std::string name_;
    float duration_ = 0.0f;
    float fps_ = 30.0f;
    std::map<int, std::vector<Keyframe>> boneKeyframes_;
};

} // namespace mannequin
