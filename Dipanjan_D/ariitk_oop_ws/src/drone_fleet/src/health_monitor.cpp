#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include <deque>
#include <map>
#include <chrono>

struct TelemSample {
    rclcpp::Time timestamp;
    double battery;
};

class HealthMonitor : public rclcpp::Node {
public:
    HealthMonitor() : Node("health_monitor") {
        std::vector<std::string> drones = {"Alpha", "Beta", "Gamma"};
        
        for (const auto& d : drones) {
            subs_telemetry_.push_back(this->create_subscription<std_msgs::msg::String>(
                "/drone/" + d + "/telemetry", 10,
                [this, d](const std_msgs::msg::String::SharedPtr msg) { this->telemetry_cb(d, msg); }));
        }

        pub_warning_ = this->create_publisher<std_msgs::msg::String>("/fleet/health_warning", 10);
        pub_summary_ = this->create_publisher<std_msgs::msg::String>("/fleet/health_summary", 10);

        timer_diag_ = this->create_wall_timer(
            std::chrono::seconds(10), std::bind(&HealthMonitor::diagnostics_cb, this));
            
        RCLCPP_INFO(this->get_logger(), "Health Monitor active.");
    }

private:
    std::map<std::string, std::deque<TelemSample>> history_;
    std::vector<rclcpp::Subscription<std_msgs::msg::String>::SharedPtr> subs_telemetry_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_warning_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_summary_;
    rclcpp::TimerBase::SharedPtr timer_diag_;

    double get_battery(const std::string& json) {
        size_t pos = json.find("\"battery\":");
        if (pos == std::string::npos) return 0.0;
        pos += 10;
        size_t end = json.find_first_of(",\n\r}", pos);
        return std::stod(json.substr(pos, end - pos));
    }

    void telemetry_cb(const std::string& drone, const std_msgs::msg::String::SharedPtr msg) {
        double current_batt = get_battery(msg->data);
        auto now = this->now();
        
        auto& buffer = history_[drone];
        buffer.push_back({now, current_batt});
        if (buffer.size() > 10) buffer.pop_front();

        // Check warning if buffer has enough data
        if (buffer.size() >= 2) {
            double dt = (buffer.back().timestamp - buffer.front().timestamp).seconds();
            double dbatt = buffer.front().battery - buffer.back().battery;
            if (dt > 0) {
                double rate = dbatt / dt;
                if (rate > 1.5) {
                    auto warn = std_msgs::msg::String();
                    warn.data = "HIGH DRAIN RATE [" + drone + "]: " + std::to_string(rate) + " %/s";
                    pub_warning_->publish(warn);
                }
            }
        }
    }

    void diagnostics_cb() {
        std::cout << "\n--- [DIAGNOSTICS] ---\n";
        std::string json_summary = "{\n  \"diagnostics\": [\n";
        bool first = true;

        for (const auto& [drone, buffer] : history_) {
            if (buffer.size() < 2) continue;

            double dt = (buffer.back().timestamp - buffer.front().timestamp).seconds();
            double dbatt = buffer.front().battery - buffer.back().battery;
            double rate = (dt > 0) ? (dbatt / dt) : 0.0;
            double current_batt = buffer.back().battery;
            
            double time_to_crit = (rate > 0) ? (current_batt - 20.0) / rate : -1;
            double time_to_dep = (rate > 0) ? current_batt / rate : -1;

            if (time_to_crit < 0) time_to_crit = 0;

            std::cout << "[" << drone << "] Rate: " << rate 
                      << " | T-Critical: " << time_to_crit << "s"
                      << " | T-Depletion: " << time_to_dep << "s\n";

            if (!first) json_summary += ",\n";
            json_summary += "    {\"drone\": \"" + drone + "\", \"rate\": " + std::to_string(rate) + "}";
            first = false;
        }
        json_summary += "\n  ]\n}";
        
        auto msg = std_msgs::msg::String();
        msg.data = json_summary;
        pub_summary_->publish(msg);
    }
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<HealthMonitor>());
    rclcpp::shutdown();
    return 0;
}
