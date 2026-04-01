#include <GLFW/glfw3.h>
#include "core/engine.h"
#include <iostream>
#include <chrono>

// Global engine instance
mannequin::Engine* g_engine = nullptr;

// Window resize callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    if (g_engine) {
        g_engine->getRenderer().resize(width, height);
    }
}

// Key input callback
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, true);
                break;
            case GLFW_KEY_SPACE:
                if (g_engine) {
                    if (g_engine->isPlaying()) {
                        g_engine->pause();
                    } else {
                        g_engine->play();
                    }
                }
                break;
            case GLFW_KEY_R:
                if (mods & GLFW_MOD_CONTROL) {
                    // Start/stop recording
                    if (g_engine && !g_engine->isRecordingVideo()) {
                        g_engine->startVideoRecording("output_video.mp4");
                    } else if (g_engine) {
                        g_engine->stopVideoRecording();
                    }
                }
                break;
        }
    }
}

int main(int argc, char** argv) {
    std::cout << "=== Mannequin Studio 3D ===" << std::endl;
    std::cout << "Advanced Character Creation & Animation Tool" << std::endl;
    std::cout << std::endl;
    
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    // Configure GLFW for Vulkan
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    
    // Create window
    GLFWwindow* window = glfwCreateWindow(
        1920, 1080,
        "Mannequin Studio - 3D Character Creator",
        nullptr, nullptr
    );
    
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    // Set callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    
    // Create engine
    g_engine = new mannequin::Engine();
    
    // Initialize engine with Vulkan surface
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    
    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(g_engine->getRenderer().getInstance(), window, nullptr, &surface) != VK_SUCCESS) {
        std::cerr << "Failed to create Vulkan surface" << std::endl;
        delete g_engine;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    
    if (!g_engine->initialize((void*)window, width, height)) {
        std::cerr << "Failed to initialize engine" << std::endl;
        delete g_engine;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    
    std::cout << "Engine initialized successfully!" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  - SPACE: Play/Pause animation" << std::endl;
    std::cout << "  - CTRL+R: Start/Stop video recording" << std::endl;
    std::cout << "  - ESC: Exit" << std::endl;
    std::cout << std::endl;
    
    // Main loop
    auto lastTime = std::chrono::high_resolution_clock::now();
    
    while (!glfwWindowShouldClose(window)) {
        // Calculate delta time
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;
        
        // Poll events
        glfwPollEvents();
        
        // Update
        g_engine->update(deltaTime);
        
        // Render
        g_engine->render();
        
        // Check if recording and capture frame
        if (g_engine->isRecordingVideo()) {
            // Video recording handled in engine
        }
    }
    
    // Cleanup
    g_engine->shutdown();
    delete g_engine;
    g_engine = nullptr;
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    std::cout << "Application closed successfully." << std::endl;
    
    return 0;
}
