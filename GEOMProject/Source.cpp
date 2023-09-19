#define		GLEW_STATIC
#include	<stdio.h>
#include    <iostream>
#include    <string>
#include    <windows.h>
#include	<GL/glew.h>
#define		GLFW_DLL
#include	<GLFW/glfw3.h>

void mainRenderLoop();
int init();
//void framebufferSizeCallback(GLFWwindow* window, int width, int height);

#define WINDOW_HEIGHT   720
#define WINDOE_WIDTH    1280

GLFWwindow* window;

GLuint vertex_buffer_object = 0;
GLuint vertex_array_object = 0; // Vertex Buffer Descriptor
GLuint shader_program_object = 0;
GLint program_linked;
GLint shader_compiled;
GLsizei log_length = 0;
GLchar message[1024];


GLfloat triangle_vertecies[] =
{
    .0f, .5f, .0f,
    .5f, -.5f, .0f,
    -.5f, -.5f, .0f
};

const char* vertex_shader = "#version 460\n"
"in vec3 attrib_position;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(attrib_position, 1.0);\n"
"}\n";

const char* frag_shader = "#version 460\n"
"out vec4 out_colour;\n"
"void main()\n"
"{\n"
"   out_colour = vec4(245.0f/255.0f, 157.0f/255.0f, 86.0f/255.0f, 1.0f);\n"
"}\n";

int main()
{
    int ret = init();

    if (ret != 0)
    {
        glfwTerminate();
        return ret;
    }

    mainRenderLoop();

    return 0;
}

int init()
{
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(1280, 720, "OpenGL", NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window!" << std::endl;

        glfwTerminate();

        return -1;
    }

    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    if (GLEW_OK != err)
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));

    fprintf(stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

    // Vertex Buffer Object

    glGenBuffers(1, &vertex_buffer_object);
    {
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 3, triangle_vertecies, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind
    }

    // Vertex Array Object - adatok elrendezésének leírása

    glGenVertexArrays(1, &vertex_array_object);
    {
        glBindVertexArray(vertex_array_object);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    {
        GLuint vertex_shader_object = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader_object, 1, &vertex_shader, NULL);

        glCompileShader(vertex_shader_object);

        glGetShaderiv(vertex_shader_object, GL_COMPILE_STATUS, &shader_compiled);
        if (shader_compiled != GL_TRUE)
        {
            glGetShaderInfoLog(vertex_shader_object, 1024, &log_length, message);
            fprintf(stderr, message);
            return -2;
        }

        GLuint fragment_shader_object = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader_object, 1, &frag_shader, NULL);
        glCompileShader(fragment_shader_object);

        glGetShaderiv(fragment_shader_object, GL_COMPILE_STATUS, &shader_compiled);
        if (shader_compiled != GL_TRUE)
        {
            glGetShaderInfoLog(fragment_shader_object, 1024, &log_length, message);
            fprintf(stderr, message);
            return -3;
        }

        shader_program_object = glCreateProgram();

        glBindAttribLocation(shader_program_object, 0, "attrib_position"); // The index passed into glBindAttribLocation is
        glAttachShader(shader_program_object, vertex_shader_object);
        glAttachShader(shader_program_object, fragment_shader_object);
        glLinkProgram(shader_program_object);

        glGetProgramiv(shader_program_object, GL_LINK_STATUS, &program_linked);
        if (program_linked != GL_TRUE)
        {
            glGetProgramInfoLog(shader_program_object, 1024, &log_length, message);
            fprintf(stderr, message);
            return -4;
        }

        //GLint u_time_loc = glGetUniformLocation(shader_program_object, "u_time_s");
    }

    return 0;
}

void mainRenderLoop()
{
    glViewport(0, 0, 1280, 720);

    //glClearColor(.2f, .2f, .2f, 1.0f);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);


    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader_program_object);
        glBindVertexArray(vertex_array_object);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        /*UNDONE*/

        glBindVertexArray(0);
        glUseProgram(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}

//void framebufferSizeCallback(GLFWwindow* window, int width, int height)
//{
//    glViewport(0, 0, width, height);
//}