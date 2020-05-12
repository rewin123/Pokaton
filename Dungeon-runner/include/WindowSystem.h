//
// Created by rewin on 11.05.2020.
//

#ifndef DUNGEON_RUNNER_WINDOWSYSTEM_H
#define DUNGEON_RUNNER_WINDOWSYSTEM_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

void glfw_base_init() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //Минорная
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //Установка профайла для которого создается контекст
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}


class Window {
private:
    int width, height;
public:
    GLFWwindow *window;
    Window(int width, int height, std::string name) {
        this->width = width;
        this->height = height;

        window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return;
        }

        glViewport(0, 0, width, height);
    }

    ~Window() {
        glfwTerminate();
    }
};

#endif //DUNGEON_RUNNER_WINDOWSYSTEM_H
