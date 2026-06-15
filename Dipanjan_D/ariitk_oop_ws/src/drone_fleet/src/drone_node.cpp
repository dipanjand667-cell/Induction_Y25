#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include <sstream>
#include "drone_fleet/MissionDrone.hpp"

class DroneNode : public rclcpp::Node {
public:
    DroneNode() : Node("drone_node"), publish_count_(0) {
        // Declare and get parameters
        this->declare_parameter<std::string>("drone_name", "Unknown");
        this->declare_parameter<double>("initial_battery", 100.0);
        this->declare_parameter<std::string>("mission_name", "Default Mission");

        drone_name_ = this->get_parameter("drone_name").as_string();
        double battery = this->get_parameter("initial_battery").as_double();
        std::string mission = this->get_parameter("mission_name").as_string();

        // Initialize MissionDrone
        drone_ = std::make_unique<MissionDrone>(drone_name_, battery, 100.0, mission);
        setup_mission();
        drone_->take_off(15.2);

        // Publishers
        std::string base_topic = "/drone/" + drone_name_;
        pub_status_ = this->create_publisher<std_msgs::msg::String>(base_topic + "/status", 10);
        pub_telemetry_ = this->create_publisher<std_msgs::msg::String>(base_topic + "/telemetry", 10);
        pub_alert_ = this->create_publisher<std_msgs::msg::String>(base_topic + "/alert", 10);
        pub_mission_ = this->create_publisher<std_msgs::msg::String>(base_topic + "/mission_complete", 10);

        // Timers
        timer_status_ = this->create_wall_timer(std::chrono::seconds(1), std::bind(&DroneNode::status_callback, this));
        timer_telemetry_ = this->create_wall_timer(std::chrono::seconds(2), std::bind(&DroneNode::telemetry_callback, this));
        
        RCLCPP_INFO(this->get_logger(), "Drone Node %s started.", drone_name_.c_str());
    }

private:
    std::unique_ptr<MissionDrone> drone_;
    std::string drone_name_;
    int publish_count_;

    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_status_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_telemetry_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_alert_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_mission_;
    rclcpp::TimerBase::SharedPtr timer_status_;
    rclcpp::TimerBase::SharedPtr timer_telemetry_;

    void setup_mission() {
        drone_->waypoints = { {10,10,15}, {20,20,15}, {30,30,15}, {40,40,15}, {50,50,15} };
        drone_->current_waypoint_index = 0;
    }

    void status_callback() {
        if (drone_->get_status() == "idle" || drone_->get_status() == "error") return;

        // Drain battery
        drone_->drain_battery(0.5);
        publish_count_++;

        // Advance waypoint
        if (publish_count_ % 3 == 0) {
            drone_->next_waypoint();
        }

        // Check Critical
        if (drone_->is_critical()) {
            drone_->land();
            auto msg = std_msgs::msg::String();
            msg.data = "CRITICAL BATTERY: " + std::to_string(drone_->get_battery_level()) + "%";
            pub_alert_->publish(msg);
            RCLCPP_WARN(this->get_logger(), "%s", msg.data.c_str());
            return;
        }

        // Check Mission Complete
        if (drone_->mission_complete()) {
            auto msg = std_msgs::msg::String();
            msg.data = "Mission Complete! Restarting.";
            pub_mission_->publish(msg);
            setup_mission(); // Restart
        }

        // Publish Status
        auto msg = std_msgs::msg::String();
        std::stringstream ss;
        ss << "name:" << drone_name_ 
           << "|battery:" << drone_->get_battery_level() 
           << "|altitude:" << drone_->get_altitude()
           << "|status:" << drone_->get_status()
           << "|waypoint:" << (drone_->current_waypoint_index + 1) << "/5"
           << "|speed:" << drone_->get_speed();
        msg.data = ss.str();
        pub_status_->publish(msg);
    }

    void telemetry_callback() {
        auto msg = std_msgs::msg::String();
        std::stringstream ss;
        ss << "{\n"
           << "  \"name\": \"" << drone_name_ << "\",\n"
           << "  \"battery\": " << drone_->get_battery_level() << ",\n"
           << "  \"altitude\": " << drone_->get_altitude() << ",\n"
           << "  \"status\": \"" << drone_->get_status() << "\",\n"
           << "  \"mission_index\": " << drone_->current_waypoint_index << "\n"
           << "}";
        msg.data = ss.str();
        pub_telemetry_->publish(msg);
    }
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<DroneNode>());
    rclcpp::shutdown();
    return 0;
}

