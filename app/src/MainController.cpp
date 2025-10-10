//
// Created by janic on 7/19/2025.
//
#define GLM_ENABLE_EXPERIMENTAL

#include "MainController.hpp"
#include <engine/core/Controller.hpp>
#include <engine/platform/PlatformController.hpp>
#include <spdlog/spdlog.h>
#include <engine/resources/ResourcesController.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <GuiController.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>


namespace app {

class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {
public:
    void on_mouse_move(engine::platform::MousePosition position) override;
};

void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
    auto gui = engine::core::Controller::get<app::GuiController>();
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (!gui->is_enabled() && platform->key(engine::platform::KeyId::MOUSE_BUTTON_LEFT)
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
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    engine::resources::Shader *shader = resources->shader("post_process");
    int w = platform->window()
                    ->width();
    int h = platform->window()
                    ->height();
    post_process = std::make_unique<engine::graphics::PostProcess>(
            w, h,
            shader
    );
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

void MainController::draw_shuttle() {
    if (!is_shuttle_active()) { return; }

    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto gui = engine::core::Controller::get<app::GuiController>();
    auto camera = graphics->camera();

    auto shader = resources->shader("basic");
    auto shuttle_model = resources->model("shuttle");

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", camera->view_matrix());

    glm::mat4 model = glm::translate(glm::mat4(1.0f), shuttle_spawn_pos);
    glm::vec3 forward = glm::normalize(shuttle_direction);

    glm::vec3 model_forward = glm::vec3(0.0f, -1.0f, 0.0f);
    glm::vec3 model_up = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::quat qDir = glm::rotation(model_forward, forward);
    glm::vec3 new_up = qDir * model_up;
    glm::quat qRoll = glm::rotation(new_up, glm::vec3(0, 1, 0));
    glm::quat qFinal = qRoll * qDir;
    model *= glm::toMat4(qFinal);
    model = glm::scale(model, glm::vec3(0.002f));

    shader->set_mat4("model", model);

    shader->set_vec3("viewPos", camera->Position);
    float intensity = gui->get_dir_light_intensity();
    shader->set_vec3("dir_light.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
    shader->set_vec3("dir_light.ambient", glm::vec3(0.3f) * intensity);
    shader->set_vec3("dir_light.diffuse", glm::vec3(0.6f) * intensity);
    shader->set_vec3("dir_light.specular", glm::vec3(0.9f) * intensity);

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

    shader->set_float("shininess", 32.0f);
    shuttle_model->draw(shader);
}

void MainController::draw_asteroid() {
    if (!asteroid_active) { return; }

    //Drawing the asteroid
    auto resources = Controller::get<engine::resources::ResourcesController>();
    auto graphics = Controller::get<engine::graphics::GraphicsController>();
    auto shader = resources->shader("sphere");
    auto model = resources->model("sphere");

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()
                                     ->view_matrix());

    glm::mat4 m = glm::mat4(1.0f);
    m = glm::translate(m, asteroid_position);
    m = glm::scale(m, glm::vec3(0.00003f));

    shader->set_mat4("model", m);
    shader->set_vec3("color", glm::vec3(1.0f));

    model->draw(shader);

    //Drawing the asteroid "trail". Used the duplicating method for this implementation.
    for (size_t i = 0; i < asteroid_trail.size(); ++i) {
        float scale = 0.00001f + (float(i) / asteroid_trail.size()) * 0.00002f;
        float alpha = 1.0f - float(i) / asteroid_trail.size();

        glm::mat4 m = glm::mat4(1.0f);
        m = glm::translate(m, asteroid_trail[i]);
        m = glm::scale(m, glm::vec3(scale));

        shader->set_mat4("model", m);
        shader->set_vec3("color", glm::vec3(1.0f) * alpha);

        model->draw(shader);
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

    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();

    if (!shuttle_active && platform->key(engine::platform::KeyId::KEY_SPACE)
                                   .state() == engine::platform::Key::State::JustPressed) {
        shuttle_active = true;
        shuttle_direction = camera->Front;
        shuttle_spawn_pos = camera->Position + glm::vec3(0.0f, -0.1f, 0.0f);
    }

    if (shuttle_active) {
        float dt = platform->dt();
        shuttle_spawn_pos += shuttle_direction * get_shuttle_speed() * dt;

        float traveled = glm::length(shuttle_spawn_pos - (camera->Position - shuttle_direction * 2.0f));
        if (traveled >= shuttle_max_dist) {
            shuttle_active = false;
        }
    }

    //Updating the asteroid position
    asteroid_timer += platform->dt();

    if (!asteroid_active && asteroid_timer >= 10.0f) {
        asteroid_timer = 0.0f;
        asteroid_active = true;

        glm::vec3 forward = glm::normalize(camera->Front);
        glm::vec3 right = glm::normalize(glm::cross(forward, camera->Up));
        glm::vec3 up = glm::normalize(camera->Up);

        float x_offset = (rand() % 1000 / 1000.0f - 0.5f) * 2.0f;
        float y_offset = (rand() % 1000 / 1000.0f - 0.5f) * 2.0f;

        asteroid_position = camera->Position + forward * 15.0f + right * x_offset * 5.0f + up * y_offset * 5.0f;
        asteroid_velocity = glm::normalize(glm::cross(up, forward)) * 2.5f;
    }

    if (asteroid_active) {
        //Asteroid section
        asteroid_position += asteroid_velocity * platform->dt();
        //Trail section
        trail_record_timer += platform->dt();
        if (trail_record_timer > 0.01f) {
            trail_record_timer = 0.0f;
            asteroid_trail.push_back(asteroid_position);
            if (asteroid_trail.size() > max_trail_points) {
                asteroid_trail.erase(asteroid_trail.begin());
            }
        }

        if (glm::length(asteroid_position - graphics->camera()
                                                    ->Position) > 100.0f) {
            asteroid_active = false;
        }
    }

    //Sharpen
    if (platform->key(engine::platform::KeyId::KEY_P)
                .state() == engine::platform::Key::State::JustPressed) {
        post_process->toggle_sharpen();
        spdlog::info("Sharpen post-process {}", post_process->is_sharpen_enabled() ? "ENABLED" : "DISABLED");
    }
    //Bloom
    if (platform->key(engine::platform::KeyId::KEY_B)
                .state() == engine::platform::Key::State::JustPressed) {
        post_process->toggle_bloom();
        spdlog::info("Bloom post-process {}", post_process->is_bloom_enabled() ? "ENABLED" : "DISABLED");
    }
}

void MainController::begin_draw() {
    post_process->begin();
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
    draw_shuttle();
    draw_spheres();
    draw_asteroid();
    draw_skybox();
}

void MainController::end_draw() {
    post_process->end();
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}

void MainController::terminate() {
    post_process->terminate();
}
}
