#define		GLEW_STATIC
#include	<stdio.h>
#include    <iostream>
#include    <string>
#include    <windows.h>
#include	<GL/glew.h>
#define		GLFW_DLL
#include	<GLFW/glfw3.h>
#include    "gl_utils.h"
#include    <vector>
#include    <random>
#define     _USE_MATH_DEFINES
#include    <math.h>
#include    <numeric>
#include    "gl_utils.h"
#include    "maths_funcs.h"


#define VBO 2
#define VAO 1

void keyCallBack();
void mainRenderLoop();
int init();
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void genBezier(int N, int M);
void genGrid(GLint N, GLint M);
GLdouble deltaTime();

#define WINDOW_HEIGHT   720
#define WINDOW_WIDTH    1280

GLuint vertex_buffer_object[VBO];
GLuint vertex_array_object[VAO];
GLuint indices_buffer_object = 0;
GLuint shader_program_object = 0;
GLint program_linked;
GLint shader_compiled;
GLsizei log_length = 0;
GLchar message[1024];
char vertex_shader[1024 * 256];
char fragment_shader[1024 * 256];

GLint g_gl_width = 1280;
GLint g_gl_height = 720;
GLFWwindow* g_window = 0;

std::vector<std::vector<GLfloat>> controll_vertices;
std::vector<GLfloat> sur;

//std::vector<GLfloat> controll_vertices[4][4];

std::vector<GLfloat> K;
std::vector<GLfloat> J;
std::vector<GLfloat> xBezier;
std::vector<GLfloat> yBezier;
std::vector<GLfloat> zBezier;
std::vector<std::vector<GLfloat>> Bezier;

GLfloat n = 0.1f; // near
GLfloat f = 100.0f; // far
GLfloat fov = 67.0f * ONE_DEG_IN_RAD;
GLfloat aspect = (GLfloat)g_gl_width / (GLfloat)g_gl_height;
GLfloat inverse_range = 1.0f / tan(fov * 0.5f);
GLfloat Sx = inverse_range / aspect;
GLfloat Sy = inverse_range;
GLfloat Sz = -(f + n) / (f - n);
GLfloat Pz = -(2.0f * f * n) / (f - n);
GLfloat proj_mat[] = { Sx, 0.0f, 0.0f, 0.0f, 0.0f, Sy, 0.0f, 0.0f, 0.0f, 0.0f, Sz, -1.0f, 0.0f, 0.0f, Pz, 0.0f };
GLfloat cam_speed = 1.0f;
GLfloat cam_yaw_speed = 10.0f;
GLfloat cam_pos[] = { 0.0f, 0.0f, 12.0f };
GLfloat cam_yaw = 0.0f;
GLboolean cam_moved = false;
GLint d = 1;
GLdouble pos = 0.0;
GLdouble DT = 0.0;
mat4 T = translate(identity_mat4(), vec3(-cam_pos[0], -cam_pos[1], -cam_pos[2]));
mat4 R = rotate_y_deg(identity_mat4(), -cam_yaw);
mat4 view_mat = R * T;
mat4 model_mat = rotate_y_deg(identity_mat4(), ONE_DEG_IN_RAD * 100);

mat4 vert_mat = translate(identity_mat4(), vec3(0.0, 0.0, 0.0));
mat4 vert_mat_fl = translate(identity_mat4(), vec3(0.0, -2.0, 0.0));

GLint view_mat_location = 0;
GLint proj_mat_location = 0;
GLint model_mat_location = 0;
GLint vert_mat_location = 0;

int rot = 0;

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

void keyCallBack()
{
    if (glfwGetKey(g_window, GLFW_KEY_A))
    {
        cam_pos[0] -= cam_speed * DT;
        cam_moved = true;
    }
    if (glfwGetKey(g_window, GLFW_KEY_D))
    {
        cam_pos[0] += cam_speed * DT;
        cam_moved = true;
    }
    if (glfwGetKey(g_window, GLFW_KEY_W))
    {
        cam_pos[2] -= cam_speed * DT;
        cam_moved = true;
    }
    if (glfwGetKey(g_window, GLFW_KEY_S))
    {
        cam_pos[2] += cam_speed * DT;
        cam_moved = true;
    }
    if (glfwGetKey(g_window, GLFW_KEY_Q))
    {
        cam_pos[1] -= cam_speed * DT;
        cam_moved = true;
    }
    if (glfwGetKey(g_window, GLFW_KEY_E))
    {
        cam_pos[1] += cam_speed * DT;
        cam_moved = true;
    }
}

