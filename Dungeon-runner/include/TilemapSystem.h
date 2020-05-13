//
// Created by rewin on 12.05.2020.
//

#ifndef DUNGEON_RUNNER_TILEMAPSYSTEM_H
#define DUNGEON_RUNNER_TILEMAPSYSTEM_H

#include <ShaderSystem.h>
#include <string>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <SOIL/SOIL.h>

GLuint load_tex(std::string path, GLuint filter = GL_NEAREST) {
    int width, height;
    unsigned char* image = SOIL_load_image(path.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

class GUARDED_TEXTURE {
    GLuint *texture;
public:
    GUARDED_TEXTURE(GLuint *texture) {
        this->texture = texture;
    }

    ~GUARDED_TEXTURE() {
        glDeleteTextures(1, texture);
    }
};

#define DECLARE_GUARDED_TEXTURE(name) GLuint name; GUARDED_TEXTURE name##_guarded_tex(&name);
#define LOAD_GUARDED_TEXTURE(name, path) DECLARE_GUARDED_TEXTURE(name); name = load_tex(path);

class SimpleTexDraw {
private:

    GLuint VBO;
    GLuint VAO;
    GLuint shader;
public:
    SimpleTexDraw(ShaderSystem &shaders) {
        GLfloat quad_verts[] = {
                -1, -1, 0, 1,
                -1, 1, 0, 0,
                1, -1, 1, 1,

                1, -1, 1, 1,
                -1, 1, 0, 0,
                1, 1, 1, 0,
        };

        glGenBuffers(1, &VBO);
        glGenVertexArrays(1, &VAO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad_verts), quad_verts, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

        shader = shaders.getShader("simple_texture");
    }

    void draw(GLuint texture) {
        glUseProgram(shader);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);

        glm::vec2 pos(0, 0);
        glUniform2fv(glGetUniformLocation(shader, "pos"), 1, glm::value_ptr(pos));
        glUniform1f(glGetUniformLocation(shader, "scale_x"),1);
        glUniform1f(glGetUniformLocation(shader, "scale_y"),1);

        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        glUseProgram(0);
    }

    ~SimpleTexDraw() {
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);
    }
};

struct CameraInfo {
    float x, y, w, h;

    glm::mat4 getProjection() {
        return glm::ortho(x - w / 2.f, x + w / 2.f,
                          y - h / 2.f, y + h / 2.f, -10.f, 10.0f );
    }
};

class SpriteRenderer {
private:
    GLuint VBO;
    GLuint VAO;
    GLuint shader;
    CameraInfo *camera;
public:
    SpriteRenderer(ShaderSystem &shaders, CameraInfo &camera) {
        shader = shaders.getShader("sprite");
        this->camera = &camera;

        GLfloat quad_verts[] = {
                0, 0, 0, 1,
                0, 1, 0, 0,
                1, 0, 1, 1,

                1, 0, 1, 1,
                0, 1, 0, 0,
                1, 1, 1, 0,
        };

        glGenBuffers(1, &VBO);
        glGenVertexArrays(1, &VAO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad_verts), quad_verts, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }

    void Draw(GLuint texture, glm::vec2 position, glm::vec2 size, float rotate, glm::vec3 color) {
        glUseProgram(shader);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(position, 0.0f));

        model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
        model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

        model = glm::scale(model, glm::vec3(size, 1.0f));

        auto proj = camera->getProjection();

        auto model_id = glGetUniformLocation(shader, "model");
        auto spriteColor_id = glGetUniformLocation(shader, "spriteColor");
        auto projection_id = glGetUniformLocation(shader, "projection");

        glUniformMatrix4fv(model_id, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(projection_id, 1, GL_FALSE, glm::value_ptr(proj));
        glUniform3fv(spriteColor_id, 1, glm::value_ptr(color));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    ~SpriteRenderer() {
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);
    }
};

std::vector<std::string> getFilesInDir(std::string path) {
    DIR *dir;
    struct dirent *ent;

    std::vector<std::string> res;

    if ((dir = opendir (path.c_str())) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) {
            res.push_back(ent->d_name);
        }
        closedir (dir);
    } else {

    }
    return res;
}

class TileAtlas {
    std::vector<std::string> names;
    std::string path;
public:
    TileAtlas(std::string path) {
        this->path = path;
        names = getFilesInDir(path);
        for(int i = 0;i < names.size();i++) {
            if(names[i].find(".png")==std::string::npos) {
                names.erase(names.begin() + i, names.begin() + i + 1);
                i--;
                continue;
            }
        }

    }

    void printTileNames() {
        std::cout << "Tile names in atlas: " << std::endl;
        for(int i = 0; i < names.size();i++) {
            std::cout << names[i] << std::endl;
        }
    }
};

class Tilemap {

};

#endif //DUNGEON_RUNNER_TILEMAPSYSTEM_H
