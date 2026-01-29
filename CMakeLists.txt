cmake_minimum_required(VERSION 3.26)
project(SpaceSim)

set(CMAKE_CXX_STANDARD 17)

# 1. Find your dependencies
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)

# 2. Define ImGui directory
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/imgui)

# 3. Explicitly pick ONLY the files you need
# Avoid GLOB_RECURSE on the project root to prevent grabbing example folders
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# 4. Grab your actual project source (don't recurse into 'external')
file(GLOB PROJECT_SOURCES "src/*.cpp" "main.cpp")

# 5. Build
add_executable(SpaceSim ${PROJECT_SOURCES} ${IMGUI_SOURCES})

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
