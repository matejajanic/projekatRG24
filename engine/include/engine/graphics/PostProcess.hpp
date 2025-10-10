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

    void toggle_bloom() { bloomEnabled = !bloomEnabled; }

    bool is_bloom_enabled() const { return bloomEnabled; }

    void set_bloom_color(float targetHue, float hueWidth, float minS, float minV, float strength) {
        bloomTargetHue = targetHue;
        bloomHueWidth = hueWidth;
        bloomMinS = minS;
        bloomMinV = minV;
        bloomStrength = strength;
    }

    void terminate();

private:
    void initQuad();

    FrameBuffers fb;
    engine::resources::Shader *shader;
    unsigned int quadVAO{}, quadVBO{};

    bool enabled = false;

    bool bloomEnabled = false;

    //sejderi za bloom
    engine::resources::Shader *bloomExtract = nullptr;
    engine::resources::Shader *bloomBlur = nullptr;
    engine::resources::Shader *bloomFinal = nullptr;

    // ping-pong FBO
    unsigned int pingpongFBO[2] = {0, 0};
    unsigned int pingpongColor[2] = {0, 0};
    unsigned int brightTex = 0;

    //namestanje parametara u HSV za tirkiznu boju stanice
    float bloomTargetHue = 0.50f;
    float bloomHueWidth = 0.08f;
    float bloomMinS = 0.25f;
    float bloomMinV = 0.60f;
    float bloomStrength = 1.2f;
    glm::vec2 offset;

    void initBloomBuffers(unsigned int w, unsigned int h);

    void ensureBloomShaders();
};

}

#endif
