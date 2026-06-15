#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include <std_srvs/srv/trigger.hpp>
#include <map>
#include <string>
#include <iomanip>

struct DroneState {
    double battery = 0.0;
    double altitude = 0.0;
    std::string status = "unknown";
    int waypoint = 0;
};

class FleetManager : public rclcpp::Node {
public:
    FleetManager() : Node("fleet_manager") {
        std::vector<std::string> drones = {"Alpha", "Beta", "Gamma"};
        
        for (const auto& d : drones) {
            fleet_state_[d] = DroneState();
            
            subs_telemetry_.push_back(this->create_subscription<std_msgs::msg::String>(
                "/drone/" + d + "/telemetry", 10,
                [this, d](const std_msgs::msg::String::SharedPtr msg) { this->telemetry_cb(d, msg); }));
                
            subs_alert_.push_back(this->create_subscription<std_msgs::msg::String>(
                "/drone/" + d + "/alert", 10,
                [this, d](const std_msgs::msg::String::SharedPtr msg) { this->alert_cb(d, msg); }));
                
            subs_mission_.push_back(this->create_subscription<std_msgs::msg::String>(
                "/drone/" + d + "/mission_complete", 10,
                [this, d](const std_msgs::msg::String::SharedPtr /*msg*/) { 
                    RCLCPP_INFO(this->get_logger(), "[%s] Mission Complete!", d.c_str()); 
                }));
        }

        timer_report_ = this->create_wall_timer(
            std::chrono::seconds(5), std::bind(&FleetManager::print_report, this));

        srv_report_ = this->create_service<std_srvs::srv::Trigger>(
            "/fleet/status_report",
            [this](const std::shared_ptr<std_srvs::srv::Trigger::Request> req,
                   std::shared_ptr<std_srvs::srv::Trigger::Response> res) {
                (void)req;
                this->print_report();
                res->success = true;
                res->message = "Report generated.";
            });
            
        RCLCPP_INFO(this->get_logger(), "Fleet Manager initialized.");
    }

private:
    std::map<std::string, DroneState> fleet_state_;
    std::vector<rclcpp::Subscription<std_msgs::msg::String>::SharedPtr> subs_telemetry_, subs_alert_, subs_mission_;
    rclcpp::TimerBase::SharedPtr timer_report_;
    rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr srv_report_;

    // Manual JSON extraction helper
    std::string extract_json_val(const std::string& json, const std::string& key, bool is_string) {
        std::string search = "\"" + key + "\":";
        size_t pos = json.find(search);
        if (pos == std::string::npos) return "";
        pos += search.length();
        
        if (is_string) {
            size_t start = json.find("\"", pos) + 1;
            size_t end = json.find("\"", start);
            return json.substr(start, end - start);
        } else {
            size_t start = json.find_first_not_of(" \t", pos);
            size_t end = json.find_first_of(",\n\r}", start);
            return json.substr(start, end - start);
        }
    }

    void telemetry_cb(const std::string& drone, const std_msgs::msg::String::SharedPtr msg) {
        std::string data = msg->data;
        try {
            fleet_state_[drone].battery = std::stod(extract_json_val(data, "battery", false));
            fleet_state_[drone].altitude = std::stod(extract_json_val(data, "altitude", false));
            fleet_state_[drone].status = extract_json_val(data, "status", true);
            fleet_state_[drone].waypoint = std::stoi(extract_json_val(data, "mission_index", false)) + 1;
        } catch (...) { /* Handle parsing errors silently for continuous operation */ }
    }

    void alert_cb(const std::string& drone, const std_msgs::msg::String::SharedPtr msg) {
        RCLCPP_ERROR(this->get_logger(), "[ALERT] %s: %s", drone.c_str(), msg->data.c_str());
    }

    void print_report() {
        std::cout << "\n----------------------------------------------------\n";
        std::cout << std::left << std::setw(10) << "Drone" 
                  << std::setw(10) << "Battery" 
                  << std::setw(10) << "Altitude" 
                  << std::setw(10) << "Waypoint" 
                  << std::setw(10) << "Status" << "\n";
        std::cout << "----------------------------------------------------\n";
        for (const auto& [name, state] : fleet_state_) {
            std::cout << std::left << std::setw(10) << name 
                      << std::setw(10) << state.battery 
                      << std::setw(10) << state.altitude 
                      << std::setw(10) << state.waypoint 
                      << std::setw(10) << state.status << "\n";
        }
        std::cout << "----------------------------------------------------\n";
    }
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<FleetManager>());
    rclcpp::shutdown();
    return 0;
}
