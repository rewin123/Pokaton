//
// Created by rewin on 11.05.2020.
//

#ifndef DUNGEON_RUNNER_DRAWTEXT_H
#define DUNGEON_RUNNER_DRAWTEXT_H

#include <ShaderSystem.h>
#include <string>
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stdio.h"

#define CATCH_CALL(code) if(code != 0) { std::cout << "Error in " << #code << " in line " << __LINE__  << std::endl; return;}

template<typename T> struct TextureMapStorage {
    std::map<T, GLuint> data;

    void clear() {
        for (auto it = data.begin(); it != data.end(); it++ )
        {
            glDeleteTextures(1, &it->second);
        }
        data.clear();
    }

    ~TextureMapStorage() {
        for (auto it = data.begin(); it != data.end(); it++ )
        {
            glDeleteTextures(1, &it->second);
        }
    }
};

struct TexChar {
    GLuint texture;
    glm::ivec2 Size;
    glm::ivec2 Bearing;
    GLuint Advance;
};

class DrawTextSystem {
private:
    FT_Library ft;
    FT_Face face;
    FT_Int32 load_flags = FT_LOAD_DEFAULT;
    FT_Render_Mode  render_flags = FT_RENDER_MODE_NORMAL;

    int w = -1;
    int h = -1;

    std::map<FT_ULong, TexChar> textures;
    std::map<FT_ULong, bool> is_load_tex;

    GLuint VBO;
    GLuint VAO;
public:
    DrawTextSystem(std::string font_path) {
        CATCH_CALL(FT_Init_FreeType(&ft));
        CATCH_CALL(FT_New_Face(ft, font_path.c_str(), 0, &face));

        GLfloat quad_verts[] = {
                0, 0,
                0, 1,
                1, 0,

                1, 0,
                0, 1,
                1, 1,
        };

        glGenBuffers(1, &VBO);
        glGenVertexArrays(1, &VAO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad_verts), quad_verts, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);
    }

    void DeleteTextures() {
        for (auto it = textures.begin(); it != textures.end(); it++ )
        {
            glDeleteTextures(1, &it->second.texture);
        }
        textures.clear();
        is_load_tex.clear();
    }

    void SetSize(FT_UInt width, FT_UInt height) {
        if(w != width || h != height) {
            DeleteTextures();
            FT_Set_Pixel_Sizes(face, width, height);
            w = width;
            h = height;
        }
    }

    TexChar GetTexChar(FT_ULong charcode) {
        if(is_load_tex[charcode]) {
            return textures[charcode];
        } else {
            FT_UInt glyph_index = FT_Get_Char_Index(face, charcode);
            FT_Load_Glyph(face, glyph_index, load_flags);
            FT_Render_Glyph(face->glyph, render_flags);

            GLuint tex_id;

            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glGenTextures(1, &tex_id);
            glBindTexture(GL_TEXTURE_2D, tex_id);
            glTexImage2D(GL_TEXTURE_2D,
                    0,
                    GL_RED,
                    face->glyph->bitmap.width,
                    face->glyph->bitmap.rows,
                    0,
                    GL_RED,
                    GL_UNSIGNED_BYTE,
                    face->glyph->bitmap.buffer);

            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glBindTexture(GL_TEXTURE_2D, 0);

            TexChar res;
            res.texture = tex_id;
            res.Size = glm::ivec2(face->glyph->bitmap.width,
                    face->glyph->bitmap.rows);
            res.Bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
            res.Advance = face->glyph->advance.x;

            is_load_tex[charcode] = true;
            textures[charcode] = res;
            return res;
        }

    }

    void DrawText(std::wstring text, glm::vec2 pos, float scale, GLuint shader) {
        GLfloat x = pos.x;

        for(int i = 0;i < text.size();i++) {
            FT_ULong charcode = text[i];
            if(charcode == L' ') {
                x = x + scale / 2;
                continue;
            }
            TexChar tex = GetTexChar(charcode);

            GLfloat loc_x = x + tex.Bearing.x * scale / w;
            GLfloat loc_y = pos.y - (tex.Size.y - tex.Bearing.y) * scale / h;

            x = x + (float)(tex.Advance >> 6) * scale / w;

            glm::vec2 pos(loc_x, loc_y);
            glUniform2fv(glGetUniformLocation(shader, "pos"), 1, glm::value_ptr(pos));
            glUniform1f(glGetUniformLocation(shader, "scale_x"),scale * tex.Size.x / w);
            glUniform1f(glGetUniformLocation(shader, "scale_y"),scale * tex.Size.y / h);

            glBindTexture(GL_TEXTURE_2D, tex.texture);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }


        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void PrintText(std::wstring text) {
        for(int i = 0;i < text.size();i++) {
            FT_ULong charcode = text[i];
            FT_UInt glyph_index = FT_Get_Char_Index(face, charcode);
            FT_Load_Glyph(face, glyph_index, load_flags);
            FT_Render_Glyph(face->glyph, render_flags);

            if(charcode == L' ') {
                for (size_t i = 0; i < w; i++) {
                    for (size_t j = 0; j < h; j++) {
                        printf(" ");
                    }
                    printf("\n");
                }
            } else {
                for (size_t i = 0; i < face->glyph->bitmap.rows; i++) {
                    for (size_t j = 0; j < face->glyph->bitmap.width; j++) {
                        unsigned char pixel_brightness =
                                face->glyph->bitmap.buffer[i * face->glyph->bitmap.pitch + j];

                        if (pixel_brightness > 169) {
                            printf("*");
                        } else if (pixel_brightness > 84) {
                            printf(".");
                        } else {
                            printf(" ");
                        }
                    }
                    printf("\n");
                }
            }
        }
    }



    ~DrawTextSystem() {
        DeleteTextures();
        FT_Done_Face(face);
        FT_Done_FreeType(ft);

        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);
    }
};

#endif //DUNGEON_RUNNER_DRAWTEXT_H
