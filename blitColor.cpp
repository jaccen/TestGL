

//替换指定区域的颜色
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>
#include <vector>
// 窗口大小
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
// 替换颜色的区域位置和大小
const int RECT_X = 200;
const int RECT_Y = 200;
const int RECT_WIDTH = 400;
const int RECT_HEIGHT = 200;

// 替换的颜色
glm::vec4 replacementColor(0.0f, 1.0f, 0.0f, 0.5f);  // 半透明绿色

const char* vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec2 aPosition;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPosition, 0.0, 1.0);\n"
    "}\0";
const char* fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 fragColor;\n"
    "uniform vec4 replacementColor;\n"
    "uniform vec2 rectPos;\n"
    "uniform vec2 rectSize;\n"

    "void main()\n"
    "{\n"
    "int screenWidth = 800;\n"
    "int screenHeight = 600;\n"

    "int x = int(gl_FragCoord.x);\n"
    "int y = int(gl_FragCoord.y);\n"

    "if (x >= int(rectPos.x) && x <= int(rectPos.x + rectSize.x) && y >= "
    "int(rectPos.y) && y <= int(rectPos.y + rectSize.y))\n"
    "{\n"
    "fragColor = replacementColor;\n"
    "}\n"
    "else\n"
    "{\n"
    " fragColor = vec4(0.0, 0.0, 0.0, 1.0); \n"
    " }\n"
    "}\n\0";

// 编译着色器并检查错误
GLuint compileShader(GLenum shaderType, const char* shaderSource) {
  GLuint shader = glCreateShader(shaderType);
  glShaderSource(shader, 1, &shaderSource, nullptr);
  glCompileShader(shader);

  // 检查编译错误
  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLchar infoLog[512];
    glGetShaderInfoLog(shader, 512, nullptr, infoLog);
    std::cerr << "Shader compilation failed:\n" << infoLog << std::endl;
    return 0;
  }

  return shader;
}

// 创建着色器程序并检查错误
GLuint createShaderProgram() {
  // 编译顶点着色器和片段着色器
  GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
  GLuint fragmentShader =
      compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  // 检查链接错误
  GLint success;
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    GLchar infoLog[512];
    glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
    std::cerr << "Shader program linking failed:\n" << infoLog << std::endl;
    return 0;
  }

  // 删除着色器对象
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return shaderProgram;
}
// GLFW 键盘输入回调函数
void keyCallback(GLFWwindow* window,
                 int key,
                 int scancode,
                 int action,
                 int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }
}
// 创建帧缓冲对象
GLuint createFrameBufferObject() {
  GLuint fbo;
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  return fbo;
}

// 创建纹理附件
GLuint createTextureAttachment(int width, int height) {
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, nullptr);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         texture, 0);
  return texture;
}

// 创建像素缓冲区对象
GLuint createPixelBufferObject(int size) {
  GLuint pbo;
  glGenBuffers(1, &pbo);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
  glBufferData(GL_PIXEL_UNPACK_BUFFER, size, nullptr, GL_STREAM_DRAW);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
  return pbo;
}
// 回调函数：当窗口尺寸变化时调用
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}
// 更新像素缓冲区对象数据
void updatePixelBufferObject(GLuint pbo,
                             const std::vector<unsigned char>& data) {
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
  GLubyte* ptr =
      static_cast<GLubyte*>(glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY));
  if (ptr) {
    memcpy(ptr, data.data(), data.size());
    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
  }
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

int main() {
  // 初始化GLFW
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return -1;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
  // 创建窗口
  GLFWwindow* window = glfwCreateWindow(
      WINDOW_WIDTH, WINDOW_HEIGHT, "Pixel Color Replacement", nullptr, nullptr);
  if (!window) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }

  // 将当前窗口的上下文设置为主上下文
  glfwMakeContextCurrent(window);

  // 初始化GLEW
  if (glewInit() != GLEW_OK) {
    std::cerr << "Failed to initialize GLEW" << std::endl;
    glfwTerminate();
    return -1;
  }

  // 设置视口大小
  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

  // 设置清空颜色
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  // 设置键盘输入回调函数
  glfwSetKeyCallback(window, keyCallback);

  // 创建着色器程序
  GLuint shaderProgram = createShaderProgram();

  // 获取 uniform 变量的位置
  GLint replacementColorLoc =
      glGetUniformLocation(shaderProgram, "replacementColor");
  GLint rectPosLoc = glGetUniformLocation(shaderProgram, "rectPos");
  GLint rectSizeLoc = glGetUniformLocation(shaderProgram, "rectSize");

  // 渲染循环
  while (!glfwWindowShouldClose(window)) {
    // 清空颜色缓冲区
    glClear(GL_COLOR_BUFFER_BIT);

    // 使用着色器程序
    glUseProgram(shaderProgram);

    // 设置 uniform 变量的值
    glUniform4fv(replacementColorLoc, 1, glm::value_ptr(replacementColor));
    glUniform2f(rectPosLoc, static_cast<float>(RECT_X),
                static_cast<float>(RECT_Y));
    glUniform2f(rectSizeLoc, static_cast<float>(RECT_WIDTH),
                static_cast<float>(RECT_HEIGHT));

    // 绘制一个矩形覆盖整个屏幕
    GLfloat vertices[] = {-1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f};

    GLuint indices[] = {0, 1, 2, 2, 3, 0};

    GLuint vertexArray;
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);

    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint elementBuffer;
    glGenBuffers(1, &elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &vertexBuffer);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &elementBuffer);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vertexArray);

    // 交换前后缓冲区
    glfwSwapBuffers(window);

    // 处理事件
    glfwPollEvents();
  }

  // 删除着色器程序
  glDeleteProgram(shaderProgram);

  // 清理 GLFW 资源
  glfwTerminate();

  return 0;
}
