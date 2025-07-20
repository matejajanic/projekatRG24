//
// Created by janic on 7/19/2025.
//

#include "GuiController.hpp"
#include <engine/graphics/GraphicsController.hpp>
#include <engine/platform/PlatformController.hpp>
#include <imgui.h>

namespace app {

void GuiController::initialize() {
    set_enable(false);
}

void GuiController::poll_events() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ENTER)
                .state() == engine::platform::Key::State::JustPressed) {
        set_enable(!is_enabled());
    }
}

void GuiController::draw() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    graphics->begin_gui();

    //Camera Information
    ImGui::Begin("Camera info");
    ImGui::Text("Camera position: (%f, %f, %f)", camera->Position
                                                       .x, camera->Position
                                                                 .y, camera->Position
                                                                           .z);
    //Directional Light Information
    ImGui::Separator();
    ImGui::Text("Directional Light");
    ImGui::SliderFloat("Intensity: ", &dir_light_intensity, 0.0f, 6.0f);

    // Point Lights (Stars)
    ImGui::Separator();
    ImGui::Text("Point Lights (Stars)");

    for (int i = 0; i < 5; ++i) {
        std::string label = "Star " + std::to_string(i + 1);
        if (ImGui::TreeNode(label.c_str())) {
            ImGui::SliderFloat3(("Position##" + std::to_string(i + 1)).c_str(), &point_light_positions[i].x, -2.0f,
                                2.0f);
            ImGui::ColorEdit3(("Color##" + std::to_string(i + 1)).c_str(), &point_light_colors[i].x);
            ImGui::TreePop();
        }
    }

    ImGui::SliderFloat("Constant", &point_light_constant, 0.5f, 2.0f);
    ImGui::SliderFloat("Linear", &point_light_linear, 0.0f, 0.1f);
    ImGui::SliderFloat("Quadratic", &point_light_quadratic, 0.0f, 0.01f);


    ImGui::End();

    graphics->end_gui();
}
}
