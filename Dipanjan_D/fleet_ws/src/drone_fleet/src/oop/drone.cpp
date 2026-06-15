#include "drone.hpp"
#include "drone_exceptions.hpp"
#include <sstream>

Drone::Drone(std::string n, float battery, float max_alt) 
    : Vehicle(std::move(n), battery), speed(0.0f), altitude(0.0f), max_altitude(max_alt) {}

void Drone::take_off(float target_altitude) {
    if (target_altitude > max_altitude) {
        throw AltitudeError("Takeoff failed: Target altitude " + std::to_string(target_altitude) + " exceeds max altitude.");
    }
    set_status("flying");
    altitude = target_altitude;
    add_log("Took off to altitude: " + std::to_string(altitude));
}

void Drone::land() {
    altitude = 0.0f;
    speed = 0.0f;
    set_status("idle");
    add_log("Landed successfully.");
}

void Drone::emergency_stop() {
    altitude = 0.0f;
    speed = 0.0f;
    set_status("error");
    drain_battery(30.0f); // Penalty
    add_log("EMERGENCY STOP DEPLOYED. 30% battery penalty applied.");
}

std::string Drone::get_info() const {
    std::stringstream ss;
    ss << "Drone [" << name << "] | Bat: " << get_battery_level() << "% | Status: " << get_status() << " | Alt: " << altitude;
    return ss.str();
}

void Drone::set_speed(float s) { speed = s; }
float Drone::get_speed() const { return speed; }
float Drone::get_altitude() const { return altitude; }