GLfloat factorial(int n)
{
    bool handle_odd =   false;
    GLfloat upto_number =   n;

    if ((n & 1) == 1)
    {
        upto_number -=  1;
        handle_odd   =  true;
    }
    
    GLfloat next_sum    =   upto_number;
    GLfloat next_multi  =   upto_number;
    GLfloat factorial = 1;

    while (next_sum >= 2)
    {
        factorial *= next_multi;
        next_sum -= 2;
        next_multi += next_sum;
    }

    if (handle_odd)
    {
        factorial *= n;
    }

    return factorial;
}

/* NxM - es kontroll háló legenerálása */
void genGrid(GLint N, GLint M)
{
    controll_vertices.clear();

    GLfloat r; 
    srand((unsigned int)time(NULL));

    //printf("\nKontroll pontok:\n\n");
    for (GLfloat i = 0.0; i < (GLfloat)N; i++)
    {
        for (GLfloat j = 0.0; j < (GLfloat)M; j++)
        {
            //r = (GLfloat)(rand() % M);
            //r = 0.0;
            if ((i == 0.0f || j == 0.0f) || (i == N-1 || j == M - 1))
            {
                r = 0.0;
            }
            else
            {
                r = 1.0;
            }

            controll_vertices.push_back({i,j,r});
            printf("[%lf; %lf; %lf]\n", i, j, r);
        }
    }

    genBezier(N, M);
}

GLfloat B(GLint nm, GLint ij, GLfloat uv)
{
    return (factorial(nm) / (factorial(ij) * factorial(nm - ij)))* pow(uv, ij)* pow(1 - uv, nm - ij);
}

void genBezier(GLint N, GLint M)
{
    Bezier.clear();
    xBezier.clear();
    yBezier.clear();
    zBezier.clear();

    for (GLfloat u = 0.0f; u <= 1.0f; u += 0.05f)
    {
        for (GLfloat v = 0.0f; v <= 1.0f; v += 0.05f)
        {
            for (GLint i = 0; i < N; i++)
            {
                for (GLint j = 0; j < M; j++)
                {
                    xBezier.push_back(B(N-1, i, u) * B(M-1, j, v) * controll_vertices[i * M + j][0]);
                    yBezier.push_back(B(N-1, i, u) * B(M-1, j, v) * controll_vertices[i * M + j][1]);
                    zBezier.push_back(B(N-1, i, u) * B(M-1, j, v) * controll_vertices[i * M + j][2]);
                }
            }

            Bezier.push_back({ std::accumulate(xBezier.begin(), xBezier.end(), 0.0f), std::accumulate(yBezier.begin(), yBezier.end(), 0.0f), std::accumulate(zBezier.begin(), zBezier.end(), 0.0f) });

            xBezier.clear();
            yBezier.clear();
            zBezier.clear();
        }
    }

    printf("\n\nBezier-felület pontjai:\n\n");
    for (auto v : Bezier)
    {
        
        printf("[%lf; %lf; %lf]\n", v[0], v[1], v[2]);
    }
}

