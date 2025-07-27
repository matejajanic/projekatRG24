//
// Created by janic on 7/19/2025.
//

#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP

#include <engine/core/Controller.hpp>
#include <glm/glm.hpp>
#include <engine/graphics/PostProcess.hpp>

namespace app {

class MainController : public engine::core::Controller {
    void initialize() override;

    bool loop() override;

    void draw_space_station();

    void draw_spheres();

    void draw_shuttle();

    void draw_asteroid();

    void update_camera();

    void update() override;

    void begin_draw() override;

    void draw_skybox();

    void draw() override;

    void end_draw() override;

public:
    std::string_view name() const override {
        return "app::MainController";
    }

    float get_shuttle_speed() const {
        return shuttle_speed;
    }

    void set_shuttle_speed(float v) {
        shuttle_speed = v;
    }

    float get_shuttle_max_dist() const {
        return shuttle_max_dist;
    }

    void set_shuttle_max_dist(float v) {
        shuttle_max_dist = v;
    }

    bool is_shuttle_active() const {
        return shuttle_active;
    }


private:
    bool shuttle_active = false;
    glm::vec3 shuttle_spawn_pos;
    glm::vec3 shuttle_direction;
    float shuttle_speed = 0.15f;
    float shuttle_max_dist = 6.0f;
    std::unique_ptr<engine::graphics::PostProcess> post_process;
    float asteroid_timer = 0.0f;
    bool asteroid_active = false;
    glm::vec3 asteroid_position;
    glm::vec3 asteroid_velocity;
    std::vector<glm::vec3> asteroid_trail;
    int max_trail_points = 30;
    float trail_record_timer = 0.0f;
};

}


#endif //MAINCONTROLLER_HPP
