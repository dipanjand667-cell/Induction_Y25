#pragma once
#include "mission_drone.hpp"

class AutonomousDrone : public MissionDrone {
private:
    std::vector<std::string> obstacle_log;

public:
    std::string ai_mode;
    std::tuple<float, float, float> home_position;

    AutonomousDrone(std::string n, float battery, float max_alt, std::string m_name, std::tuple<float, float, float> home);

    void set_ai_mode(const std::string& mode);
    void detect_obstacle(std::tuple<float, float, float> position, const std::string& severity);
    std::vector<std::tuple<float, float, float>> auto_replan(const std::vector<std::tuple<float, float, float>>& obstacles);

    std::string get_info() const override;
};
