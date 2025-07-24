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

private:
    void initQuad();

    FrameBuffers fb;
    engine::resources::Shader *shader;
    unsigned int quadVAO{}, quadVBO{};
};

}

#endif
