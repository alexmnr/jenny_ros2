// Copyright 2023 ros2_control Development Team
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef JONNY_HARDWARE_INTERFACE_HPP_
#define JONNY_HARDWARE_INTERFACE_HPP_JONNY_HARDWARE_INTERFACE_HPP_

#include <math.h>

#include "hardware_interface/handle.hpp"
#include "hardware_interface/hardware_info.hpp"
#include "hardware_interface/system_interface.hpp"
#include "hardware_interface/types/hardware_interface_return_values.hpp"
#include "hardware_interface/types/hardware_interface_type_values.hpp"

#include "ros2_socketcan/socket_can_sender.hpp"
#include "ros2_socketcan/socket_can_receiver.hpp"
#include "ros2_socketcan/socket_can_common.hpp"
#include "ros2_socketcan/socket_can_id.hpp"

#include "rclcpp/rclcpp.hpp"

#include <jonny_interfaces/msg/joint_status.hpp>
#include <jonny_interfaces/msg/hardware_status.hpp>

#include "jonny_robot_control.hpp"

using hardware_interface::return_type;

namespace jonny_hardware_interface {
using CallbackReturn =
    rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn;

class JonnyHardwareInterface
    : public hardware_interface::SystemInterface {

  struct Config {
    std::string device = "can0";
    bool homing = false;
    bool debug = false;
  };

  struct MotorStatus {
    uint8_t id;
    bool ready = false;
    double position = 0.0;
    double previous_position = 0.0;
    double velocity = 0.0;
    std::chrono::time_point<std::chrono::high_resolution_clock> previous_time;
  };

public:
  CallbackReturn on_init(const hardware_interface::HardwareInfo &info) override;

  std::vector<hardware_interface::StateInterface>
  export_state_interfaces() override;

  std::vector<hardware_interface::CommandInterface>
  export_command_interfaces() override;

  CallbackReturn
  on_configure(const rclcpp_lifecycle::State &previous_state) override;

  CallbackReturn
  on_cleanup(const rclcpp_lifecycle::State &previous_state) override;

  CallbackReturn
  on_shutdown(const rclcpp_lifecycle::State &previous_state) override;

  CallbackReturn
  on_activate(const rclcpp_lifecycle::State &previous_state) override;

  return_type read(const rclcpp::Time &time,
                   const rclcpp::Duration &period) override;

  return_type write(const rclcpp::Time & /*time*/,
                    const rclcpp::Duration & /*period*/) override;
protected:
  bool ready = false;
  Config config_;

  // robot
  JonnyRobotControl robot;

  // Hardware Status Logger
  rclcpp::Publisher<jonny_interfaces::msg::HardwareStatus>::SharedPtr pub_;
  rclcpp::Node::SharedPtr node_;

  // // can stuff
  // std::unique_ptr<drivers::socketcan::SocketCanSender> sender;
  // std::unique_ptr<drivers::socketcan::SocketCanReceiver> receiver;

  /// Output from ros2_control
  std::vector<double> joint_position_command_;
  std::vector<double> joint_velocities_command_;
  /// Input to ros2_control
  std::vector<double> joint_position_;
  std::vector<double> joint_velocities_;
  // more ros2_control
  std::unordered_map<std::string, std::vector<std::string>> joint_interfaces = {
      {"position", {}}, {"velocity", {}}};

  // motor stats
  std::array<MotorStatus, 6> motor_stats;
  double getJointVelocity(uint8_t id);
  double getJointPosition(uint8_t id);
  // position buffers
  // std::array<double, 6> motor_position_buffer_;

  // velocity buffers
  // std::array<double, 6> motor_previous_position_buffer_;
  // std::array<double, 6> motor_velocity_buffer_;
  // std::array<std::chrono::time_point<std::chrono::high_resolution_clock>, 6> previous_time_;

  // // can functions
  // bool sendData(uint8_t can_id, std::vector<uint8_t> data_vec);

  // // thread for can response
  // std::thread can_response_thread_;
  // std::atomic<bool> stop_thread_; // Atomic flag to control the thread's loop
  // void handleCANResponses();
  // std::chrono::time_point<std::chrono::high_resolution_clock> current_thread_time_;

  // // joint control
  // bool setJointPosition(uint8_t id, double position, double speed, double acceleration);
  // bool setBCJointPosition(double position[2], double speed, double acceleration);
  // double getJointPosition(uint8_t id);
  // double getJointVelocity(uint8_t id);

  // // motor functions
  // bool setMotorPosition(uint8_t can_id, double position, double speed, double acceleration);
  // bool requestPosition(uint8_t can_id);
  // bool requestStatus(uint8_t can_id);

  // loop stuff
  std::chrono::time_point<std::chrono::high_resolution_clock> current_loop_time_;
  std::chrono::time_point<std::chrono::high_resolution_clock> previous_loop_time_;

  // debug functions
  void printJointInfo(uint8_t id);
  void publishHardwareInfo();
};
} // namespace jonny_hardware_interface

#endif
