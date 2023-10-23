#define		GLEW_STATIC
#include	<stdio.h>
#include    <iostream>
#include    <string>
#include    <windows.h>
#include	<GL/glew.h>
#include    <GL/GLU.h>
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
#include    <omp.h>
#include    "imGui/imgui.h"
#include    "imGui/imgui_impl_glfw.h"
#include    "imGui/imgui_impl_opengl3.h"
#pragma comment(lib, "glu32.lib")

#define VBO 6
#define VAO 2

extern "C"
{
    _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

void keyCallBack();
void mainRenderLoop();
int init();
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mousebuttonCallback(GLFWwindow* window, int button, int action, int mods);
void genBezier(int N, int M);
void genGrid(GLint N, GLint M);
void genSurface();
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
//
//GLint N = 6;
//GLint M = 5;
GLfloat step_u = 0.03;
GLfloat step_v = 0.03;

std::vector<GLfloat> K;
std::vector<GLfloat> J;
std::vector<GLfloat> xBezier;
std::vector<GLfloat> yBezier;
std::vector<GLfloat> zBezier;
std::vector<std::vector<GLfloat>> Bezier;
std::vector<GLfloat> sur_Bezier;
std::vector<GLfloat> norm_Bezier;

int    wireframe       = 0;
bool    surface         = false;
bool    cont_mesh       = true;
bool    cont_points     = true;
float   slider_x        = 0.0f;
float   slider_y        = 0.0f;
float   slider_z        = 0.0f;
int     slider_N        = 5;
int     slider_M        = 4;
int     slider_N_old    = 0;
int     slider_M_old    = 0;
float   slider_U        = 0.05;
float   slider_V        = 0.05;
float   slider_U_old    = 0.0f;
float   slider_V_old    = 0.0f;

GLfloat n = 0.1f; // near
GLfloat f = 100.0f; // far
GLfloat fov = 67.0f * ONE_DEG_IN_RAD;
GLfloat aspect = (GLfloat)g_gl_width / (GLfloat)g_gl_height;
GLfloat inverse_range = 1.0f / tan(fov * 0.5f);
GLfloat Sx = inverse_range / aspect;
GLfloat Sy = inverse_range;
GLfloat Sz = -(f + n) / (f - n);
GLfloat Pz = -(2.0f * f * n) / (f - n);
mat4    proj_mat = { Sx, 0.0f, 0.0f, 0.0f, 0.0f, Sy, 0.0f, 0.0f, 0.0f, 0.0f, Sz, -1.0f, 0.0f, 0.0f, Pz, 0.0f };
GLfloat cam_speed = 1.0f;
GLfloat cam_yaw_speed = 10.0f;
GLfloat cam_pos[]{ (GLfloat)(slider_N-1)/2.0f, (GLfloat)(slider_M-1)/2.0f, 5.0f };
GLfloat cam_yaw = 0.0f;
GLfloat rotateY = 0.0f;
GLfloat rotateX = 0.0f;
GLfloat cam_pitch = 0.0f;
GLboolean cam_moved = false;
GLboolean rotate_moved = false;
GLint d = 1;
GLdouble pos = 0.0;
GLdouble DT = 0.0;
mat4 T = translate(identity_mat4(), vec3(-cam_pos[0], -cam_pos[1], -cam_pos[2]));
mat4 R = rotate_y_deg(identity_mat4(), -cam_yaw);
mat4 view_mat = R * T;
mat4 model_mat = rotate_y_deg(identity_mat4(), ONE_DEG_IN_RAD * 100);

mat4 vert_mat = translate(identity_mat4(), vec3(0.0, 0.0, 0.0));
mat4 vert_mat_fl = translate(identity_mat4(), vec3(0.0, -2.0, 0.0));

std::vector<GLfloat> light_pos;

GLint view_mat_location = 0;
GLint proj_mat_location = 0;
GLint model_mat_location = 0;
GLint vert_mat_location = 0;
GLint norm_Bezier_location = 0;
GLint light_pos_location = 0;

int rot = 0;

int main()
{
    T * R;
    int ret = init();

    if (ret != 0)
    {
        glfwTerminate();
        return ret;
    }

    mainRenderLoop();

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
        cam_pos[2] += cam_speed * DT;;
        cam_moved = true;
    }
    if (glfwGetKey(g_window, GLFW_KEY_Q))
    {
        cam_pos[1] += cam_speed * DT;
        cam_moved = true;
    }
    if (glfwGetKey(g_window, GLFW_KEY_E))
    {
        cam_pos[1] -= cam_speed * DT;
        cam_moved = true;
    }
    if (glfwGetKey(g_window, GLFW_KEY_LEFT))
    {
        rotateY = rotateY + 1.0f;
        rotate_moved = true;
    }
    if (glfwGetKey(g_window, GLFW_KEY_RIGHT))
    {
        rotateY = rotateY - 1.0f;
        rotate_moved = true;
    }
    if (glfwGetKey(g_window, GLFW_KEY_UP))
    {
        rotateX+= 1.0f;
        rotate_moved = true;
    }
    if (glfwGetKey(g_window, GLFW_KEY_DOWN))
    {
        rotateX -= 1.0f;
        rotate_moved = true;
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

/* MxN - es kontroll háló legenerálása */
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
            if ((i == 0.0f || j == 0.0f) || (i == N - 1 || j == M - 1))
            {
                r = 0.0;
            }
            else
            {
                r = 1.0;
            }

            controll_vertices.push_back({ i,j,r });
            printf("[%lf; %lf; %lf]\n", i, j, r);
        }
    }

    genBezier(N, M);
}

