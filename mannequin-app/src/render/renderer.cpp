#include "render/renderer.h"
#include <iostream>
#include <algorithm>

namespace mannequin {

// Camera Implementation
Camera::Camera() 
    : position_(0.0f, 0.0f, 5.0f)
    , target_(0.0f, 0.0f, 0.0f)
    , up_(0.0f, 1.0f, 0.0f) {
}

void Camera::setProjection(float fov, float aspect, float near, float far) {
    fov_ = fov;
    aspect_ = aspect;
    near_ = near;
    far_ = far;
    
    float tanHalfFov = tan(fov / 2.0f * 3.14159f / 180.0f);
    projectionMatrix_ = Eigen::Matrix4f::Zero();
    
    projectionMatrix_(0, 0) = 1.0f / (aspect * tanHalfFov);
    projectionMatrix_(1, 1) = 1.0f / tanHalfFov;
    projectionMatrix_(2, 2) = -(far + near) / (far - near);
    projectionMatrix_(2, 3) = -(2.0f * far * near) / (far - near);
    projectionMatrix_(3, 2) = -1.0f;
}

void Camera::setView(const Eigen::Affine3f& viewMatrix) {
    viewMatrix_ = viewMatrix.matrix();
}

void Camera::orbit(float yaw, float pitch, float distance) {
    yaw_ = yaw;
    pitch_ = std::max(-89.0f, std::min(89.0f, pitch));
    distance_ = distance;
    
    float radYaw = yaw * 3.14159f / 180.0f;
    float radPitch = pitch * 3.14159f / 180.0f;
    
    position_.x() = target_.x() + distance * cos(radPitch) * sin(radYaw);
    position_.y() = target_.y() + distance * sin(radPitch);
    position_.z() = target_.z() + distance * cos(radPitch) * cos(radYaw);
}

void Camera::pan(float x, float y) {
    Eigen::Vector3f right = (position_ - target_).cross(up_).normalized();
    Eigen::Vector3f up = right.cross(position_ - target_).normalized();
    
    target_ += right * x + up * y;
    position_ += right * x + up * y;
}

void Camera::zoom(float delta) {
    distance_ = std::max(0.5f, distance_ - delta);
    orbit(yaw_, pitch_, distance_);
}

void Camera::update() {
    // LookAt matrix
    Eigen::Vector3f zAxis = (position_ - target_).normalized();
    Eigen::Vector3f xAxis = zAxis.cross(up_).normalized();
    Eigen::Vector3f yAxis = xAxis.cross(zAxis).normalized();
    
    viewMatrix_ = Eigen::Matrix4f::Identity();
    viewMatrix_(0, 0) = xAxis.x(); viewMatrix_(0, 1) = xAxis.y(); viewMatrix_(0, 2) = xAxis.z();
    viewMatrix_(1, 0) = yAxis.x(); viewMatrix_(1, 1) = yAxis.y(); viewMatrix_(1, 2) = yAxis.z();
    viewMatrix_(2, 0) = zAxis.x(); viewMatrix_(2, 1) = zAxis.y(); viewMatrix_(2, 2) = zAxis.z();
    viewMatrix_(0, 3) = -xAxis.dot(position_);
    viewMatrix_(1, 3) = -yAxis.dot(position_);
    viewMatrix_(2, 3) = -zAxis.dot(position_);
}

// Shader Implementation
Shader::Shader() {}

Shader::~Shader() {
    if (programID_ != 0) {
        // glDeleteProgram(programID_);
    }
}

bool Shader::loadFromFile(const std::string& vertexPath, const std::string& fragmentPath) {
    // Load shader source from files
    std::ifstream vFile(vertexPath);
    std::ifstream fFile(fragmentPath);
    
    if (!vFile.is_open() || !fFile.is_open()) {
        return false;
    }
    
    std::string vSource((std::istreambuf_iterator<char>(vFile)),
                        std::istreambuf_iterator<char>());
    std::string fSource((std::istreambuf_iterator<char>(fFile)),
                        std::istreambuf_iterator<char>());
    
    return compile(vSource, fSource);
}

bool Shader::compile(const std::string& vertexSource, const std::string& fragmentSource) {
    // Compile shaders (OpenGL implementation would go here)
    // For Vulkan, we'd compile SPIR-V
    compiled_ = true;
    return true;
}

void Shader::use() {
    // glUseProgram(programID_);
}

void Shader::setUniform(const std::string& name, const Eigen::Matrix4f& matrix) {
    // glUniformMatrix4fv(glGetUniformLocation(programID_, name.c_str()), 1, GL_FALSE, matrix.data());
}

void Shader::setUniform(const std::string& name, const Eigen::Vector3f& vector) {
    // glUniform3f(glGetUniformLocation(programID_, name.c_str()), vector.x(), vector.y(), vector.z());
}

void Shader::setUniform(const std::string& name, float value) {
    // glUniform1f(glGetUniformLocation(programID_, name.c_str()), value);
}

void Shader::setUniform(const std::string& name, int value) {
    // glUniform1i(glGetUniformLocation(programID_, name.c_str()), value);
}

// Renderer Implementation
Renderer::Renderer() {}

Renderer::~Renderer() {
    cleanup();
}

bool Renderer::initialize(void* windowHandle, int width, int height) {
    std::cout << "Initializing Vulkan Renderer..." << std::endl;
    
    if (!createInstance()) {
        std::cerr << "Failed to create Vulkan instance" << std::endl;
        return false;
    }
    
    if (!createDevice()) {
        std::cerr << "Failed to create Vulkan device" << std::endl;
        return false;
    }
    
    if (!createSwapchain(width, height)) {
        std::cerr << "Failed to create swapchain" << std::endl;
        return false;
    }
    
    if (!createRenderPass()) {
        std::cerr << "Failed to create render pass" << std::endl;
        return false;
    }
    
    if (!createPipeline()) {
        std::cerr << "Failed to create pipeline" << std::endl;
        return false;
    }
    
    if (!createFramebuffers()) {
        std::cerr << "Failed to create framebuffers" << std::endl;
        return false;
    }
    
    if (!createSyncObjects()) {
        std::cerr << "Failed to create sync objects" << std::endl;
        return false;
    }
    
    // Create command buffers
    commandBuffers_.resize(MAX_FRAMES_IN_FLIGHT);
    
    std::cout << "Vulkan Renderer initialized successfully!" << std::endl;
    return true;
}

void Renderer::cleanup() {
    if (device_ == VK_NULL_HANDLE) return;
    
    vkDeviceWaitIdle(device_);
    
    // Cleanup sync objects
    if (imageAvailableSemaphore_ != VK_NULL_HANDLE) {
        vkDestroySemaphore(device_, imageAvailableSemaphore_, nullptr);
    }
    if (renderFinishedSemaphore_ != VK_NULL_HANDLE) {
        vkDestroySemaphore(device_, renderFinishedSemaphore_, nullptr);
    }
    if (inFlightFence_ != VK_NULL_HANDLE) {
        vkDestroyFence(device_, inFlightFence_, nullptr);
    }
    
    // Cleanup framebuffers
    for (auto framebuffer : framebuffers_) {
        vkDestroyFramebuffer(device_, framebuffer, nullptr);
    }
    
    // Cleanup pipeline
    if (pipeline_ != VK_NULL_HANDLE) {
        vkDestroyPipeline(device_, pipeline_, nullptr);
    }
    if (pipelineLayout_ != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(device_, pipelineLayout_, nullptr);
    }
    
    // Cleanup render pass
    if (renderPass_ != VK_NULL_HANDLE) {
        vkDestroyRenderPass(device_, renderPass_, nullptr);
    }
    
    // Cleanup swapchain
    for (auto imageView : swapchainImageViews_) {
        vkDestroyImageView(device_, imageView, nullptr);
    }
    if (swapchain_ != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(device_, swapchain_, nullptr);
    }
    
    // Cleanup surface
    if (surface_ != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(instance_, surface_, nullptr);
    }
    
    // Cleanup device and instance
    if (device_ != VK_NULL_HANDLE) {
        vkDestroyDevice(device_, nullptr);
    }
    if (instance_ != VK_NULL_HANDLE) {
        vkDestroyInstance(instance_, nullptr);
    }
    
    device_ = VK_NULL_HANDLE;
    instance_ = VK_NULL_HANDLE;
}

bool Renderer::createInstance() {
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Mannequin Studio";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Mannequin Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2;
    
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    
    // Enable validation layers in debug
    const char* validationLayers[] = {"VK_LAYER_KHRONOS_validation"};
    createInfo.enabledLayerCount = 0; // Disabled for release
    
    if (vkCreateInstance(&createInfo, nullptr, &instance_) != VK_SUCCESS) {
        return false;
    }
    
    return true;
}

bool Renderer::createDevice() {
    // Select physical device (GPU)
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance_, &deviceCount, nullptr);
    
