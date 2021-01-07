cmake_minimum_required(VERSION 3.17)
project(simplePhysicsEngine)

set(CMAKE_CXX_STANDARD 11)



add_subdirectory(extern/glad)
add_subdirectory(extern/glfw)
add_subdirectory(extern/imgui)



set (ENGINE_SOURCE_FILES
        src/render/imgui/imgui_impl_glfw.cpp
        src/render/imgui/imgui_impl_glfw.h
        src/render/imgui/imgui_impl_opengl3.cpp
        src/render/imgui/imgui_impl_opengl3.h
        src/render/render.h
        src/main.cpp
        src/common/math.h
        src/render/draw.h
        src/render/draw.cpp
        src/common/common.h
        src/common/macro.h src/common/header.h src/core/world.h src/core/object.h src/core/object.cpp src/core/world.cpp src/core/shape.h src/core/shape.cpp)

add_executable(simplePhysicsEngine ${ENGINE_SOURCE_FILES})
target_include_directories(simplePhysicsEngine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(simplePhysicsEngine PUBLIC glfw imgui glad)
