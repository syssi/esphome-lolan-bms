#pragma once
#include <cstdint>
#include <vector>
#include "esphome/components/lolan_bms_ble/lolan_bms_ble.h"
#include "esphome/components/button/button.h"

namespace esphome::lolan_bms_ble::testing {

class TestableLolanBmsBle : public LolanBmsBle {
 public:
  using LolanBmsBle::decode_cell_info_data_;
  using LolanBmsBle::decode_confirmations_;
  using LolanBmsBle::decode_settings_data_;
  using LolanBmsBle::decode_status_data_;
  using LolanBmsBle::on_lolan_bms_ble_data;
  using LolanBmsBle::send_command;
  using LolanBmsBle::send_factory_reset;
  using LolanBmsBle::char_command_handle_;
};

class TestSwitch : public switch_::Switch {
 public:
  void set_parent(LolanBmsBle *parent) { this->parent_ = parent; }
  void set_command_turn_on(uint16_t cmd) { this->command_turn_on_ = cmd; }
  void set_command_turn_off(uint16_t cmd) { this->command_turn_off_ = cmd; }

 protected:
  void write_state(bool state) override {
    if (this->parent_->send_command(state ? this->command_turn_on_ : this->command_turn_off_)) {
      this->publish_state(state);
    }
  }
  LolanBmsBle *parent_{nullptr};
  uint16_t command_turn_on_{0};
  uint16_t command_turn_off_{0};
};

class TestButton : public button::Button, public Component {
 public:
  void set_parent(LolanBmsBle *parent) { this->parent_ = parent; }
  void set_holding_register(uint16_t reg) { this->holding_register_ = reg; }

