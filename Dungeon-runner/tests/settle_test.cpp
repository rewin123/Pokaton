
#include <ShaderSystem.h>
#include <WindowSystem.h>
#include <DrawText.h>

int main() {
    glfw_base_init();
    Window window(800, 600, "Dungeon runner");

    ShaderSystem shaders;

    GLuint text_shader = shaders.getShader("text");
    DrawTextSystem draw_text("res/fonts/kashima.otf");
    draw_text.SetSize(128,128);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (!glfwWindowShouldClose(window.window)) {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(text_shader);
        draw_text.DrawText(L"Привет мир!", glm::vec2(-1,0), 0.1f, text_shader);

        glfwSwapBuffers(window.window);
    }

    return 0;
}