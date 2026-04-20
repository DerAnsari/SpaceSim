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
#include "../include/camera.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>

using std::cout, std::endl;

//Sets width and height of the main window
int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;

Camera2D camera;

// Helper to read shader files
std::string readShaderFile(const char* filePath) {
    // Try original path
    std::ifstream file(filePath);
    if (file.is_open()) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    // Try parent directory (common when running from a build folder)
    std::string fallbackPath = "../" + std::string(filePath);
    std::ifstream fallbackFile(fallbackPath);
    if (fallbackFile.is_open()) {
        std::stringstream buffer;
        buffer << fallbackFile.rdbuf();
        return buffer.str();
    }

    std::cout << "Failed to open shader file at: " << filePath << " or " << fallbackPath << std::endl;
    return "";
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

// Global for trail quad
unsigned int trailVAO = 0;
unsigned int trailProgram = 0;

void setupTrailQuad() {
    float quadVertices[] = {
        -1.0f,  1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,

        -1.0f,  1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         1.0f,  1.0f, 0.0f
    };
    unsigned int VBO;
    glGenVertexArrays(1, &trailVAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(trailVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

//Boiler functions necessary for getting openGL up and running
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window, float dt);
void applyProjection(unsigned int shader, GLFWwindow *window, Camera2D& cam);

unsigned int shaderProgram = 0;

void processInput(GLFWwindow *window, float dt) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.moveUp(dt);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.moveDown(dt);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.moveLeft(dt);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.moveRight(dt);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
  camera.adjustZoom(static_cast<float>(yoffset));
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glfwGetFramebufferSize(window, &SCR_WIDTH, &SCR_HEIGHT);
  glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
}

void applyProjection(unsigned int shader, GLFWwindow *window, Camera2D& cam) {
  // 1. Get actual framebuffer size
  int frameWidth, frameHeight;
  glfwGetFramebufferSize(window, &frameWidth, &frameHeight);

  float aspect = static_cast<float>(frameWidth) / static_cast<float>(frameHeight);

  // 2. Create projection matrix using camera zoom and position
  glm::mat4 projection;
  float zoom = cam.zoom;
  float left, right, bottom, top;

  if (aspect > 1.0f) {
    left   = (cam.position.x - aspect) / zoom;
    right  = (cam.position.x + aspect) / zoom;
    bottom = (cam.position.y - 1.0f) / zoom;
    top    = (cam.position.y + 1.0f) / zoom;
  } else {
    left   = (cam.position.x - 1.0f) / zoom;
    right  = (cam.position.x + 1.0f) / zoom;
    bottom = (cam.position.y - (1.0f / aspect)) / zoom;
    top    = (cam.position.y + (1.0f / aspect)) / zoom;
  }
  projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);

  // 3. Send to GPU
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
  glfwSetScrollCallback(window, scroll_callback);

  // glad: load all OpenGL function pointers
  if (!gladLoadGL(glfwGetProcAddress)) {
    cout << "Failed to initialize GLAD" << endl;
    return -1;
  }

  // Enable Blending
  glEnable(GL_BLEND);
  // Set the math for how colors mix (Standard Alpha Blending)
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Fallback shader sources in case files are missing
  const char* fallbackVS = "#version 330 core\n"
                           "layout (location = 0) in vec3 aPos;\n"
                           "uniform mat4 projection;\n"
                           "uniform vec2 offset;\n"
                           "uniform float scale;\n"
                           "out vec2 localPos;\n"
                           "out vec2 worldPos;\n"
                           "void main() {\n"
                           "   localPos = aPos.xy;\n"
                           "   vec2 positioned = (aPos.xy * scale) + offset;\n"
                           "   worldPos = positioned;\n"
                           "   gl_Position = projection * vec4(positioned, 0.0, 1.0);\n"
                           "}\0";

  const char* fallbackFS = "#version 330 core\n"
                           "out vec4 FragColor;\n"
                           "in vec2 localPos;\n"
                           "in vec2 worldPos;\n"
                           "void main() {\n"
                           "   float distToParticleCenter = length(localPos);\n"
                           "   float particleAlpha = 1.0 - smoothstep(0.0, 1.0, distToParticleCenter);\n"
                           "   float distToGalaxyCenter = length(worldPos);\n"
                           "   float galaxyBrightness = 1.0 / (1.0 + distToGalaxyCenter * 1.5);\n"
                           "   vec3 color = vec3(0.8, 0.9, 1.0) * galaxyBrightness;\n"
                           "   FragColor = vec4(color, particleAlpha * 0.7);\n"
                           "}\n\0";

  // build and compile shader program from external files
  std::string vSourceStr = readShaderFile("shaders/vertex.glsl");
  std::string fSourceStr = readShaderFile("shaders/fragment.glsl");
  
  if (vSourceStr.empty() || fSourceStr.empty()) {
      std::cout << "Using fallback shaders..." << std::endl;
      shaderProgram = setupShaders(fallbackVS, fallbackFS);
  } else {
      shaderProgram = setupShaders(vSourceStr.c_str(), fSourceStr.c_str());
  }

  // Setup trails
  setupTrailQuad();
  const char* trailVS = "#version 330 core\nlayout (location = 0) in vec3 aPos; void main() { gl_Position = vec4(aPos, 1.0); }";
  const char* trailFS = "#version 330 core\nout vec4 FragColor; void main() { FragColor = vec4(0.0, 0.0, 0.0, 0.3); }";
  trailProgram = setupShaders(trailVS, trailFS);

  // Creates a circle, abstracted out to header file
  Circle circleRenderer(1.0f, 32);
  Universe myUniverse;

  //adding the sun
  myUniverse.addBody(new Star(0.0f, 0.0f, 500.0f, 0.04f));

  //Making our tuning variables
  float minRad{0.003f}, maxRad{0.008f};
  int numBodies{200}; // Increased for galaxy look
  float G = 0.0001f; // Matching the new G in universe.h
  float sunMass = 500.0f;


  //generating random coordinates thatll be the basis for our bodies spawning
  std::random_device rd;
  std::mt19937 gen(rd());
  // Standard distribution for a more "core-heavy" galaxy look
  std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * 3.14159f);
  std::uniform_real_distribution<float> radiusDist(0.15f, 0.9f); 
  std::uniform_real_distribution<float> sizeDist(minRad, maxRad);

  for (int i = 0; i < numBodies; i++) {
    float theta = angleDist(gen);
    float r = radiusDist(gen);
    
    float x = r * std::cos(theta);
    float y = r * std::sin(theta);

    Planet *p = new Planet(x, y, 1.0f, sizeDist(gen));

    // Calculate velocity for a circular orbit: v = sqrt(G*M / r)
    // We add a tiny bit of random variation to make it look "natural"
    float orbitSpeed = std::sqrt((G * sunMass) / r);
    
    glm::vec2 unitTangent = glm::normalize(glm::vec2(-y, x));
    p->vel = unitTangent * orbitSpeed;

    myUniverse.addBody(p);
  }

  float deltaTime = 0.0f;
  float lastFrame = 0.0f;

  // Initial clear
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // render loop
  while (!glfwWindowShouldClose(window)) {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput(window, deltaTime);

    // instead of full clear, draw a semi-transparent black quad to create trails
    glUseProgram(trailProgram);
    glBindVertexArray(trailVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // clear only depth if needed (we aren't using depth here, but good practice)
    glClear(GL_DEPTH_BUFFER_BIT);

    //tells gpu to use this shader program for drawing
    glUseProgram(shaderProgram);

    //boiler code necessary to keep it dynamically scaling
    applyProjection(shaderProgram, window, camera);

    //live update planatary circles and their gravity
    // Using deltaTime with a multiplier to slow down the whole simulation
    myUniverse.updatePhysics(deltaTime * 0.5f);

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