GLfloat B(GLint n_m, GLint i_j, GLfloat u_v)
{
    return (factorial(n_m) / (factorial(i_j) * factorial(n_m - i_j))) * pow(u_v, i_j) * pow(1 - u_v, n_m - i_j);
}

int num_V;
int num_U;

void genBezier(GLint N, GLint M)
{
    Bezier.clear();
    xBezier.clear();
    yBezier.clear();
    zBezier.clear();
    num_U = 0;


    for (GLfloat u = 0.0f; u <= 1.0f; u += slider_U)
    {
        num_U++;
        num_V = 0;
        for (GLfloat v = 0.0f; v <= 1.0f; v += slider_V)
        {
            num_V++;
            for (GLint i = 0; i < N; i++)
            {
                for (GLint j = 0; j < M; j++)
                {
                    xBezier.push_back(B(N - 1, i, u) * B(M - 1, j, v) * controll_vertices[i * M + j][0]);
                    yBezier.push_back(B(N - 1, i, u) * B(M - 1, j, v) * controll_vertices[i * M + j][1]);
                    zBezier.push_back(B(N - 1, i, u) * B(M - 1, j, v) * controll_vertices[i * M + j][2]);
                }
            }
            
            Bezier.push_back({ std::accumulate(xBezier.begin(), xBezier.end(), 0.0f), std::accumulate(yBezier.begin(), yBezier.end(), 0.0f), std::accumulate(zBezier.begin(), zBezier.end(), 0.0f) });

            xBezier.clear();
            yBezier.clear();
            zBezier.clear();
        }
    }

    //printf("\n\nBezier-felület pontjai:\n\n");
    genSurface();
    //for (auto v : Bezier)
    //{

    //    printf("[%lf; %lf; %lf]\n", v[0], v[1], v[2]);
    //}
}

int init()
{
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    g_window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "GEOM Project", NULL, NULL);

    if (g_window == NULL)
    {
        std::cout << "Failed to create GLFW window!" << std::endl;

        glfwTerminate();

        return -1;
    }

    glfwMakeContextCurrent(g_window);

    GLenum err = glewInit();
    if (GLEW_OK != err)
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));

    fprintf(stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsLight();

    ImGui_ImplGlfw_InitForOpenGL(g_window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    light_pos.push_back(1.0f);
    light_pos.push_back(3.0f);
    light_pos.push_back(5.0f);

    glGenBuffers(VBO, vertex_buffer_object);
    
    glGenVertexArrays(VAO, vertex_array_object);

    glBindVertexArray(vertex_array_object[0]);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object[1]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object[2]);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object[3]);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object[4]);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object[5]);
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

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
    norm_Bezier_location = glGetUniformLocation(shader_program_object, "surfaceNormal");
    light_pos_location = glGetUniformLocation(shader_program_object, "light_pos");

    glUseProgram(shader_program_object);
    glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view_mat.m);
    glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, proj_mat.m);
    glUniformMatrix4fv(model_mat_location, 1, GL_FALSE, model_mat.m);
    glUniformMatrix4fv(vert_mat_location, 1, GL_FALSE, vert_mat.m);
    glUniformMatrix4fv(vert_mat_fl_location, 1, GL_FALSE, vert_mat_fl.m);

    glfwSetFramebufferSizeCallback(g_window, framebufferSizeCallback);
    glfwSetMouseButtonCallback(g_window, mousebuttonCallback);

    return 0;
}

