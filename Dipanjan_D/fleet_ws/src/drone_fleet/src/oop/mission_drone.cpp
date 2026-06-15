#include "mission_drone.hpp"
#include <sstream>

MissionDrone::MissionDrone(std::string n, float battery, float max_alt, std::string m_name)
    : Drone(std::move(n), battery, max_alt), mission_name(std::move(m_name)), current_waypoint_index(0) {}

std::tuple<float, float, float> MissionDrone::next_waypoint() {
    if (mission_complete()) {
        return {0,0,0}; // Or throw an exception
    }
    
    drain_battery(1.5f);
    auto current_wp = waypoints[current_waypoint_index];
    visited_waypoints.push_back({current_wp, get_timestamp()});
    current_waypoint_index++;
    
    add_log("Reached waypoint " + std::to_string(current_waypoint_index));
    return current_wp;
}

void MissionDrone::skip_waypoint(const std::string& reason) {
    if (!mission_complete()) {
        add_log("Skipped waypoint " + std::to_string(current_waypoint_index + 1) + ". Reason: " + reason);
        current_waypoint_index++;
    }
}

bool MissionDrone::mission_complete() const {
    return current_waypoint_index >= static_cast<int>(waypoints.size());
}

std::string MissionDrone::mission_summary() const {
    std::stringstream ss;
    ss << "Mission '" << mission_name << "' Summary:\n";
    ss << "Total waypoints: " << waypoints.size() << "\n";
    ss << "Visited: " << visited_waypoints.size() << "\n";
    return ss.str();
}

std::string MissionDrone::get_info() const {
    return Drone::get_info() + " | Mission: " + mission_name;
}

