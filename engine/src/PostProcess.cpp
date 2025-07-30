//
// Created by janic on 7/22/2025.
//
#include <glad/glad.h>
#include "engine/graphics/PostProcess.hpp"
#include <engine/resources/ResourcesController.hpp>

using namespace engine::graphics;
using namespace engine::resources;

PostProcess::PostProcess(unsigned int width,
                         unsigned int height,
                         Shader *shader)
        : fb(width, height), shader(shader) {
    auto resources = engine::core::Controller::get<ResourcesController>();

    shader->use();
    shader->set_int("screenTexture", 0);
    shader->set_vec2("offset", glm::vec2(1.0f / width, 1.0f / height));
    shader->set_bool("sharpen", enabled);
    initQuad();
}

PostProcess::~PostProcess() {
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
}

void PostProcess::initQuad() {
    float quadVertices[] = {
            // positions   // texCoords
            -1.0f, 1.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f
    };
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));
    glBindVertexArray(0);
}

void PostProcess::begin() {
    fb.bind();
    glEnable(GL_DEPTH_TEST);
}

void PostProcess::end() {
    fb.unbind();
    glDisable(GL_DEPTH_TEST);
    shader->use();
    shader->set_bool("sharpen", enabled);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fb.get_color_texture());
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void PostProcess::resize(unsigned int w, unsigned int h) {
    fb = FrameBuffers(w, h);
    shader->use();
    shader->set_vec2("offset", glm::vec2(1.0f / w, 1.0f / h));
}