void genSurface()
{
    //std::vector<GLfloat> sur_Bezier;

    int br = 1;

    sur_Bezier.clear();

    for (int i = 0; i < Bezier.size() - num_V; i++)
    {
        if (br == num_V)
        {
            br = 1;
            continue;
        }

        vec3 A = vec3(Bezier[i][0], Bezier[i][1], Bezier[i][2]);
        vec3 B = vec3(Bezier[i + 1][0], Bezier[i + 1][1], Bezier[i + 1][2]);
        vec3 C = vec3(Bezier[i + num_V][0], Bezier[i + num_V][1], Bezier[i + num_V][2]);

        sur_Bezier.push_back((GLfloat)A.v[0]);
        sur_Bezier.push_back((GLfloat)A.v[1]);
        sur_Bezier.push_back((GLfloat)A.v[2]);

        sur_Bezier.push_back((GLfloat)B.v[0]);
        sur_Bezier.push_back((GLfloat)B.v[1]);
        sur_Bezier.push_back((GLfloat)B.v[2]);

        sur_Bezier.push_back((GLfloat)C.v[0]);
        sur_Bezier.push_back((GLfloat)C.v[1]);
        sur_Bezier.push_back((GLfloat)C.v[2]);

        vec3 AB = B - A;
        vec3 AC = C - A;

        vec3 c = cross(AB, AC);

        norm_Bezier.push_back((GLfloat)c.v[0]);
        norm_Bezier.push_back((GLfloat)c.v[1]);
        norm_Bezier.push_back((GLfloat)c.v[2]);

        A = vec3(Bezier[i + 1][0], Bezier[i + 1][1], Bezier[i + 1][2]);
        B = vec3(Bezier[i + num_V + 1][0], Bezier[i + num_V + 1][1], Bezier[i + num_V + 1][2]);
        C = vec3(Bezier[i + num_V][0], Bezier[i + num_V][1], Bezier[i + num_V][2]);

        sur_Bezier.push_back((GLfloat)A.v[0]);
        sur_Bezier.push_back((GLfloat)A.v[1]);
        sur_Bezier.push_back((GLfloat)A.v[2]);

        sur_Bezier.push_back((GLfloat)B.v[0]);
        sur_Bezier.push_back((GLfloat)B.v[1]);
        sur_Bezier.push_back((GLfloat)B.v[2]);

        sur_Bezier.push_back((GLfloat)C.v[0]);
        sur_Bezier.push_back((GLfloat)C.v[1]);
        sur_Bezier.push_back((GLfloat)C.v[2]);

        AB = B - A;
        AC = C - A;

        c = cross(AB, AC);

        norm_Bezier.push_back((GLfloat)c.v[0]);
        norm_Bezier.push_back((GLfloat)c.v[1]);
        norm_Bezier.push_back((GLfloat)c.v[2]);

        br++;
    }
}

/*TODO*/
std::vector<GLfloat> genWireframe()
{
    std::vector<GLfloat> tmp;

    int br = 1;

    for (int i = 0; i < Bezier.size() - num_V; i++)
    {
        tmp.push_back(Bezier[i][0]);
        tmp.push_back(Bezier[i][1]);
        tmp.push_back(Bezier[i][2]);

        tmp.push_back(Bezier[i + num_V][0]);
        tmp.push_back(Bezier[i + num_V][1]);
        tmp.push_back(Bezier[i + num_V][2]);
    }

    for (auto row = Bezier.begin(); row != Bezier.end() - 1; row++)
    {
        
        if (br == std::ceil(1.0 / slider_V))
        {
            br = 1;
            continue;
        }

        tmp.push_back(row[0][0]);
        tmp.push_back(row[0][1]);
        tmp.push_back(row[0][2]);

        tmp.push_back(std::next(row)[0][0]);
        tmp.push_back(std::next(row)[0][1]);
        tmp.push_back(std::next(row)[0][2]);
        
        br++;
    }

    return tmp;
}

bool credits = false;
bool show_window = true;

float selectedPointX = 0;
float selectedPointY = 0;
float selectedPointZ = 0;

