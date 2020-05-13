#include <ShaderSystem.h>
#include <WindowSystem.h>
#include <DrawText.h>
#include <TilemapSystem.h>

int main() {
    glfw_base_init();
    Window window(800, 600, "Dungeon runner");

    ShaderSystem shaders;

    GLuint text_shader = shaders.getShader("text");
    DrawTextSystem draw_text("res/fonts/kashima.otf");
    draw_text.SetSize(64,64);

    LOAD_GUARDED_TEXTURE(rock_floor, "res/sprites/main_tile_atlas/rock_floor.png");
    LOAD_GUARDED_TEXTURE(rock, "res/sprites/main_tile_atlas/rock.png");

    SimpleTexDraw tex_draw(shaders);
    CameraInfo camera;
    camera.x = 0;
    camera.y = 0;
    camera.w = 10;
    camera.h = 600.f / 800.f * camera.w;
    SpriteRenderer sprite_render(shaders, camera);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float rot = 0;

    float index = 0;

    while (!glfwWindowShouldClose(window.window)) {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);

        sprite_render.Draw(rock, glm::vec2(0,0), glm::vec2(1,1), rot, glm::vec3(1,1,1));
        sprite_render.Draw(rock, glm::vec2(1,0), glm::vec2(1,1), rot, glm::vec3(1,1,1));
        sprite_render.Draw(rock, glm::vec2(0,1), glm::vec2(1,1), rot, glm::vec3(1,1,1));
        sprite_render.Draw(rock, glm::vec2(-1,-1), glm::vec2(1,1), rot, glm::vec3(1,1,1));

        sprite_render.Draw(rock_floor, glm::vec2(-2,-1), glm::vec2(1,1), rot, glm::vec3(1,1,1));

        index++;
        rot = std::sin(index / 6.f) * 3.14f * 2.f;
//        glUseProgram(text_shader);
//        draw_text.DrawText(L"Привет мир!", glm::vec2(-0.5f,0), 0.2f, text_shader);

        glfwSwapBuffers(window.window);
    }

    return 0;
}