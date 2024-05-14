#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp> 
#include <glm/ext.hpp>
#include "mesh.h"
#include <iostream>
#include "wrapper.h"

int gWidth, gHeight;
glm::mat4 projectionMatrix;
glm::mat4 viewingMatrix;

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
}

int main(int argc, char** argv)
{
    GLFWwindow* window;
    int width = 1280, height = 720;

    if (!glfwInit())
    {
        exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, "CENG469 Median Cut & Cubemap", NULL, NULL);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glewInit();

    glEnable(GL_DEPTH_TEST);

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

        //for (const auto& obj : objects) {
        //    draw_model(obj->levels[obj->cur_level], shader, obj->draw_data, light_pos, camera_pos, draw_mode);
        //}

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
