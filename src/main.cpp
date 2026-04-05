//Boiler Code Dependencies
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Own Dependencies
#include "../include/circle.h"
#include <iostream>
#include <random>

using std::cout, std::endl;

//Sets width and height of the main window
int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;

//Boiler functions necessary for getting openGL up and running
void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void processInput(GLFWwindow *window);

// Compiling the Necessary Shaders (setups)
auto vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "uniform mat4 projection;\n"
    "uniform vec2 offset;\n"
    "uniform float scale;\n"
    "void main()\n"
    "{\n"
    "   vec2 positioned = (aPos.xy * scale) + offset;\n"
    "   gl_Position = projection * vec4(positioned, 0.0, 1.0);\n"
    "}\0";

auto fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 1.0f, 1.0f, 0.6f);\n"
    "}\n\0";

unsigned int shaderProgram = 0;

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glfwGetFramebufferSize(window, &SCR_WIDTH, &SCR_HEIGHT);
  glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
}

int main() {
  //struct used to keep the attributes fed to draw circle func
  struct Body {
    float x, y;
    float radius;
  };

  // glfw: initialize and configure
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  //Special stuff needed to run on macOS
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  // glfw window creation
  GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Circle OpenGL", nullptr, nullptr);
  if (window == nullptr) {
    cout << "Failed to create GLFW window" << endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // glad: load all OpenGL function pointers
  if (!gladLoadGL(glfwGetProcAddress)) {
    cout << "Failed to initialize GLAD" << endl;
    return -1;
  }

  // Enable Blending
  glEnable(GL_BLEND);
  // Set the math for how colors mix (Standard Alpha Blending)
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // build and compile shader program

  //compiles vertex shaders
  unsigned const int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
  glCompileShader(vertexShader);

  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
    cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
  }

  //compiles fragment shaders
  unsigned const int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
  glCompileShader(fragmentShader);

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
    cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
  }

  //link the two shaders into a shader Programm to be used by the main function
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  //check if shaders linked successfully or not
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
    cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
  }

  //getting rid of garbage
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  //generating random coordinates thatll be the basis for out bodies spaqning
  std::vector<Body> bodies;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> posDist(-0.8f, 0.8f); // Stay away from very edges
  std::uniform_real_distribution<float> sizeDist(0.002f, 0.005f);

  for (int i = 0; i < 5000; i++) {
    bodies.push_back({posDist(gen), posDist(gen), sizeDist(gen)});
  }
  // Creates a circle, abstracted out to header file
  Circle circle(1.0f, 100);

  // render loop
  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    //adds color to the window
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); //black
    glClear(GL_COLOR_BUFFER_BIT); //clear screen post loop

    glUseProgram(shaderProgram); //tells gpu to use this shader program for drawing

    //boiler code necessary to keep it dynamically scaling


    // Get actual framebuffer size (handles DPI scaling)
    int frameWidth, frameHeight;
    glfwGetFramebufferSize(window, &frameWidth, &frameHeight);

    float aspect = static_cast<float>(frameWidth) / static_cast<float>(frameHeight);

    // Create projection matrix that maintains aspect ratio
    glm::mat4 projection;
    if (aspect > 1.0f) {
      // Window is wider - scale X
      projection = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
    } else {
      // Window is taller - scale Y
      projection = glm::ortho(-1.0f, 1.0f, -1.0f / aspect, 1.0f / aspect, -1.0f, 1.0f);
    }

    int const projLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


    // draws the circle (abstracted out to the header file
    for (const auto &b: bodies) {
      // Draw the circle geometry at that spot
      circle.draw(shaderProgram, b.x, b.y, b.radius);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  //clean Up
  glDeleteProgram(shaderProgram);
  glfwTerminate();
  return 0;
}