int init()
{
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    g_window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL", NULL, NULL);

    if (g_window == NULL)
    {
        std::cout << "Failed to create GLFW window!" << std::endl;

        glfwTerminate();

        return -1;
    }

    printf("%d\n", factorial(200));

    glfwMakeContextCurrent(g_window);

    GLenum err = glewInit();
    if (GLEW_OK != err)
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));

    fprintf(stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

    genGrid(4,4);

    glGenBuffers(VBO, vertex_buffer_object);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object[0]);
    glBufferData(GL_ARRAY_BUFFER, controll_vertices.size() * sizeof(std::vector<GLfloat>), controll_vertices.data(), GL_STATIC_DRAW);


    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object[1]);
    
    glBufferData(GL_ARRAY_BUFFER, Bezier.size() * sizeof(std::vector<GLfloat>), Bezier.data(), GL_STATIC_DRAW);

    glGenVertexArrays(VAO, vertex_array_object);

    glBindVertexArray(vertex_array_object[0]);
    //glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object[1]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    parse_file_into_str("VertexShader.vert", vertex_shader, 1024 * 256);
    parse_file_into_str("FragmentShader.frag", fragment_shader, 1024 * 256);

    const GLchar* shader = (const GLchar*)vertex_shader;

    GLuint vertex_shader_object = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader_object, 1, &shader, NULL);

    glCompileShader(vertex_shader_object);

    glGetShaderiv(vertex_shader_object, GL_COMPILE_STATUS, &shader_compiled);
    if (shader_compiled != GL_TRUE)
    {
        glGetShaderInfoLog(vertex_shader_object, 1024, &log_length, message);
        fprintf(stderr, message);
        return -2;
    }

    shader = (const GLchar*)fragment_shader;

    GLuint fragment_shader_object = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader_object, 1, &shader, NULL);
    glCompileShader(fragment_shader_object);

    glGetShaderiv(fragment_shader_object, GL_COMPILE_STATUS, &shader_compiled);
    if (shader_compiled != GL_TRUE)
    {
        glGetShaderInfoLog(fragment_shader_object, 1024, &log_length, message);
        fprintf(stderr, message);
        return -3;
    }

    shader_program_object = glCreateProgram();

    glBindAttribLocation(shader_program_object, 0, "attrib_position"); 
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

    view_mat_location = glGetUniformLocation(shader_program_object, "view");
    proj_mat_location = glGetUniformLocation(shader_program_object, "proj");
    model_mat_location = glGetUniformLocation(shader_program_object, "model");
    vert_mat_location = glGetUniformLocation(shader_program_object, "vert");
    GLint vert_mat_fl_location = glGetUniformLocation(shader_program_object, "vert_fl");

    glUseProgram(shader_program_object);
    glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view_mat.m);
    glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, proj_mat);
    glUniformMatrix4fv(model_mat_location, 1, GL_FALSE, model_mat.m);
    glUniformMatrix4fv(vert_mat_location, 1, GL_FALSE, vert_mat.m);
    glUniformMatrix4fv(vert_mat_fl_location, 1, GL_FALSE, vert_mat_fl.m);

    glfwSetFramebufferSizeCallback(g_window, framebufferSizeCallback);

    return 0;
}

GLdouble deltaTime()
{
    static double previous_seconds = glfwGetTime();
    double current_seconds = glfwGetTime();
    double elapsed_seconds = current_seconds - previous_seconds;
    previous_seconds = current_seconds;
    return elapsed_seconds;
}

void mainRenderLoop()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);

    GLint surface_loc = glGetUniformLocation(shader_program_object, "surface");
    
    while (!glfwWindowShouldClose(g_window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, g_gl_width, g_gl_height);
        DT = deltaTime();

        glUseProgram(shader_program_object);

        if (cam_moved)
        {
            mat4 T = translate(identity_mat4(), vec3(-cam_pos[0], -cam_pos[1], -cam_pos[2]));
            mat4 R = rotate_y_deg(identity_mat4(), -cam_yaw);
            mat4 view_mat = R * T;
            glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view_mat.m);
        }

        glPointSize(20.0);


        /*TODO Fix This*/
        glBindVertexArray(vertex_array_object[0]);
        
        glUniform1i(surface_loc, 0);
        glEnableVertexAttribArray(0);
        for (int i = 0; i < controll_vertices.size(); i++)
        {
            glBufferData(GL_ARRAY_BUFFER, controll_vertices.size() * sizeof(GLfloat), controll_vertices[i].data(), GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object[0]);
            glDrawArrays(GL_POINTS, 0, controll_vertices[i].size());
        }

        glPointSize(5.0);

        glUniform1i(surface_loc, 1);
        glEnableVertexAttribArray(1);
        for (int i = 0; i < Bezier.size(); i++)
        {
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object[1]);
            glBufferData(GL_ARRAY_BUFFER, Bezier.size() * sizeof(GLfloat), Bezier[i].data(), GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object[1]);
            glDrawArrays(GL_POINTS, 0, Bezier[i].size());
        }

        glBindVertexArray(0);
        glUseProgram(0);

        keyCallBack();
        glfwSwapBuffers(g_window);
        glfwPollEvents();
    }

    Bezier.clear();
    glfwTerminate();
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    g_gl_height = height;
    g_gl_width = width;
    aspect = (float)width / (float)height;
    Sx = inverse_range / aspect;
    proj_mat[0] = Sx;
    glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, proj_mat);
}