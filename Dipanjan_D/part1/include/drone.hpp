#pragma once
#include "vehicle.hpp"

class Drone : public Vehicle {
private:
    float speed;

protected:
    float altitude;
    float max_altitude;

public:
    Drone(std::string n, float battery, float max_alt);

    void take_off(float target_altitude);
    void land();
    void emergency_stop();

    std::string get_info() const override;
    
    // Getters/Setters
    void set_speed(float s);
    float get_speed() const;
    float get_altitude() const;
};
