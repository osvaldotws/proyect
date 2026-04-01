#pragma once

#include <imgui.h>
#include <vector>
#include <string>
#include <functional>
#include "core/mesh.h"
#include "render/renderer.h"

namespace mannequin {

// Forward declarations
class Engine;

// UI Viewport component
class ViewportUI {
public:
    ViewportUI();
    
    void render(Renderer& renderer, const Camera& camera);
    
    void setViewportSize(int width, int height) { viewportWidth_ = width; viewportHeight_ = height; }
    int getViewportWidth() const { return viewportWidth_; }
    int getViewportHeight() const { return viewportHeight_; }
    
    // View modes
    void setWireframeMode(bool enabled);
    void setXRayMode(bool enabled);
    void setShadedMode(bool enabled);
    
    bool isWireframeMode() const { return wireframeMode_; }
    bool isXRayMode() const { return xRayMode_; }
    bool isShadedMode() const { return shadedMode_; }

private:
    int viewportWidth_ = 800;
    int viewportHeight_ = 600;
    
    bool wireframeMode_ = false;
    bool xRayMode_ = false;
    bool shadedMode_ = true;
};

// Property Panel for character customization
class PropertyPanel {
public:
    PropertyPanel();
    
    void render(Mesh& mesh, Skeleton& skeleton);
    
    // Body parameters
    struct BodyParams {
        float height = 1.75f;
        float weight = 70.0f;
        float muscleMass = 0.5f;
        float chestSize = 0.5f;
        float hipSize = 0.5f;
        float waistSize = 0.5f;
        
        // Limb proportions
        float armLength = 1.0f;
        float legLength = 1.0f;
        float torsoLength = 1.0f;
        
        // Head
        float headSize = 1.0f;
    };
    
    BodyParams& getBodyParams() { return bodyParams_; }
    const BodyParams& getBodyParams() const { return bodyParams_; }

private:
    BodyParams bodyParams_;
    
    void renderHumanScaleSection();
    void renderAnatomySection();
    void renderBlendShapeSection(Mesh& mesh);
};

// Animation Timeline
class TimelineUI {
public:
    TimelineUI();
    
    void render(AnimationClip& clip, Skeleton& skeleton);
    
    void play();
    void pause();
    void stop();
    void seek(float time);
    
    bool isPlaying() const { return isPlaying_; }
    float getCurrentTime() const { return currentTime_; }
    float getDuration() const { return duration_; }
    
    void setDuration(float duration) { duration_ = duration; }
    
    // Callbacks
    using TimeChangedCallback = std::function<void(float)>;
    void onTimeChanged(TimeChangedCallback callback) { onTimeChanged_ = callback; }

private:
    bool isPlaying_ = false;
    float currentTime_ = 0.0f;
    float duration_ = 10.0f;
    float fps_ = 30.0f;
    
    TimeChangedCallback onTimeChanged_;
    
    void renderTimelineControls();
    void renderKeyframeEditor();
};

// Export Hub
class ExportHub {
public:
    ExportHub();
    
    void render(Mesh& mesh, Skeleton& skeleton, AnimationClip& clip);
    
    enum class ExportFormat {
        OBJ,
        FBX,
        GLTF,
        GLB,
        BVH,
        MP4_VIDEO
    };
    
    struct ExportSettings {
        ExportFormat format = ExportFormat::GLB;
        int lodLevel = 0; // 0 = highest, higher = lower poly
        bool includeAnimation = true;
        bool includeSkeleton = true;
        bool includeBlendShapes = true;
        int videoFPS = 30;
        int videoResolution[2] = {1920, 1080};
    };
    
    ExportSettings& getSettings() { return settings_; }
    const ExportSettings& getSettings() const { return settings_; }
    
    // Export functions
    bool exportMesh(const std::string& filepath, const Mesh& mesh);
    bool exportAnimation(const std::string& filepath, const AnimationClip& clip);
    bool exportVideo(const std::string& filepath, Renderer& renderer, int fps, int width, int height);

private:
    ExportSettings settings_;
    
    void renderFormatSelection();
    void renderLODSelection();
    void renderOptions();
};

// Main Interface
class Interface {
public:
    Interface();
    ~Interface();
    
    bool initialize(void* windowHandle);
    void cleanup();
    
    void beginFrame();
    void endFrame();
    
    void render(Engine& engine);
    
    // Get UI components
    ViewportUI& getViewport() { return viewport_; }
    PropertyPanel& getPropertyPanel() { return propertyPanel_; }
    TimelineUI& getTimeline() { return timeline_; }
    ExportHub& getExportHub() { return exportHub_; }

private:
    ViewportUI viewport_;
    PropertyPanel propertyPanel_;
    TimelineUI timeline_;
    ExportHub exportHub_;
    
    bool initialized_ = false;
};

} // namespace mannequin
