#include "vehicle.hpp"
#include "drone_exceptions.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <algorithm>

Vehicle::Vehicle(std::string n, float battery) 
    : battery_level(std::max(0.0f, std::min(100.0f, battery))), status("idle"), name(std::move(n)) {}

std::string Vehicle::get_timestamp() const {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void Vehicle::add_log(const std::string& entry) {
    flight_log.push_back("[" + get_timestamp() + "] " + entry);
}

void Vehicle::set_status(const std::string& new_status) {
    if (new_status != "idle" && new_status != "flying" && new_status != "charging" && new_status != "error") {
        throw InvalidStateError("Attempted to set invalid state: " + new_status);
    }
    status = new_status;
    add_log("Status changed to: " + status);
}

void Vehicle::drain_battery(float amount) {
    if (battery_level <= 0.0f) {
        throw BatteryDepletedError("Cannot drain battery: Already depleted (0%).");
    }
    battery_level -= amount;
    if (battery_level < 0.0f) battery_level = 0.0f;
}

void Vehicle::charge_battery(float amount, int duration_seconds) {
    if (status != "charging") {
        throw InvalidStateError("Cannot charge: Vehicle is not in 'charging' state.");
    }
    battery_level += amount;
    if (battery_level > 100.0f) battery_level = 100.0f;
    add_log("Charged by " + std::to_string(amount) + "% over " + std::to_string(duration_seconds) + " seconds.");
}

bool Vehicle::is_critical() const {
    return battery_level < 20.0f;
}

std::string Vehicle::get_flight_log() const {
    std::string full_log = "--- Flight Log for " + name + " ---\n";
    for (const auto& log : flight_log) {
        full_log += log + "\n";
    }
    return full_log;
}

float Vehicle::get_battery_level() const { return battery_level; }
std::string Vehicle::get_status() const { return status; }
