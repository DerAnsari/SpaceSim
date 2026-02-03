cmake_minimum_required(VERSION 3.26)
project(SpaceSim)
set(CMAKE_CXX_STANDARD 17)

# Find dependencies
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

# ImGui
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# GLAD
set(GLAD_DIR ${CMAKE_SOURCE_DIR}/external/glad)
set(GLAD_SOURCES ${GLAD_DIR}/src/gl.c)

# Your sources
file(GLOB PROJECT_SOURCES "src/*.cpp" "main.cpp")

# Build
add_executable(SpaceSim ${PROJECT_SOURCES} ${IMGUI_SOURCES} ${GLAD_SOURCES})

target_link_libraries(SpaceSim
    OpenGL::GL
    glfw
    glm::glm
)

target_include_directories(SpaceSim PRIVATE
    include
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${GLAD_DIR}/include
)
