cmake_minimum_required(VERSION 3.26)
project(SpaceSim)
set(CMAKE_CXX_STANDARD 20)

# Find dependencies
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(OpenMP REQUIRED)

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
set(GLAD_SOURCES ${GLAD_DIR}/src/gl.c
        include/circle.h
        include/SimObject.h
        include/universe.h)

# Your sources
file(GLOB PROJECT_SOURCES "src/*.cpp" "src/main.cpp")

# Build
add_executable(SpaceSim ${PROJECT_SOURCES} ${IMGUI_SOURCES} ${GLAD_SOURCES})

if(WIN32)
    # Force MinGW include paths to avoid host leakage
    target_include_directories(SpaceSim PRIVATE
        /usr/x86_64-w64-mingw32/include
        include
        src
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
        ${GLAD_DIR}/include
    )
    # Static linking for standalone .exe
    target_link_options(SpaceSim PRIVATE -static -static-libgcc -static-libstdc++)
    target_link_libraries(SpaceSim
        opengl32
        /usr/x86_64-w64-mingw32/lib/libglfw3.a
        gdi32
        user32
        kernel32
        OpenMP::OpenMP_CXX
    )
else()
    target_link_libraries(SpaceSim
        OpenGL::GL
        glfw
        glm::glm
        OpenMP::OpenMP_CXX
    )

    if(UNIX)
        target_link_options(SpaceSim PRIVATE -static-libgcc -static-libstdc++)
    endif()

    target_include_directories(SpaceSim PRIVATE
        include
        src
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
        ${GLAD_DIR}/include
    )
endif()