    if (deviceCount == 0) {
        std::cerr << "No Vulkan-capable GPUs found!" << std::endl;
        return false;
    }
    
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance_, &deviceCount, devices.data());
    
    // Use first available device (simplified)
    physicalDevice_ = devices[0];
    
    // Get queue families
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &queueFamilyCount, nullptr);
    
    // Create logical device
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = 0;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    
    VkPhysicalDeviceFeatures deviceFeatures{};
    
    const char* deviceExtensions[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = 1;
    createInfo.ppEnabledExtensionNames = deviceExtensions;
    
    if (vkCreateDevice(physicalDevice_, &createInfo, nullptr, &device_) != VK_SUCCESS) {
        return false;
    }
    
    vkGetDeviceQueue(device_, 0, 0, &graphicsQueue_);
    presentQueue_ = graphicsQueue_;
    
    return true;
}

bool Renderer::createSwapchain(int width, int height) {
    // Simplified swapchain creation
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface_;
    createInfo.minImageCount = 2;
    createInfo.imageFormat = VK_FORMAT_B8G8R8A8_SRGB;
    createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    createInfo.imageExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    createInfo.clipped = VK_TRUE;
    
    if (vkCreateSwapchainKHR(device_, &createInfo, nullptr, &swapchain_) != VK_SUCCESS) {
        return false;
    }
    
    // Get swapchain images
    uint32_t imageCount;
    vkGetSwapchainImagesKHR(device_, swapchain_, &imageCount, nullptr);
    swapchainImages_.resize(imageCount);
    vkGetSwapchainImagesKHR(device_, swapchain_, &imageCount, swapchainImages_.data());
    
    swapchainImageFormat_ = VK_FORMAT_B8G8R8A8_SRGB;
    swapchainExtent_ = createInfo.imageExtent;
    
    return true;
}

