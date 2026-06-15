#pragma once
#include "Drone.hpp"
#include <tuple>
#include <vector>

class MissionDrone : public Drone {
private:
    std::vector<std::pair<std::tuple<float, float, float>, std::string>> visited_waypoints;

public:
    std::string mission_name;
    std::vector<std::tuple<float, float, float>> waypoints;
    int current_waypoint_index;

    MissionDrone(std::string n, float battery, float max_alt, std::string m_name);

    std::tuple<float, float, float> next_waypoint();
    void skip_waypoint(const std::string& reason);
    bool mission_complete() const;
    std::string mission_summary() const;

    std::string get_info() const override;
};