void mainRenderLoop()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glMatrixMode(GL_MODELVIEW | GL_PROJECTION);

    GLint surface_loc = glGetUniformLocation(shader_program_object, "surface");

    std::vector<GLfloat> cont_v_tmp;
    std::vector<GLfloat> Bez_v_tmp;
    GLint temp = 1;

    genGrid(slider_N, slider_M);

    while (!glfwWindowShouldClose(g_window))
    {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, g_gl_width, g_gl_height);
        glShadeModel(GL_FLAT);

        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        /*Menu Window*/
        {
            ImGui::Begin("Menu");

            ImGui::Checkbox("Controll Mesh", &cont_mesh);
            ImGui::Checkbox("Controll Points", &cont_points);

            ImGui::RadioButton("Wireframe", &wireframe, 0);
            ImGui::RadioButton("Surface", &wireframe, 1);
            //ImGui::Checkbox("Wireframe", &wireframe);
            //ImGui::Checkbox("Surface", &surface);

            ImGui::SliderInt("N", &slider_N, 4, 10);
            ImGui::SliderInt("M", &slider_M, 4, 10);

            ImGui::CaptureKeyboardFromApp(false);
            ImGui::SliderFloat("U", &slider_U, 0.1, 0.01);
            ImGui::SliderFloat("V", &slider_V, 0.1, 0.01);

            ImGui::SliderFloat("Light X", &light_pos[0], 1, 10);
            ImGui::SliderFloat("Light Y", &light_pos[1], 1, 10);
            ImGui::SliderFloat("Light Z", &light_pos[2], 1, 10);

            /*Külön ablakban, a pont kiválasztása után*/
            //ImGui::InputFloat("Kiv. pont X poz:", &selectedPointX);
            //ImGui::InputFloat("Kiv. pont Y poz:", &selectedPointY);
            //ImGui::InputFloat("Kiv. pont Z poz:", &selectedPointZ);

            // Button to trigger action
            if (ImGui::Button("Rendben")) {
                // Handle button click event
                std::cout << "X Value entered: " << selectedPointX << std::endl;
                std::cout << "Y Value entered: " << selectedPointY << std::endl;
                std::cout << "Z Value entered: " << selectedPointZ << std::endl;
            }


            ImGui::End();
        }

        if (slider_U == 0.0) slider_U = 1.0;
        if (slider_V == 0.0) slider_V = 1.0;
        if (slider_M == 0) slider_M = 1;
        if (slider_N == 0) slider_N = 1;

        if (slider_M != slider_M_old || slider_N != slider_N_old)
        {
            int br = 1;
            genGrid(slider_N, slider_M);
            slider_M_old = slider_M;
            slider_N_old = slider_N;

            cont_v_tmp.clear();

            for (int i = 0; i < controll_vertices.size(); i++)
            {

                if (br == slider_M)
                {
                    br = 1;

                    continue;
                }

                cont_v_tmp.push_back(controll_vertices[i][0]);
                cont_v_tmp.push_back(controll_vertices[i][1]);
                cont_v_tmp.push_back(controll_vertices[i][2]);

                cont_v_tmp.push_back(controll_vertices[i + 1][0]);
                cont_v_tmp.push_back(controll_vertices[i + 1][1]);
                cont_v_tmp.push_back(controll_vertices[i + 1][2]);

                br++;
            }

            for (int i = 0; i < controll_vertices.size() - slider_M; i++)
            {
                cont_v_tmp.push_back(controll_vertices[i][0]);
                cont_v_tmp.push_back(controll_vertices[i][1]);
                cont_v_tmp.push_back(controll_vertices[i][2]);

                cont_v_tmp.push_back(controll_vertices[i + slider_M][0]);
                cont_v_tmp.push_back(controll_vertices[i + slider_M][1]);
                cont_v_tmp.push_back(controll_vertices[i + slider_M][2]);
            }

            Bez_v_tmp = genWireframe();
        }

        if (slider_U != slider_U_old || slider_V != slider_V_old)
        {
            genBezier(slider_N, slider_M);
            slider_U_old = slider_U;
            slider_V_old = slider_V;

            Bez_v_tmp = genWireframe();

           //Bez_v_tmp.push_back(0.0f);
        }
        

        DT = deltaTime();

        glUseProgram(shader_program_object);

        if (cam_moved)
        {
            mat4 T = translate(identity_mat4(), vec3(-cam_pos[0], -cam_pos[1], -cam_pos[2]));
            mat4 R = rotate_y_deg(identity_mat4(), -cam_yaw);

            mat4 view_mat = R * T;
            glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view_mat.m);
        }

        if (rotate_moved)
        {
            mat4 RY = rotate_y_deg(identity_mat4(), rotateY);
            mat4 RX = rotate_x_deg(identity_mat4(), rotateX);
            mat4 Tto = translate(identity_mat4(), vec3(2.0f, 2.0f, 2.0f));
            mat4 Tback = translate(identity_mat4(), vec3(-2.0f, -2.0f, -2.0f));

            mat4 model_mat = identity_mat4(); // Initialize with identity matrix
            model_mat = model_mat * Tto *  RY * RX * Tback; // Combine transformations

            glUniformMatrix4fv(model_mat_location, 1, GL_FALSE, model_mat.m);
        }
        
        glPointSize(10.0);

        /*TODO: Fix This*/  
        glBindVertexArray(vertex_array_object[0]);

        if (cont_points)
        {
            glUniform1i(surface_loc, 0);
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object[0]);
            for (int i = 0; i < controll_vertices.size(); i++)
            {
                glBufferData(GL_ARRAY_BUFFER, controll_vertices[i].size() * sizeof(GLfloat), controll_vertices[i].data(), GL_STATIC_DRAW);
                glDrawArrays(GL_POINTS, 0, controll_vertices[i].size());
            }
        }

        glLineWidth(30.0);

        if (cont_mesh)
        {
            glUniform1i(surface_loc, 2);
            glEnableVertexAttribArray(2);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object[2]);

            glBufferData(GL_ARRAY_BUFFER, cont_v_tmp.size() * sizeof(GLfloat), cont_v_tmp.data(), GL_STATIC_DRAW);
            glDrawArrays(GL_LINES, 0, cont_v_tmp.size());
        }

        if (wireframe == 0)
        {
            glUniform1i(surface_loc, 1);
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object[1]);
            glBufferData(GL_ARRAY_BUFFER, Bez_v_tmp.size() * sizeof(GLfloat), Bez_v_tmp.data(), GL_STATIC_DRAW);
            glDrawArrays(GL_LINES, 0, Bez_v_tmp.size());
        }

        if (wireframe == 1)
        {
            glUniform1i(surface_loc, 3);
            glEnableVertexAttribArray(3);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object[3]);
            glBufferData(GL_ARRAY_BUFFER, sur_Bezier.size() * sizeof(GLfloat), sur_Bezier.data(), GL_STATIC_DRAW);
            glDrawArrays(GL_TRIANGLES, 0, sur_Bezier.size());

            glEnableVertexAttribArray(4);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object[4]);
            glBufferData(GL_ARRAY_BUFFER, norm_Bezier.size() * sizeof(GLfloat), norm_Bezier.data(), GL_DYNAMIC_READ);

            glEnableVertexAttribArray(5);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object[5]);
            glBufferData(GL_ARRAY_BUFFER, light_pos.size() * sizeof(GLfloat), light_pos.data(), GL_DYNAMIC_READ);
        }


        glBindVertexArray(0);
        glUseProgram(0);


        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        keyCallBack();
        glfwSwapBuffers(g_window);
    }

    Bezier.clear();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(g_window);
    glfwTerminate();
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    g_gl_height = height;
    g_gl_width = width;
    aspect = (float)width / (float)height;
    Sx = inverse_range / aspect;
    proj_mat.m[0] = Sx;
    glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, proj_mat.m);
}