 protected:
  void press_action() override { this->parent_->send_command(this->holding_register_); }
  LolanBmsBle *parent_{nullptr};
  uint16_t holding_register_{0};
};

// ── Status frames from tests/esp32-ble-example-faker.yaml ─────────────────────
// Frame type 0x01 (40 bytes)
// Contains: voltage, current, temperatures, capacity, runtime, cycles, SOC, switch states

// Status frame with charging + discharging enabled (flags=0x03)
// SOC=39%  voltage=23.13V  current=0A  temp1=26.98°C  temp2=25.65°C
// total_charged=49.90Ah  total_discharged=78.79Ah  runtime=3554896s  cycles=1
// charging=ON  discharging=ON  errors=0
static const std::vector<uint8_t> STATUS_FRAME_CHARGING_AND_DISCHARGING = {
    0x01, 0x00, 0x03, 0x00,  // frame_type, reserved, flags=0x03, error=0x00
    0x41, 0xb9, 0x06, 0x15,  // total_voltage = 23.13V (IEEE float)
    0x00, 0x00, 0x00, 0x00,  // negative_current = 0A (IEEE float)
    0x00, 0x00, 0x00, 0x00,  // current = 0A (IEEE float)
    0x41, 0xd7, 0xce, 0x40,  // temp1 = 26.98°C (IEEE float)
    0x41, 0xcd, 0x38, 0x60,  // temp2 = 25.65°C (IEEE float)
    0x42, 0x9d, 0x93, 0x20,  // total_discharged = 78.79Ah (IEEE float)
    0x42, 0x47, 0x9e, 0x44,  // total_charged = 49.90Ah (IEEE float)
    0x00, 0x36, 0x3e, 0x50,  // total_runtime = 3554896s (uint32)
    0x00, 0x01, 0x00, 0x27,  // cycles = 1, SOC = 39%
};

static const std::vector<uint8_t> STATUS_FRAME_ONLY_CHARGING_ENABLED = {
    0x01, 0x00, 0x02, 0x00,  // frame_type, reserved, flags=0x02, error=0x00
    0x41, 0xb9, 0x02, 0x90,  // total_voltage = 23.13V (IEEE float)
    0x00, 0x00, 0x00, 0x00,  // negative_current = 0A (IEEE float)
    0x00, 0x00, 0x00, 0x00,  // current = 0A (IEEE float)
    0x41, 0xd7, 0xd9, 0xb0,  // temp1 = 26.98°C (IEEE float)
    0x41, 0xcd, 0x60, 0x00,  // temp2 = 25.67°C (IEEE float)
    0x42, 0x9d, 0x93, 0x20,  // total_discharged = 78.79Ah (IEEE float)
    0x42, 0x47, 0x9e, 0x44,  // total_charged = 49.90Ah (IEEE float)
    0x00, 0x36, 0x3e, 0x5a,  // total_runtime = 3554906s (uint32)
    0x00, 0x01, 0x00, 0x27,  // cycles = 1, SOC = 39%
};

static const std::vector<uint8_t> STATUS_FRAME_ONLY_DISCHARGING_ENABLED = {
    0x01, 0x00, 0x01, 0x00,  // frame_type, reserved, flags=0x01, error=0x00
    0x41, 0xb9, 0x9d, 0x71,  // total_voltage = 23.20V (IEEE float)
    0x00, 0x00, 0x00, 0x00,  // negative_current = 0A (IEEE float)
    0x00, 0x00, 0x00, 0x00,  // current = 0A (IEEE float)
    0x41, 0xd8, 0x18, 0x90,  // temp1 = 27.01°C (IEEE float)
    0x41, 0xcd, 0xfe, 0x50,  // temp2 = 25.75°C (IEEE float)
    0x42, 0x9d, 0x93, 0x20,  // total_discharged = 78.79Ah (IEEE float)
    0x42, 0x47, 0xaf, 0xe4,  // total_charged = 49.92Ah (IEEE float)
    0x00, 0x36, 0x3e, 0x7b,  // total_runtime = 3554939s (uint32)
    0x00, 0x01, 0x00, 0x27,  // cycles = 1, SOC = 39%
};

// ── Cell info frame from tests/esp32-ble-example-faker.yaml ─────────────────
// Frame type 0x02 (88 bytes)
// Contains: cell count, cell voltages, balancer voltage

// Cell info frame with 7 cells
// cell_1=3.300V  cell_2=3.300V  cell_3=3.301V  cell_4=3.300V  cell_5=3.301V  cell_6=3.301V  cell_7=3.301V
// balancer=3.350V  min=3.300V  max=3.301V  avg=3.3006V
static const std::vector<uint8_t> CELL_INFO_FRAME_1 = {
    0x02, 0x00, 0x07, 0x00,  // frame_type, reserved, cell_count=7, reserved
    0x00, 0x00, 0x00, 0x00,  // reserved
    0x00, 0x00, 0x00, 0x00,  // reserved
    0x00, 0x00, 0x00, 0x00,  // reserved
    0x00, 0x00, 0x00, 0x00,  // reserved (balancing mask)
    0x40, 0x56, 0x66, 0x66,  // balancer_voltage = 3.350V (IEEE float)
    0x40, 0x53, 0x30, 0xa4,  // cell_1 = 3.300V (IEEE float)
    0x40, 0x53, 0x3a, 0xe1,  // cell_2 = 3.300V (IEEE float)
    0x40, 0x53, 0x45, 0x1f,  // cell_3 = 3.301V (IEEE float)
    0x40, 0x53, 0x38, 0x52,  // cell_4 = 3.300V (IEEE float)
    0x40, 0x53, 0x45, 0x1f,  // cell_5 = 3.301V (IEEE float)
    0x40, 0x53, 0x45, 0x1f,  // cell_6 = 3.301V (IEEE float)
    0x40, 0x53, 0x3d, 0x71,  // cell_7 = 3.301V (IEEE float)
    0x00, 0x00, 0x00, 0x00,  // cell_8 = 0V (not installed)
    0x00, 0x00, 0x00, 0x00,  // cell_9 = 0V (not installed)
    0x00, 0x00, 0x00, 0x00,  // cell_10 = 0V (not installed)
    0x00, 0x00, 0x00, 0x00,  // cell_11 = 0V (not installed)
    0x00, 0x00, 0x00, 0x00,  // cell_12 = 0V (not installed)
    0x00, 0x00, 0x00, 0x00,  // cell_13 = 0V (not installed)
    0x00, 0x00, 0x00, 0x00,  // cell_14 = 0V (not installed)
    0x00, 0x00, 0x00, 0x00,  // cell_15 = 0V (not installed)
    0x00, 0x00, 0x00, 0x00,  // cell_16 = 0V (not installed)
};

// ── Settings frame from tests/esp32-ble-example-faker.yaml ───────────────────
// Frame type 0x03 (108 bytes)
// Contains: battery settings, protection limits, serial number, CRC

// Settings frame with various parameters
// cell_count=7  max_charging_current=50A  max_discharging_current=80A  cycle_count=1
static const std::vector<uint8_t> SETTINGS_FRAME_1 = {
    0x03, 0x6c,              // frame_type=0x03, unknown=0x6c
    0x00, 0xbc, 0x61, 0x4e,  // BLE password = 12345678 (uint32, bytes 2-5)
    0x0c, 0x00,              // output_voltage_offset=12, input_voltage_offset=0
    0x00, 0x32,              // max_charging_current = 50A (uint16)
    0x00, 0x50,              // max_discharging_current = 80A (uint16)
    0x08, 0x0f, 0x07, 0x01,  // charge_zero=8, discharge_zero=15, cell_count=7, battery_type=1
    0xff, 0xfa, 0x00, 0x01,  // mNC2=0xfffa, cycle_count=1
    0x42, 0x51, 0x09, 0x41,  // battery_capacity = 52.26Ah (IEEE float)
    0x40, 0x56, 0x66, 0x66,  // start_balancing_voltage = 3.350V (IEEE float)
    0x3f, 0x80, 0x00, 0x00,  // unknown28 = 1.0 (IEEE float)
    0x40, 0xb7, 0x95, 0x81,  // ref_discharging_voltage = 5.737V (IEEE float)
    0x40, 0xb2, 0xb8, 0x52,  // ref_charging_voltage = 5.585V (IEEE float)
    0x41, 0x9a, 0x00, 0x00,  // pack_undervoltage_protection = 19.25V (IEEE float)
    0x41, 0xcc, 0x00, 0x00,  // pack_overvoltage_protection = 25.5V (IEEE float)
    0x40, 0x30, 0x00, 0x00,  // cell_undervoltage_protection = 2.75V (IEEE float)
    0x40, 0x69, 0x99, 0x9a,  // cell_overvoltage_protection = 3.65V (IEEE float)
    0x42, 0x82, 0x00, 0x00,  // mosfet_overtemperature_protection = 65°C (IEEE float)
    0x42, 0x82, 0x00, 0x00,  // circuit_temperature_protection = 65°C (IEEE float)
    0x46, 0x1c, 0x40, 0x00,  // temperature_adjustment = 10000 (IEEE float)
    0xcc, 0x10, 0xf1, 0x52,  // serial_number = 3423662418 (uint32)
    0x01, 0x34, 0x8b, 0x9a,  // software_date = 20220826 (uint32)
    0x40, 0x30, 0x00, 0x00,  // lowpower_voltage = 2.75V (IEEE float)
    0x40, 0x53, 0x33, 0x33,  // battery4_adjustment = 3.3 (IEEE float)
    0x3c, 0x23, 0xd7, 0x0a,  // voltage_equal = 0.01V (IEEE float)
    0x41, 0xc0, 0xcc, 0xcd,  // overcharge_release_voltage = 24.1V (IEEE float)
    0x40, 0x5c, 0xcc, 0xcd,  // overcharge_cell_voltage = 3.45V (IEEE float)
    0x41, 0xa2, 0x66, 0x66,  // discharge_release_voltage = 20.3V (IEEE float)
    0x40, 0x39, 0x99, 0x9a,  // discharge_cell_voltage = 2.9V (IEEE float)
    0x35, 0xe2,              // CRC16
    0x5a, 0xa5,              // unknown
};

// ── Invalid frames for rejection logic ────────────────────────────────────────

static const std::vector<uint8_t> INVALID_FRAME_TOO_SHORT = {0x01, 0x00};
static const std::vector<uint8_t> INVALID_FRAME_WRONG_CRC = {0x01, 0x00, 0x03, 0x00, 0x41, 0xb9, 0x00, 0x00};

// ── Confirmation frames from tests/esp32-ble-example-faker.yaml ──────────────
// Frame type 0x21, 0x2f, 0x35 (4 bytes each)
// Responses to write commands

static const std::vector<uint8_t> CONFIRMATION_PASSWORD_ERROR = {0x21, 0x1d, 0x1a, 0xe3};
static const std::vector<uint8_t> CONFIRMATION_SETTINGS_SUCCESS = {0x21, 0xb7, 0x33, 0x31};
static const std::vector<uint8_t> CONFIRMATION_BLUETOOTH_NAME_SUCCESS = {0x21, 0x98, 0xae, 0xc5};
static const std::vector<uint8_t> CONFIRMATION_SETTING_SUCCESS = {0x2f, 0x99, 0x83, 0xca};
static const std::vector<uint8_t> CONFIRMATION_DATA_ERROR = {0x35, 0x8f, 0x03, 0x22};
static const std::vector<uint8_t> CONFIRMATION_PASSWORD_CHANGED_SUCCESS = {0x35, 0x95, 0x7f, 0x4a};
static const std::vector<uint8_t> CONFIRMATION_CALIBRATION_SUCCESS = {0x35, 0x8b, 0x76, 0x35};

}  // namespace esphome::lolan_bms_ble::testing
