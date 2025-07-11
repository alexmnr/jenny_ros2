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

#ifndef JENNY_HARDWARE_INTERFACE_HPP_
#define JENNY_HARDWARE_INTERFACE_HPP_JENNY_HARDWARE_INTERFACE_HPP_

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


using hardware_interface::return_type;

namespace jenny_hardware_interface {
using CallbackReturn =
    rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn;

class JennyHardwareInterface
    : public hardware_interface::SystemInterface {

  struct Config {
    std::string device = "can0";
    bool homing = false;
    bool debug = false;
    bool correction = true;
  };

  struct CANCommands {
    // Query commands
    static constexpr uint8_t QUERY_MOTOR = 0xF1; /**< Query motor command. */
    
    // Read commands
    static constexpr uint8_t READ_ENCODER = 0x31; /**< Read encoder value (addition). */
    static constexpr uint8_t READ_VELOCITY = 0x32; /**< Read velocity command. */
    static constexpr uint8_t READ_PULSES = 0x33; /**< Read pulses command. */
    static constexpr uint8_t READ_IO = 0x34; /**< Read IO command. */
    static constexpr uint8_t READ_RAW_ENCODER = 0x35; /**< Read raw encoder command. */
    static constexpr uint8_t READ_ERROR = 0x39; /**< Read error command. */
    static constexpr uint8_t READ_ENABLE_STATE = 0x3A; /**< Read enable state command. */
    static constexpr uint8_t READ_SHAFT_PROTECTION_STATE = 0x3E; /**< Read protection command. */

    // Set/Control commands
    static constexpr uint8_t CALIBRATE = 0x80; /**< Calibrate command. */
    static constexpr uint8_t SET_WORKING_MODE = 0x82; /**< Set working mode command. */
    static constexpr uint8_t SET_CURRENT = 0x83; /**< Set current command. */
    static constexpr uint8_t SET_SUBDIVISIONS = 0x84; /**< Set subdivisions command. */
    static constexpr uint8_t SET_ENABLE_SETTINGS = 0x85; /**< Set enable settings command. */
    static constexpr uint8_t SET_DIRECTION = 0x86; /**< Set direction command. */
    static constexpr uint8_t ENABLE_SHAFT_PROTECTION = 0x88; /**< Enable protection command. */
    static constexpr uint8_t RELEASE_SHAFT_PROTECTION = 0x3D; /**< Release the motor shaft locked-rotor protection state. */
    static constexpr uint8_t SET_HOME_PARAMS = 0x90; /**< Set home parameters command. */
    static constexpr uint8_t GO_HOME = 0x91; /**< Go home command. */
    static constexpr uint8_t SET_ZERO_POSITION = 0x92; /**< Set zero command. */
    
    // Motion commands
    static constexpr uint8_t ENABLE_MOTOR = 0xF3; /**< Enable motor command. */
    static constexpr uint8_t RELATIVE_POSITION = 0xF4; /**< Relative position command. */
    static constexpr uint8_t ABSOLUTE_POSITION = 0xF5; /**< Absolute position command. */
    static constexpr uint8_t SPEED_CONTROL = 0xF6; /**< Speed control command. */
    static constexpr uint8_t EMERGENCY_STOP = 0xF7; /**< Emergency stop command. */
    static constexpr uint8_t POSITION_CONTROL = 0xFD; /**< Position control command. */
    static constexpr uint8_t ABSOLUTE_POSITION_PULSE = 0xFE; /**< Absolute position pulse command. */
  };

  struct MotorConstants {
    // Encoder constants
    static constexpr double ENCODER_STEPS = 16384.0;  // 0x4000 steps per revolution
    static constexpr double DEGREES_PER_REVOLUTION = 360.0;
    static constexpr double RADIANS_PER_REVOLUTION = 2.0 * M_PI;

    // Motor speed limits (RPM)
    static constexpr double MAX_RPM_OPEN = 400.0;    // OPEN mode max speed
    static constexpr double MAX_RPM_CLOSE = 1500.0;  // CLOSE mode max speed
    static constexpr double MAX_RPM_vFOC = 3000.0;   // vFOC mode max speed

    // Conversion factors
    static constexpr double RPM_TO_RADPS = M_PI / 30.0;     // RPM to rad/s (π/30)
    static constexpr double RADPS_TO_RPM = 30.0 / M_PI;     // rad/s to RPM (30/π)
    static constexpr double DEG_TO_RAD = M_PI / 180.0;      // degrees to radians
    static constexpr double RAD_TO_DEG = 180.0 / M_PI;      // radians to degrees
  };

  struct RobotConstants {
    static constexpr double AXIS_RATIO[6] = {13.5, 150, 150, 45, 45, 45};
    static constexpr double AXIS_SET_INVERTED[6] = {1, -1, 1, -1, -1, 1};
    static constexpr double AXIS_GET_INVERTED[6] = {1, -1, -1, -1, -1, 1};
    static constexpr double AXIS_ACCELERATION[6] = {0, 0, 0, 0, 0, 0};
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
  double trigger_joint_command_threshold_ = 1e-3;
  bool ready = false;
  Config config_;

  // can stuff
  std::unique_ptr<drivers::socketcan::SocketCanSender> sender;
  std::unique_ptr<drivers::socketcan::SocketCanReceiver> receiver;

  /// Output from ros2_control
  std::vector<double> joint_position_command_;
  std::vector<double> joint_velocities_command_;
  /// Input to ros2_control
  std::vector<double> joint_position_;
  std::vector<double> joint_velocities_;
  // more ros2_control
  std::unordered_map<std::string, std::vector<std::string>> joint_interfaces = {
      {"position", {}}, {"velocity", {}}};

  // threads
  std::thread can_response_thread_;
  std::atomic<bool> stop_thread_; // Atomic flag to control the thread's loop

  // position buffers
  std::array<double, 6> motor_position_buffer_;

  // velocity buffers
  std::array<double, 6> motor_previous_position_buffer_;
  std::array<double, 6> motor_velocity_buffer_;
  std::chrono::time_point<std::chrono::high_resolution_clock> current_time_;
  std::chrono::time_point<std::chrono::high_resolution_clock> previous_time_;

  // can functions
  bool sendData(uint8_t id, std::vector<uint8_t> data_vec);

  // joint control
  bool setJointPosition(uint8_t id, double position, double speed, double acceleration);
  bool setBCJointPosition(double position[2], double speed, double acceleration);
  double getJointPosition(uint8_t id);
  double getJointVelocity(uint8_t id);
  void calculateMotorVelocities();

  // motor functions
  bool setMotorPosition(uint8_t id, double position, double speed, double acceleration);
  void handleCANResponses();
  bool requestPosition(uint8_t id);

  // debug functions
  void printJointInfo(uint8_t id);
};
} // namespace jenny_hardware_interface

#endif
