#include <iostream>
#include <GLFW/glfw3.h>

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "SpaceSim", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    std::cout << "Window created! Press ESC or close window to exit.\n";

    while (!glfwWindowShouldClose(window)) {
        // Clear screen with a color
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Swap buffers
        glfwSwapBuffers(window);

        // Poll events
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
