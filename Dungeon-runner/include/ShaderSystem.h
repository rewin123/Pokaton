//
// Created by rewin on 11.05.2020.
//

#ifndef DUNGEON_RUNNER_SHADERSYSTEM_H
#define DUNGEON_RUNNER_SHADERSYSTEM_H

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <dirent.h>
#include <glad/glad.h>
#include <map>

#define SHADER_STORAGE_PATH "res/shaders/"

std::vector<std::string> get_directories(std::string path) {
    DIR *dr = opendir(path.c_str());

    if(dr == NULL) {
        return std::vector<std::string>();
    }

    struct dirent *de;

    std::vector<std::string> res;
    while ((de = readdir(dr)) != NULL)
        res.push_back(de->d_name);

    closedir(dr);

    for(int i = 0;i < res.size();i++) {
        if(res[i] == "." || res[i] == "..") {
            res.erase(res.begin() + i, res.begin() + i + 1);
            i--;
            continue;
        }
    }
    return res;
}

std::string readAllText(std::string path) {
    std::ifstream ifs(path.c_str());
    std::string content((std::istreambuf_iterator<char>(ifs)),
                        (std::istreambuf_iterator<char>()));
    return content;
}

void testShaderCompile(GLuint shader) {
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
}

void testShaderLink(GLuint shader) {
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
}

GLuint getShaderRaw(std::string vertex_path, std::string fragment_path) {
    std::string vertex = readAllText(vertex_path);
    std::string fragment = readAllText(fragment_path);

    const GLchar* vc = vertex.c_str();
    const GLchar* fc = fragment.c_str();

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vc, NULL);
    glCompileShader(vs);
    testShaderCompile(vs);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fc, NULL);
    glCompileShader(fs);
    testShaderCompile(fs);
    auto shader = glCreateProgram();
    glAttachShader(shader, vs);
    glAttachShader(shader, fs);
    glLinkProgram(shader);
    testShaderLink(shader);
    glDeleteShader(vs);
    glDeleteShader(fs);
    return shader;
}

class ShaderSystem {
private:
    std::vector<std::string> shader_names;
    std::map<std::string, GLuint> compiled_shaders;
    std::map<std::string, bool> is_compiled;
public:
    ShaderSystem() {
        shader_names = get_directories(SHADER_STORAGE_PATH);
    }

    void PrintShaderNames() {
        std::cout << "Shaders: " << std::endl;
        for(int i = 0;i < shader_names.size();i++)
            std::cout << shader_names[i] << std::endl;
    }

    GLuint getShader(std::string name) {
        if(is_compiled[name]) {
            return compiled_shaders[name];
        } else {
            GLuint shader = getShaderRaw(SHADER_STORAGE_PATH + name + "/vertex.glsl",
                                         SHADER_STORAGE_PATH + name + "/fragment.glsl");
            compiled_shaders[name] = shader;
            is_compiled[name] = true;

            return shader;
        }
    }

    ~ShaderSystem() {
        for(int i = 0;i < shader_names.size();i++) {
            if(is_compiled[shader_names[i]]) {
                glDeleteProgram(compiled_shaders[shader_names[i]]);
            }
        }
    }
};

#endif //DUNGEON_RUNNER_SHADERSYSTEM_H