bool Renderer::createRenderPass() {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapchainImageFormat_;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    
    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    
    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    
    VkRenderPassCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = &colorAttachment;
    createInfo.subpassCount = 1;
    createInfo.pSubpasses = &subpass;
    createInfo.dependencyCount = 1;
    createInfo.pDependencies = &dependency;
    
    if (vkCreateRenderPass(device_, &createInfo, nullptr, &renderPass_) != VK_SUCCESS) {
        return false;
    }
    
    return true;
}

bool Renderer::createPipeline() {
    // Pipeline creation would load shaders and configure rasterization
    // Simplified for now
    return true;
}

bool Renderer::createFramebuffers() {
    framebuffers_.resize(swapchainImageViews_.size());
    return true;
}

bool Renderer::createSyncObjects() {
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    
    if (vkCreateSemaphore(device_, &semaphoreInfo, nullptr, &imageAvailableSemaphore_) != VK_SUCCESS ||
        vkCreateSemaphore(device_, &semaphoreInfo, nullptr, &renderFinishedSemaphore_) != VK_SUCCESS ||
        vkCreateFence(device_, &fenceInfo, nullptr, &inFlightFence_) != VK_SUCCESS) {
        return false;
    }
    
    return true;
}

void Renderer::beginFrame() {
    // Wait for previous frame
    vkWaitForFences(device_, 1, &inFlightFence_, VK_TRUE, UINT64_MAX);
    
    // Acquire next image
    uint32_t imageIndex;
    vkAcquireNextImageKHR(device_, swapchain_, UINT64_MAX, imageAvailableSemaphore_, VK_NULL_HANDLE, &imageIndex);
    
    currentFrame_ = imageIndex % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::render(const Mesh& mesh, const Skeleton& skeleton, const Camera& camera) {
    recordCommandBuffer(currentFrame_);
    
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    
    VkSemaphore waitSemaphores[] = {imageAvailableSemaphore_};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers_[currentFrame_];
    
    VkSemaphore signalSemaphores[] = {renderFinishedSemaphore_};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    
    vkResetFences(device_, 1, &inFlightFence_);
    
    if (vkQueueSubmit(graphicsQueue_, 1, &submitInfo, inFlightFence_) != VK_SUCCESS) {
        std::cerr << "Failed to submit draw command buffer!" << std::endl;
    }
}

void Renderer::endFrame() {
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    
    presentInfo.waitSemaphoreCount = 1;
    VkSemaphore waitSemaphores[] = {renderFinishedSemaphore_};
    presentInfo.pWaitSemaphores = waitSemaphores;
    
    VkSwapchainKHR swapchains[] = {swapchain_};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &currentFrame_;
    
    vkQueuePresentKHR(presentQueue_, &presentInfo);
}

void Renderer::recordCommandBuffer(uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    
    if (vkBeginCommandBuffer(commandBuffers_[imageIndex], &beginInfo) != VK_SUCCESS) {
        return;
    }
    
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass_;
    renderPassInfo.framebuffer = framebuffers_[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapchainExtent_;
    
    VkClearValue clearColor = {{{0.1f, 0.1f, 0.15f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;
    
    vkCmdBeginRenderPass(commandBuffers_[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    
    // Bind pipeline and draw mesh here
    
    vkCmdEndRenderPass(commandBuffers_[imageIndex]);
    
    if (vkEndCommandBuffer(commandBuffers_[imageIndex]) != VK_SUCCESS) {
        std::cerr << "Failed to record command buffer!" << std::endl;
    }
}

void Renderer::resize(int width, int height) {
    vkDeviceWaitIdle(device_);
    cleanup();
    createSwapchain(width, height);
    createFramebuffers();
}

void Renderer::setWireframeMode(bool enabled) {
    wireframeMode_ = enabled;
}

void Renderer::setXRayMode(bool enabled) {
    xRayMode_ = enabled;
}

void Renderer::setMatCapMode(bool enabled) {
    matCapMode_ = enabled;
}

bool Renderer::captureScreenshot(const std::string& filepath) {
    // Capture current framebuffer to image file
    return true;
}

bool Renderer::startVideoRecording(const std::string& filepath, int fps) {
    isRecording_ = true;
    videoOutputPath_ = filepath;
    videoFPS_ = fps;
    return true;
}

bool Renderer::stopVideoRecording() {
    isRecording_ = false;
    return true;
}

// Lighting Implementation
Lighting::Lighting() {}

void Lighting::addLight(const Light& light) {
    lights_.push_back(light);
}

void Lighting::removeLight(int index) {
    if (index >= 0 && index < static_cast<int>(lights_.size())) {
        lights_.erase(lights_.begin() + index);
    }
}

void Lighting::clearLights() {
    lights_.clear();
}

} // namespace mannequin
