//Boiler Code Dependencies
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Own Dependencies
#include "../include/circle.h"
#include "../include/SimObject.h"
#include "../include/universe.h"
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

unsigned int setupShaders(const char *vSrc, const char *fSrc) {
  int success;
  char infoLog[512];

  // Vertex
  unsigned int vShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vShader, 1, &vSrc, nullptr);
  glCompileShader(vShader);
  glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vShader, 512, nullptr, infoLog);
    std::cout << "ERROR::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
  }

  // Fragment
  unsigned int fShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fShader, 1, &fSrc, nullptr);
  glCompileShader(fShader);
  glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fShader, 512, nullptr, infoLog);
    std::cout << "ERROR::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
  }

  // Link
  unsigned int program = glCreateProgram();
  glAttachShader(program, vShader);
  glAttachShader(program, fShader);
  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program, 512, nullptr, infoLog);
    std::cout << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
  }

  // cleanup
  glDeleteShader(vShader);
  glDeleteShader(fShader);
  return program;
}

void applyProjection(unsigned int shader, GLFWwindow *window) {
  // 1. Get actual framebuffer size
  int frameWidth, frameHeight;
  glfwGetFramebufferSize(window, &frameWidth, &frameHeight);

  float aspect = static_cast<float>(frameWidth) / static_cast<float>(frameHeight);

  // 2. Create projection matrix
  glm::mat4 projection;
  if (aspect > 1.0f) {
    projection = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
  } else {
    projection = glm::ortho(-1.0f, 1.0f, -1.0f / aspect, 1.0f / aspect, -1.0f, 1.0f);
  }

  // 3. Send to GPU
  // Use 'shader' (the parameter), not 'shaderProgram'
  int projLoc = glGetUniformLocation(shader, "projection");
  glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
}


int main() {
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
  unsigned int shaderProgram = setupShaders(vertexShaderSource, fragmentShaderSource);


  // Creates a circle, abstracted out to header file
  Circle circleRenderer(1.0f, 32);
  Universe myUniverse;

  //adding the sun
  myUniverse.addBody(new Star(0.0f, 0.0f, 500.0f, 0.04f));

  //Making our tuning variables
  float minRad{0.003f}, maxRad{0.008f};
  int numBodies{100};
  float G = 0.001f;
  float sunMass = 500.0f;

  //generating random coordinates thatll be the basis for our bodies spawning
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> posDist(-0.9f, 0.9f); // Stay away from very edges
  std::uniform_real_distribution<float> sizeDist(minRad, maxRad);

  for (int i = 0; i < numBodies; i++) {
    float x = posDist(gen);
    float y = posDist(gen);

    // 1. Calculate distance from Sun (at 0,0)
    float r = std::sqrt(x * x + y * y);

    // Avoid spawning planets inside the Sun or too close (prevents slingshots)
    if (r < 0.1f) {
      i--;
      continue;
    }

    Planet *p = new Planet(x, y, 1.0f, sizeDist(gen));

    // 2. Calculate the required speed for a circular orbit
    float speed = std::sqrt((G * sunMass) / r);

    // 3. Create a perpendicular velocity vector
    // For a position (x, y), the perpendicular vector is (-y, x)
    glm::vec2 unitTangent = glm::normalize(glm::vec2(-y, x));
    p->vel = unitTangent * speed;

    myUniverse.addBody(p);
  }

  // render loop
  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    //adds color to the window
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); //black
    glClear(GL_COLOR_BUFFER_BIT); //clear screen post loop

    //tells gpu to use this shader program for drawing
    glUseProgram(shaderProgram);

    //boiler code necessary to keep it dynamically scaling
    applyProjection(shaderProgram, window);

    //live update planatary circles and their gravity
    myUniverse.updatePhysics(0.01f);

    // draws the circle (abstracted out to the header file
    myUniverse.renderAll(shaderProgram, circleRenderer);


    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  //clean Up
  glDeleteProgram(shaderProgram);
  glfwTerminate();
  return 0;
}
