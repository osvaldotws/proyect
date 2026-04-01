cmake_minimum_required(VERSION 3.20)
project(MannequinStudio VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options
option(BUILD_TESTS "Build tests" OFF)
option(USE_VULKAN "Use Vulkan instead of OpenGL" ON)

# Find packages
find_package(Vulkan REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLM REQUIRED)
find_package(assimp REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# Core library
add_library(mannequin_core STATIC
    src/core/engine.cpp
    src/core/mesh.cpp
    src/core/skeleton.cpp
    src/core/animation.cpp
    src/core/deformation.cpp
)

target_include_directories(mannequin_core PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${Vulkan_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR}
    ${GLM_INCLUDE_DIRS}
    ${assimp_INCLUDE_DIRS}
    ${EIGEN3_INCLUDE_DIRS}
    ${Python3_INCLUDE_DIRS}
)

target_link_libraries(mannequin_core PUBLIC
    ${Vulkan_LIBRARIES}
    ${OPENGL_LIBRARIES}
    assimp
    Eigen3::Eigen
    ${Python3_LIBRARIES}
)

# Render module
add_library(mannequin_render STATIC
    src/render/renderer.cpp
    src/render/shader.cpp
    src/render/camera.cpp
    src/render/lighting.cpp
)

target_link_libraries(mannequin_render PUBLIC mannequin_core)

# UI Module (using Dear ImGui + GLFW)
add_library(mannequin_ui STATIC
    src/ui/interface.cpp
    src/ui/viewport.cpp
    src/ui/timeline.cpp
    src/ui/panels.cpp
)

target_link_libraries(mannequin_ui PUBLIC mannequin_render)

# Main executable
add_executable(mannequin_app
    src/main.cpp
)

target_link_libraries(mannequin_app PRIVATE
    mannequin_core
    mannequin_render
    mannequin_ui
)

# Python bindings (optional)
if(Python3_FOUND)
    add_library(python_bindings SHARED
        src/utils/python_bridge.cpp
    )
    target_link_libraries(python_bindings PRIVATE mannequin_core ${Python3_LIBRARIES})
    set_target_properties(python_bindings PROPERTIES PREFIX "")
endif()

# Install targets
install(TARGETS mannequin_app DESTINATION bin)
install(DIRECTORY assets/ DESTINATION share/mannequin/assets)

# Testing
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

message(STATUS "Building Mannequin Studio with Vulkan: ${USE_VULKAN}")
message(STATUS "Python found: ${Python3_EXECUTABLE}")