void mousebuttonCallback(GLFWwindow* window, int button, int action, int mods)
{
    ImGuiIO& io = ImGui::GetIO();

    io.AddMouseButtonEvent(button, action == GLFW_PRESS);

    if (!io.WantCaptureMouse)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        {
            double xpos, ypos;
            //getting cursor position
            glfwGetCursorPos(window, &xpos, &ypos);
            std::cout << "Cursor Position at (" << xpos << " : " << ypos << ")" << std::endl;

            GLdouble  m[16];
            GLdouble  p[16];
            GLint     v[4];
            mat4 t;

            vec3 tv;

           

            GLdouble objX;
            GLdouble objY;
            GLdouble objZ;           

            glGetIntegerv(GL_VIEWPORT, v);
            //glGetDoublev(GL_MODELVIEW_MATRIX, m);
            //glGetDoublev(GL_PROJECTION_MATRIX, p);

            GLfloat winX = (float)xpos;
            GLfloat winY = (float)v[3] - (float)ypos;
            GLfloat winZ;

            glReadPixels(xpos, (int)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

            
            for (int i = 0; i < 16; i++)
            {
                m[i] = model_mat.m[i];
            }
            
            for (int i = 0; i < 16; i++)
            {
                p[i] = proj_mat.m[i];
            }

            gluUnProject(xpos, ypos, winZ, m, p, v, &objX, &objY, &objZ);

            printf("%lf %lf, %lf\n", objX, objY, objZ);
        }
    }
}