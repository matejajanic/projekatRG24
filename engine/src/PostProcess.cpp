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
    initBloomBuffers(width, height);
}

PostProcess::~PostProcess() {
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteFramebuffers(2, pingpongFBO);
    glDeleteTextures(2, pingpongColor);
    glDeleteTextures(1, &brightTex);
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

void PostProcess::initBloomBuffers(unsigned int w, unsigned int h) {
    // Bright target
    if (brightTex == 0) glGenTextures(1, &brightTex);
    glBindTexture(GL_TEXTURE_2D, brightTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // ping-pong za blur
    if (pingpongFBO[0] == 0) glGenFramebuffers(2, pingpongFBO);
    if (pingpongColor[0] == 0) glGenTextures(2, pingpongColor);
    for (int i = 0; i < 2; ++i) {
        glBindTexture(GL_TEXTURE_2D, pingpongColor[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColor[i], 0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcess::ensureBloomShaders() {
    if (bloomExtract && bloomBlur && bloomFinal) { return; }
    auto res = engine::core::Controller::get<ResourcesController>();
    bloomExtract = res->shader("bloom_extract");
    bloomBlur = res->shader("bloom_blur");
    bloomFinal = res->shader("bloom_final");

    bloomExtract->use();
    bloomExtract->set_int("uScene", 0);
    bloomBlur->use();
    bloomBlur->set_int("uImage", 0);
    bloomFinal->use();
    bloomFinal->set_int("uScene", 0);
    bloomFinal->set_int("uBloom", 1);
}

void PostProcess::begin() {
    fb.bind();
    glEnable(GL_DEPTH_TEST);
}

void PostProcess::end() {
    fb.unbind();
    glDisable(GL_DEPTH_TEST);

    if (bloomEnabled) {
        ensureBloomShaders();

        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[0]); // koristimo [0] kao “brightTex FBO”
        bloomExtract->use();
        bloomExtract->set_float("uTargetHue", bloomTargetHue);
        bloomExtract->set_float("uHueWidth", bloomHueWidth);
        bloomExtract->set_float("uMinS", bloomMinS);
        bloomExtract->set_float("uMinV", bloomMinV);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fb.get_color_texture());
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        bool horizontal = true;
        int passes = 8; // po želji
        for (int i = 0; i < passes; i++) {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal ? 1 : 0]);
            bloomBlur->use();
            bloomBlur->set_bool("uHorizontal", horizontal);
            glBindTexture(GL_TEXTURE_2D, (i == 0) ? pingpongColor[0] : pingpongColor[horizontal ? 0 : 1]);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            horizontal = !horizontal;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        bloomFinal->use();
        bloomFinal->set_bool("uSharpen", enabled);
        bloomFinal->set_float("uStrength", bloomStrength);
        bloomFinal->set_vec2("uOffset", offset);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fb.get_color_texture());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, pingpongColor[horizontal ? 0 : 1]);
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        return;
    }

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

    offset = glm::vec2(1.0f / w, 1.0f / h);

    shader->use();
    shader->set_vec2("offset", offset);

    initBloomBuffers(w, h);
}
