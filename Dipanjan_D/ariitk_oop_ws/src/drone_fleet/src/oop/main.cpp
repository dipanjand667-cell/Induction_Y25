#include <iostream>
#include <vector>
#include "Exceptions.hpp"
#include "Drone.hpp"
#include "MissionDrone.hpp"
#include "AutonomousDrone.hpp"

int main() {
    std::cout << "--- Initializing Fleet ---\n";

    // Create objects
    Drone basic_drone("Alpha", 100.0f, 50.0f);
    MissionDrone mapper("Beta", 80.0f, 100.0f, "Sector 4 Mapping");
    AutonomousDrone auto_drone("Gamma", 100.0f, 120.0f, "Perimeter Patrol", {0.0f, 0.0f, 0.0f});

    // 1. Demonstrate Polymorphism
    std::vector<Vehicle*> fleet = { &basic_drone, &mapper, &auto_drone };
    for (const auto* v : fleet) {
        std::cout << v->get_info() << "\n";
    }

    // 2. Encapsulation check
    // auto_drone.battery_level = 100.0f; // ERROR: 'battery_level' is a private member of 'Vehicle'
    // auto_drone.status = "flying";      // ERROR: 'status' is a private member of 'Vehicle'

    std::cout << "\n--- Testing Exceptions & Behavior ---\n";
    try {
        // Altitude Error
        basic_drone.take_off(60.0f); 
    } catch (const DroneException& e) {
        std::cerr << "Caught Exception: " << e.what() << "\n";
    }

    try {
        // Invalid State Error (charging while idle)
        mapper.charge_battery(10.0f, 60); 
    } catch (const DroneException& e) {
        std::cerr << "Caught Exception: " << e.what() << "\n";
    }

    std::cout << "\n--- Running Full Autonomous Mission ---\n";
    try {
        auto_drone.waypoints = { {10, 10, 20}, {20, 20, 20}, {30, 30, 20} };
        auto_drone.take_off(20.0f);
        auto_drone.set_ai_mode("auto");

        // Iterate through waypoints
        while (!auto_drone.mission_complete()) {
            auto wp = auto_drone.next_waypoint();
            std::cout << "Navigating to waypoint: (" << std::get<0>(wp) << "," << std::get<1>(wp) << ")\n";
            
            // Simulate obstacle halfway through
            if (auto_drone.current_waypoint_index == 2) {
                std::cout << ">> Simulated threat detected!\n";
                auto_drone.detect_obstacle({25, 25, 20}, "high"); 
                break; // Emergency stop triggers error state
            }
        }

        std::cout << "\n" << auto_drone.mission_summary();
        std::cout << "\n" << auto_drone.get_flight_log();

    } catch (const DroneException& e) {
        std::cerr << "Critical Mission Failure: " << e.what() << "\n";
    }

    return 0;
}
