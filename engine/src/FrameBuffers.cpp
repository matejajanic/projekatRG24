//
// Created by janic on 7/22/2025.
//

#include <glad/glad.h>
#include "engine/graphics/FrameBuffers.hpp"
#include <spdlog/spdlog.h>

namespace engine::graphics {

FrameBuffers::FrameBuffers(unsigned int width, unsigned int height) : SCR_WIDTH(width), SCR_HEIGHT(height) {
    init_buffers();
}

FrameBuffers::~FrameBuffers() {}

void FrameBuffers::init_buffers() {
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glGenTextures(1, &color_buffer);
    glBindTexture(GL_TEXTURE_2D, color_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F,
                 SCR_WIDTH, SCR_HEIGHT, 0,
                 GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, color_buffer, 0);

    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER,
                          GL_DEPTH24_STENCIL8,
                          SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                              GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, RBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER)
        != GL_FRAMEBUFFER_COMPLETE) {
        spdlog::info("ERROR::FBO not complete\n");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffers::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
}

void FrameBuffers::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffers::terminate() {
    glDeleteFramebuffers(1, &FBO);
    glDeleteTextures(1, &color_buffer);
    glDeleteRenderbuffers(1, &RBO);
}

}
