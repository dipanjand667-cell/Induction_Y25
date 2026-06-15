#include <string>
#include <vector>

class Vehicle {
private:
    float battery_level;
    std::string status;
    std::vector<std::string> flight_log;

protected:
    std::string get_timestamp() const;
    void add_log(const std::string& entry);

public:
    std::string name;

    Vehicle(std::string n, float battery);
    virtual ~Vehicle() = default;

    virtual std::string get_info() const = 0;

    void drain_battery(float amount);
    void charge_battery(float amount, int duration_seconds);
    bool is_critical() const;
    std::string get_flight_log() const;
    
    void set_status(const std::string& new_status);
    
    // Getters
    float get_battery_level() const;
    std::string get_status() const;
};

