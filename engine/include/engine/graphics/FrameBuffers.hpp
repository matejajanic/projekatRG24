//
// Created by janic on 7/22/2025.
//

#ifndef FRAMEBUFFERS_HPP
#define FRAMEBUFFERS_HPP

namespace engine::graphics {

class FrameBuffers {
public:
    FrameBuffers(unsigned int width, unsigned int height);

    ~FrameBuffers();

    void bind();

    void unbind();

    unsigned int get_color_texture() const { return color_buffer; }

private:
    void init_buffers();

    unsigned int SCR_WIDTH, SCR_HEIGHT;
    unsigned int color_buffer, RBO, FBO;
};

}
#endif //FRAMEBUFFERS_HPP
