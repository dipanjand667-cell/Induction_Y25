#include "autonomous_drone.hpp"
#include <cmath>

AutonomousDrone::AutonomousDrone(std::string n, float battery, float max_alt, std::string m_name, std::tuple<float, float, float> home)
    : MissionDrone(std::move(n), battery, max_alt, std::move(m_name)), ai_mode("manual"), home_position(home) {}

void AutonomousDrone::set_ai_mode(const std::string& mode) {
    ai_mode = mode;
    add_log("AI Mode set to: " + mode);
    if (mode == "return_home") {
        waypoints.push_back(home_position);
        add_log("Home position added as final waypoint.");
    }
}

void AutonomousDrone::detect_obstacle(std::tuple<float, float, float> position, const std::string& severity) {
    std::string log_entry = "Obstacle detected at (" + std::to_string(std::get<0>(position)) + "," + 
                            std::to_string(std::get<1>(position)) + ") | Severity: " + severity;
    obstacle_log.push_back("[" + get_timestamp() + "] " + log_entry);
    add_log(log_entry);

    if (severity == "high") {
        emergency_stop();
    }
}

std::vector<std::tuple<float, float, float>> AutonomousDrone::auto_replan(const std::vector<std::tuple<float, float, float>>& obstacles) {
    std::vector<std::tuple<float, float, float>> new_route;
    for (const auto& wp : waypoints) {
        bool safe = true;
        for (const auto& obs : obstacles) {
            float dx = std::get<0>(wp) - std::get<0>(obs);
            float dy = std::get<1>(wp) - std::get<1>(obs);
            float dz = std::get<2>(wp) - std::get<2>(obs);
            if (std::sqrt(dx*dx + dy*dy + dz*dz) <= 5.0f) {
                safe = false;
                break;
            }
        }
        if (safe) new_route.push_back(wp);
    }
    return new_route;
}

std::string AutonomousDrone::get_info() const {
    return MissionDrone::get_info() + " | AI: " + ai_mode;
}
