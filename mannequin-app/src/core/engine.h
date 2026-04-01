#pragma once

#include "core/mesh.h"
#include "render/renderer.h"
#include "ui/interface.h"
#include <string>
#include <memory>
#include <thread>
#include <atomic>

namespace mannequin {

// Python scripting interface
class PythonBridge {
public:
    static PythonBridge& getInstance();
    
    bool initialize();
    void cleanup();
    
    // Execute Python script
    bool executeScript(const std::string& script);
    bool executeFile(const std::string& filepath);
    
    // Call Python function
    bool callFunction(const std::string& functionName, const std::vector<std::string>& args = {});
    
    // Register C++ functions for Python
    void registerEngineFunctions();

private:
    PythonBridge() = default;
    ~PythonBridge();
    
    void* pythonState_ = nullptr;
    bool initialized_ = false;
};

// Main Engine class - orchestrates all components
class Engine {
public:
    Engine();
    ~Engine();
    
    // Initialization
    bool initialize(void* windowHandle, int width, int height);
    void shutdown();
    
    // Main loop
    void update(float deltaTime);
    void render();
    
    // Scene management
    Mesh& getMainMesh() { return mainMesh_; }
    Skeleton& getSkeleton() { return skeleton_; }
    AnimationClip& getAnimation() { return currentAnimation_; }
    Camera& getCamera() { return camera_; }
    Renderer& getRenderer() { return renderer_; }
    Interface& getInterface() { return interface_; }
    Lighting& getLighting() { return lighting_; }
    
    // Load/Save
    bool loadCharacter(const std::string& filepath);
    bool saveCharacter(const std::string& filepath);
    bool loadAnimation(const std::string& filepath);
    
    // Export
    bool exportToFormat(const std::string& filepath, Interface::ExportHub::ExportFormat format);
    
    // Video recording
    bool startVideoRecording(const std::string& filepath, int fps = 30, int width = 1920, int height = 1080);
    bool stopVideoRecording();
    bool isRecordingVideo() const { return isRecording_; }
    
    // Time management
    float getCurrentTime() const { return currentTime_; }
    void setCurrentTime(float time);
    
    // Play/Pause
    void play() { isPlaying_ = true; }
    void pause() { isPlaying_ = false; }
    bool isPlaying() const { return isPlaying_; }

private:
    void updateAnimation(float deltaTime);
    void applyBodyParameters();
    void recordVideoFrame();
    
    // Core components
    Mesh mainMesh_;
    Skeleton skeleton_;
    AnimationClip currentAnimation_;
    Camera camera_;
    Renderer renderer_;
    Interface interface_;
    Lighting lighting_;
    
    // State
    bool initialized_ = false;
    bool isPlaying_ = false;
    bool isRecording_ = false;
    float currentTime_ = 0.0f;
    
    // Video recording
    std::thread videoRecordingThread_;
    std::atomic<bool> stopRecording_{false};
    
    // Body parameters cache
    Interface::PropertyPanel::BodyParams cachedBodyParams_;
};

} // namespace mannequin
