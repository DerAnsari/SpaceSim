cmake_minimum_required(VERSION 3.26)
project(SpaceSim)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)

set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/imgui)
file(GLOB IMGUI_SOURCES
    ${IMGUI_DIR}/*.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

file(GLOB_RECURSE SOURCES "src/*.cpp" "main.cpp")
list(APPEND SOURCES ${IMGUI_SOURCES})

add_executable(SpaceSim ${SOURCES})

target_link_libraries(SpaceSim
    OpenGL::GL
    glfw
    GLEW::GLEW
    glm::glm
)

target_include_directories(SpaceSim PRIVATE
    include
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)
