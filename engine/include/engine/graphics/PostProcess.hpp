#ifndef POST_PROCESS_HPP
#define POST_PROCESS_HPP

#include "FrameBuffers.hpp"
#include <engine/core/Controller.hpp>
#include <engine/resources/ResourcesController.hpp>

namespace engine::graphics {

class PostProcess {
public:
    PostProcess(unsigned int width,
                unsigned int height,
                engine::resources::Shader *shader);

    ~PostProcess();

    void begin();

    void end();

    void resize(unsigned int w,
                unsigned int h);

    void toggle_sharpen() { enabled = !enabled; }

    bool is_sharpen_enabled() const { return enabled; }

private:
    void initQuad();

    FrameBuffers fb;
    engine::resources::Shader *shader;
    unsigned int quadVAO{}, quadVBO{};

    bool enabled = false;
};

}

#endif
