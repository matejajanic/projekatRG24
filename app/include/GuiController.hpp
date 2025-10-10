//
// Created by janic on 7/19/2025.
//

#ifndef GUICONTROLLER_HPP
#define GUICONTROLLER_HPP

#include <engine/core/Controller.hpp>
#include <glm/glm.hpp>

namespace app {

class GuiController : public engine::core::Controller {
public:
    float get_dir_light_intensity() const {
        return dir_light_intensity;
    }

    float get_point_light_constant() const {
        return point_light_constant;
    }

    float get_point_light_linear() const {
        return point_light_linear;
    }

    float get_point_light_quadratic() const {
        return point_light_quadratic;
    }

    std::array<glm::vec3, 5> point_light_positions = {
            glm::vec3(0.5f, 0.5f, -3.2f),
            glm::vec3(-0.5f, 0.5f, -3.2f),
            glm::vec3(0.0f, -0.5f, -3.0f),
            glm::vec3(1.0f, 0.0f, -3.0f),
            glm::vec3(-1.0f, 0.0f, -3.0f)
    };

    std::array<glm::vec3, 5> point_light_colors = {
            glm::vec3(1.0f, 0.9f, 0.9f),
            glm::vec3(0.9f, 1.0f, 0.9f),
            glm::vec3(0.9f, 0.9f, 1.0f),
            glm::vec3(1.0f, 1.0f, 0.9f),
            glm::vec3(0.9f, 1.0f, 1.0f)
    };

    std::string_view name() const override {
        return "app::GuiController";
    }

private:
    float dir_light_intensity = 1.0f;
    float point_light_constant = 1.0f;
    float point_light_linear = 0.1f;
    float point_light_quadratic = 0.05f;

    void initialize() override;

    void poll_events() override;

    void draw() override;
};

}


#endif //GUICONTROLLER_HPP
