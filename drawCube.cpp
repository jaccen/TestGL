//绘制一个彩盒子
#include <iostream>
using namespace std;

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;


//Global Variables
GLFWwindow* window;
const char* Title="OpenGL Window";
int width= 500,height = 500;

GLuint vbo,vao;
GLuint vs,fs,program;
const char* vertexShader   =     "#version 330 core\n"
                "layout(location=0) in vec3 pos;\n"
                "layout(location=1) in vec3 color;\n"
                "out vec3 Texcolor;\n"
                "uniform mat4 model,view,projection;\n"
                "void main(){\n"
                "Texcolor = color;"
                "gl_Position = projection*view*model*vec4(pos,1.0f);\n"
                "}";

const char* fragmentShader =     "#version 330 core\n"
                "in vec3 Texcolor;\n"
                "out vec4 fragcolor;\n"
                "void main(){\n"
                "fragcolor = vec4(Texcolor,1.0f);\n"
                "}";
// Global variables
GLfloat cubeVertices[] = {
    // Position           // Color
    -0.5f, -0.5f, 0.5f,   1.0f, 0.0f, 0.0f, // Front face - Red
    0.5f, -0.5f, 0.5f,    1.0f, 0.0f, 0.0f,
    0.5f, 0.5f, 0.5f,     1.0f, 0.0f, 0.0f,
    -0.5f, 0.5f, 0.5f,    1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, // Back face - Red
    0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,
    0.5f, 0.5f, -0.5f,    1.0f, 0.0f, 0.0f,
    -0.5f, 0.5f, -0.5f,   1.0f, 0.0f, 0.0f,
};

GLuint cubeIndices[] = {
    0, 1, 2, // Front face
    2, 3, 0,
    4, 5, 6, // Back face
    6, 7, 4,
    7, 3, 0, // Left face
    0, 4, 7,
    1, 5, 6, // Right face
    6, 2, 1,
    3, 2, 6, // Top face
    6, 7, 3,
    0, 1, 5, // Bottom face
    5, 4, 0,
};
void renderCube()
{
    // Create VAO, VBO, and EBO for the cube
    GLuint cubeVAO, cubeVBO, cubeEBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);

    // Bind the cube VAO
    glBindVertexArray(cubeVAO);

    // Bind the cube VBO and load the cube vertices into the buffer
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    // Set the vertex attribute pointers for the cube
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Bind the cube EBO and load the cube indices into the buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

    // Unbind the cube VBO, VAO, and EBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Render the cube
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Cleanup
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &cubeEBO);
}


//Main Program
int main()
{
    //Constructor Code
    if(!glfwInit())
    {
        cerr<<"Error!!GLFW";
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
    if(!(window = glfwCreateWindow(width,height,Title,NULL,NULL)))
    {
        cerr<<"Error!!GLFW window";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if(glewInit()!=GLEW_OK)
    {
        cerr<<"Error!!GLEW";
        glfwTerminate();
        return -1;
    }

    

    //Shader Creation
    vs = glCreateShader(GL_VERTEX_SHADER);
    fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vs,1,&vertexShader,NULL);
    glShaderSource(fs,1,&fragmentShader,NULL);
    glCompileShader(vs);
    glCompileShader(fs);
    int result;
    GLchar infoLog[1024];
    glGetShaderiv(vs,GL_COMPILE_STATUS,&result);
    if(!result)
    {
        glGetShaderInfoLog(vs,sizeof(infoLog),NULL,infoLog);
        cerr<<"Vertex Shader : Error !! "<<infoLog;
    }
    glGetShaderiv(fs,GL_COMPILE_STATUS,&result);
    if(!result)
    {
        glGetShaderInfoLog(fs,sizeof(infoLog),NULL,infoLog);
        cerr<<"Fragment Shader : Error !! "<<infoLog;
    }
    program = glCreateProgram();
    glAttachShader(program,vs);
    glAttachShader(program,fs);
    glLinkProgram(program);
    glGetProgramiv(fs,GL_LINK_STATUS,&result);
    if(!result)
    {
        glGetProgramInfoLog(fs,sizeof(infoLog),NULL,infoLog);
        cerr<<"Shader Program : Error !! "<<infoLog;
    }


    //MVP matrix and ( Cube Position and Rotation )
    mat4 model,view,projection;
    view        = lookAt(vec3(0.0f,0.0f,3.0f),vec3(0.0f,0.0f,0.0f),vec3(1.0f,1.0f,0.0f));
    projection = perspective(radians(45.0f),(float)width/(float)height,0.1f,100.0f);
    float cubeAngle = 0.0f;
    vec3 cubePos(0.0f,0.0f,-5.0f);
    //Loop Events
    while(!glfwWindowShouldClose(window))
    {
        //Calculation
        cubeAngle+=1.0f;
        if(cubeAngle>=360) cubeAngle=0;
        model = mat4(1.0f);
        model = translate(model,cubePos)*rotate(model,radians(cubeAngle),vec3(1.0f,0.0f,0.0f));
        //Rendering
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glClearColor(0.1f,0.3f,0.6f,1.0f);
        glUseProgram(program);
            //Assign Shader Uniforms
        glUniformMatrix4fv(glGetUniformLocation(program,"model"),1,GL_FALSE,&model[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(program,"view"),1,GL_FALSE,&view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(program,"projection"),1,GL_FALSE,&projection[0][0]);
        renderCube();
        glEnable(GL_DEPTH_TEST);
        glfwSwapBuffers(window);
    }

    //Destructor Code
    glDetachShader(program,vs);
    glDetachShader(program,fs);
    glDeleteShader(vs);
    glDeleteShader(fs);
    glDeleteProgram(program);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glfwTerminate();
    return 0;

}//End of Main Program
#endif
