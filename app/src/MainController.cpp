//
// Created by janic on 7/19/2025.
//

#include "MainController.hpp"
#include <engine/core/Controller.hpp>
#include <engine/platform/PlatformController.hpp>
#include <spdlog/spdlog.h>
#include <engine/resources/ResourcesController.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <GuiController.hpp>

namespace app {

class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {
public:
    void on_mouse_move(engine::platform::MousePosition position) override;
};

void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
    auto gui_controller = engine::core::Controller::get<app::GuiController>();
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (!gui_controller->is_enabled() && platform->key(engine::platform::KeyId::MOUSE_BUTTON_LEFT)
                                                 .is_down()) {
        auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
        camera->rotate_camera(position.dx, position.dy);
    }
}

void MainController::initialize() {
    spdlog::info("MainController initialized");
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());
    engine::graphics::OpenGL::enable_depth_testing();
}

bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE)
                .is_down()) {
        return false;
    }

    return true;
}

void MainController::draw_space_station() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto gui = engine::core::Controller::get<app::GuiController>();
    engine::resources::Model *space_station = resources->model("space_station");

    engine::resources::Shader *shader = resources->shader("basic");

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()
                                     ->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
    model = glm::scale(model, glm::vec3(0.01f));
    model = glm::rotate(model, glm::radians(-60.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    shader->set_mat4("model", model);
    shader->set_float("shininess", 40.0f);

    //Directional Light
    glm::vec3 ambient, diffuse, specular;
    float dir_intensity = gui->get_dir_light_intensity();

    ambient = glm::vec3(0.3f) * dir_intensity;
    diffuse = glm::vec3(0.6f) * dir_intensity;
    specular = glm::vec3(0.9f) * dir_intensity;

    shader->set_vec3("dir_light.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
    shader->set_vec3("dir_light.ambient", ambient);
    shader->set_vec3("dir_light.diffuse", diffuse);
    shader->set_vec3("dir_light.specular", specular);

    //Point lights
    for (int i = 0; i < 5; ++i) {
        std::string idx = std::to_string(i);
        shader->set_vec3("pointLights[" + idx + "].position", gui->point_light_positions[i]);
        shader->set_vec3("pointLights[" + idx + "].ambient", gui->point_light_colors[i] * 0.02f);
        shader->set_vec3("pointLights[" + idx + "].diffuse", gui->point_light_colors[i] * 0.8f);
        shader->set_vec3("pointLights[" + idx + "].specular", gui->point_light_colors[i]);

        shader->set_float("pointLights[" + idx + "].constant", gui->get_point_light_constant());
        shader->set_float("pointLights[" + idx + "].linear", gui->get_point_light_linear());
        shader->set_float("pointLights[" + idx + "].quadratic", gui->get_point_light_quadratic());
    }


    space_station->draw(shader);
}

void MainController::draw_spheres() {
    auto gui = Controller::get<GuiController>();
    auto graphics = Controller::get<engine::graphics::GraphicsController>();
    auto resources = Controller::get<engine::resources::ResourcesController>();

    engine::resources::Model *sphere_model = resources->model("sphere");

    auto shader = resources->shader("sphere");
    shader->use();

    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()
                                     ->view_matrix());

    for (int i = 0; i < 5; ++i) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, gui->point_light_positions[i]);
        model = glm::scale(model, glm::vec3(0.00002f));  // veličina kocke

        shader->set_mat4("model", model);
        shader->set_vec3("color", gui->point_light_colors[i]);

        sphere_model->draw(shader);
    }
}


void MainController::update_camera() {
    auto gui_controller = engine::core::Controller::get<GuiController>();
    if (gui_controller->is_enabled()) {
        return;
    }
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    float dt = platform->dt();

    if (platform->key(engine::platform::KeyId::KEY_W)
                .is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt);
    }
    if (platform->key(engine::platform::KeyId::KEY_S)
                .is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt);
    }
    if (platform->key(engine::platform::KeyId::KEY_A)
                .is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt);
    }
    if (platform->key(engine::platform::KeyId::KEY_D)
                .is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt);
    }
    if (platform->key(engine::platform::KeyId::KEY_UP)
                .is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::UP, dt);
    }
    if (platform->key(engine::platform::KeyId::KEY_DOWN)
                .is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::DOWN, dt);
    }
}

void MainController::update() {
    update_camera();
}

void MainController::begin_draw() {
    engine::graphics::OpenGL::clear_buffers();
}

void MainController::draw_skybox() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto skybox = resources->skybox("red_skybox1");
    auto shader = resources->shader("skybox");
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    graphics->draw_skybox(shader, skybox);
}

void MainController::draw() {
    draw_space_station();
    draw_spheres();
    draw_skybox();
}

void MainController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}
}
