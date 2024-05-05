#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp> 
#include "mesh.h"
#include "subdivision.h"
#include <iostream>
#include "wrapper.h"

int gWidth, gHeight;
glm::mat4 projectionMatrix;
glm::mat4 viewingMatrix;

bool animate = true;
float rot_cube = 0.0f;
float rot_tetra = 0.0f;
std::vector<SceneObject*> objects;
DrawMode draw_mode;

// Globals
int frameCount = 0;
double currentTime = 0.0;
double previousTime = 0.0;

void reshape(GLFWwindow* window, int w, int h)
{
    w = w < 1 ? 1 : w;
    h = h < 1 ? 1 : h;

    gWidth = w;
    gHeight = h;

    glViewport(0, 0, w, h);

    float fovyRad = (float)(45.0 / 180.0) * M_PI;
    projectionMatrix = glm::perspective(fovyRad, (float)w / h, 1.0f, 100.0f);

    // Assume default camera position and orientation (camera is at
    // (0, 0, 0) with looking at -z direction and its up vector pointing
    // at +y direction)

    viewingMatrix = glm::mat4(1);
    viewingMatrix = glm::translate(viewingMatrix, glm::vec3(0, -2, 0));
    viewingMatrix = glm::rotate(viewingMatrix, 0.25f, glm::vec3(1, 0, 0));
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if (key == GLFW_KEY_E && action == GLFW_PRESS)
    {
        for (auto& obj : objects) {
            if (obj->cur_level > 0) {
                obj->cur_level--;
            }
        }
    }
    if (key == GLFW_KEY_T && action == GLFW_PRESS)
    {
        for (auto& obj : objects) {
            if (obj->cur_level < MAX_SUBDIVIDE - 1) {
                obj->cur_level++;
            }
        }
    }
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        rot_cube = 0.0f;
        rot_tetra = 0.0f;
        for (auto& obj : objects) {
            obj->cur_level = 0;
        }
        objects[1]->cur_level = 1;
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        animate = !animate;
    }
    if (key == GLFW_KEY_M && action == GLFW_PRESS) {
        int tmp = draw_mode;
        tmp++;
        tmp %= 3;
        draw_mode = (DrawMode)tmp;
    }
}

// Function to update FPS
void updateFPS() {
    // Calculate time difference
    currentTime = glfwGetTime();
    double timeInterval = currentTime - previousTime;

    if (timeInterval > 1.0) { // Update FPS every second
        // Calculate FPS
        float fps = frameCount / timeInterval;

        // Output FPS to console
        std::cout << "FPS: " << fps << std::endl;

        // Reset variables
        previousTime = currentTime;
        frameCount = 0;
    }
}


int main(int argc, char** argv)
{
    GLFWwindow* window;
    if (!glfwInit())
    {
        exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    int width = 1280, height = 720;
    window = glfwCreateWindow(width, height, "CENG469 Catmull-Clark", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (GLEW_OK != glewInit())
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }

    glEnable(GL_DEPTH_TEST);

    std::cout << "Please wait, precomputing all subdivided meshes!" << std::endl;

    // init here
    SceneObject cube_obj;
    cube_obj.draw_data.color = glm::vec3(0.2, 0.9, 0.4);
    cube_obj.precompute(load_mesh("cube.obj"));
    objects.push_back(&cube_obj);

    std::cout << "Done cube" << std::endl;

    SceneObject tetra_obj;
    tetra_obj.draw_data.color = glm::vec3(0.8, 0.3, 0.4);
    tetra_obj.precompute(load_mesh("tetra.obj"));
    tetra_obj.cur_level = 1;
    objects.push_back(&tetra_obj);

    std::cout << "Done tetra" << std::endl;

    SceneObject cyl_obj;
    cyl_obj.draw_data.color = glm::vec3(0.3, 0.1, 0.8);
    cyl_obj.precompute(load_mesh("cylinder.obj"));
    objects.push_back(&cyl_obj);

    std::cout << "Done cylinder" << std::endl;

    Shader shader("example_diffuse.shader");

    glm::vec3 light_pos = glm::vec3(0, 2, -3);
    glm::vec3 camera_pos = glm::vec3(0, 1, 0);

    glfwSetKeyCallback(window, keyboard);
    glfwSetWindowSizeCallback(window, reshape);

    reshape(window, width, height);
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0, 0, 0, 1);
        glClearDepth(1.0f);
        glClearStencil(0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // scene
        shader.SetUniformMat4f("u_VP", projectionMatrix * viewingMatrix);

        // cube
        cube_obj.draw_data.transformation = glm::mat4(1.0f);
        cube_obj.draw_data.transformation = glm::translate(cube_obj.draw_data.transformation, glm::vec3(-0.5, 0, -5.5));
        cube_obj.draw_data.transformation = glm::rotate(cube_obj.draw_data.transformation, rot_cube, glm::vec3(1, 0, 0));
        cube_obj.draw_data.transformation = glm::scale(cube_obj.draw_data.transformation, glm::vec3(0.65, 0.65, 0.65));
        if (animate) rot_cube += 0.03f;

        // tetra
        tetra_obj.draw_data.transformation = glm::mat4(1.0f);
        tetra_obj.draw_data.transformation = glm::translate(tetra_obj.draw_data.transformation, glm::vec3(-1.5 + glm::sin(rot_tetra), -0.5, -8));
        tetra_obj.draw_data.transformation = glm::rotate(tetra_obj.draw_data.transformation, rot_tetra, glm::vec3(0, 1, 0));
        if (animate) rot_tetra += 0.02f;

        // cyl
        cyl_obj.draw_data.transformation = glm::mat4(1.0f);
        cyl_obj.draw_data.transformation = glm::translate(cyl_obj.draw_data.transformation, glm::vec3(1.5, 0.5, -6));
        cyl_obj.draw_data.transformation = glm::rotate(cyl_obj.draw_data.transformation, 0.35f, glm::vec3(1, 0, 0));

        for (const auto& obj : objects) {
            draw_model(obj->levels[obj->cur_level], shader, obj->draw_data, light_pos, camera_pos, draw_mode);
        }

        // Increment frame count
        frameCount++;

        glfwSwapBuffers(window);
        glfwPollEvents();

        // Update FPS
        updateFPS();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
