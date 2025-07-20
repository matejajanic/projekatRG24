//
// Created by janic on 7/19/2025.
//

#ifndef GUICONTROLLER_HPP
#define GUICONTROLLER_HPP

#include <engine/core/Controller.hpp>

namespace app {

class GuiController : public engine::core::Controller {
public:
    float get_dir_light_intensity() const {
        return dir_light_intensity;
    }

    std::string_view name() const override {
        return "app::GuiController";
    }

private:
    float dir_light_intensity = 3.0f;

    void initialize() override;

    void poll_events() override;

    void draw() override;
};

}


#endif //GUICONTROLLER_HPP
