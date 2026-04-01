#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include <memory>
#include "core/mesh.h"

namespace mannequin {

// Camera for 3D viewport
class Camera {
public:
    Camera();
    
    void setProjection(float fov, float aspect, float near, float far);
    void setView(const Eigen::Affine3f& viewMatrix);
    
    void orbit(float yaw, float pitch, float distance);
    void pan(float x, float y);
    void zoom(float delta);
    
    const Eigen::Matrix4f& getProjectionMatrix() const { return projectionMatrix_; }
    const Eigen::Matrix4f& getViewMatrix() const { return viewMatrix_; }
    const Eigen::Vector3f& getPosition() const { return position_; }
    
    void update();

private:
    Eigen::Vector3f position_;
    Eigen::Vector3f target_;
    Eigen::Vector3f up_;
    float yaw_ = 0.0f;
    float pitch_ = 30.0f;
    float distance_ = 5.0f;
    
    Eigen::Matrix4f projectionMatrix_;
    Eigen::Matrix4f viewMatrix_;
    
    float fov_ = 45.0f;
    float aspect_ = 16.0f / 9.0f;
    float near_ = 0.1f;
    float far_ = 1000.0f;
};

// Shader program
class Shader {
public:
    Shader();
    ~Shader();
    
    bool loadFromFile(const std::string& vertexPath, const std::string& fragmentPath);
    bool compile(const std::string& vertexSource, const std::string& fragmentSource);
    
    void use();
    
    void setUniform(const std::string& name, const Eigen::Matrix4f& matrix);
    void setUniform(const std::string& name, const Eigen::Vector3f& vector);
    void setUniform(const std::string& name, float value);
    void setUniform(const std::string& name, int value);
    
    uint32_t getProgramID() const { return programID_; }

private:
    uint32_t programID_ = 0;
    bool compiled_ = false;
};

// Main Renderer class (Vulkan)
class Renderer {
public:
    Renderer();
    ~Renderer();
    
    // Initialization
    bool initialize(void* windowHandle, int width, int height);
    void cleanup();
    
    // Rendering
    void beginFrame();
    void render(const Mesh& mesh, const Skeleton& skeleton, const Camera& camera);
    void endFrame();
    
    // Viewport
    void resize(int width, int height);
    void setWireframeMode(bool enabled);
    void setXRayMode(bool enabled);
    void setMatCapMode(bool enabled);
    
    // Screenshot/Video export
    bool captureScreenshot(const std::string& filepath);
    bool startVideoRecording(const std::string& filepath, int fps = 30);
    bool stopVideoRecording();
    
    // Get swapchain images
    VkImage getCurrentImage() const { return swapchainImages_[currentFrame_]; }

private:
    bool createInstance();
    bool createDevice();
    bool createSwapchain(int width, int height);
    bool createRenderPass();
    bool createPipeline();
    bool createFramebuffers();
    bool createSyncObjects();
    
    void recordCommandBuffer(uint32_t imageIndex);
    
    // Vulkan objects
    VkInstance instance_ = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
    VkDevice device_ = VK_NULL_HANDLE;
    VkQueue graphicsQueue_ = VK_NULL_HANDLE;
    VkQueue presentQueue_ = VK_NULL_HANDLE;
    
    VkSurfaceKHR surface_ = VK_NULL_HANDLE;
    VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;
    std::vector<VkImage> swapchainImages_;
    std::vector<VkImageView> swapchainImageViews_;
    VkFormat swapchainImageFormat_;
    VkExtent2D swapchainExtent_;
    
    VkRenderPass renderPass_ = VK_NULL_HANDLE;
    VkPipelineGraphics pipeline_ = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout_ = VK_NULL_HANDLE;
    
    std::vector<VkFramebuffer> framebuffers_;
    
    VkCommandPool commandPool_ = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> commandBuffers_;
    
    VkSemaphore imageAvailableSemaphore_ = VK_NULL_HANDLE;
    VkSemaphore renderFinishedSemaphore_ = VK_NULL_HANDLE;
    VkFence inFlightFence_ = VK_NULL_HANDLE;
    
    // State
    bool wireframeMode_ = false;
    bool xRayMode_ = false;
    bool matCapMode_ = true;
    
    uint32_t currentFrame_ = 0;
    const int MAX_FRAMES_IN_FLIGHT = 2;
    
    // Video recording
    bool isRecording_ = false;
    std::string videoOutputPath_;
    int videoFPS_ = 30;
};

// Lighting system
class Lighting {
public:
    struct Light {
        Eigen::Vector3f position;
        Eigen::Vector3f color;
        float intensity;
        enum Type { DIRECTIONAL, POINT, SPOT };
        Type type;
    };
    
    Lighting();
    
    void addLight(const Light& light);
    void removeLight(int index);
    void clearLights();
    
    const std::vector<Light>& getLights() const { return lights_; }
    
    void setAmbientColor(const Eigen::Vector3f& color) { ambientColor_ = color; }
    const Eigen::Vector3f& getAmbientColor() const { return ambientColor_; }

private:
    std::vector<Light> lights_;
    Eigen::Vector3f ambientColor_ = Eigen::Vector3f(0.1f, 0.1f, 0.1f);
};

} // namespace mannequin
