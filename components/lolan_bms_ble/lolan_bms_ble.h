#pragma once

#include "esphome/core/component.h"
#include "esphome/components/ble_client/ble_client.h"
#include "esphome/components/esp32_ble_tracker/esp32_ble_tracker.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/text_sensor/text_sensor.h"

#ifdef USE_ESP32

#include <esp_gattc_api.h>

namespace esphome {
namespace lolan_bms_ble {

static const uint16_t crcTable[256] = {
    0x0000, 0xC5A9, 0xCE9B, 0x0B32, 0xD8DF, 0x1D56, 0x1664, 0xD3CD, 0xF3E7, 0x364E, 0x3D7C, 0xF8D5, 0x2CC8, 0xE961,
    0xE253, 0x27FA, 0xAD87, 0x682E, 0x631C, 0xA6B5, 0x7558, 0xB0F1, 0xBBC3, 0x7E6A, 0x5980, 0x9C29, 0x971B, 0x52B2,
    0x815F, 0x44F6, 0x4FC4, 0x8A6D, 0x5B2E, 0x9E87, 0x95B5, 0x501C, 0x83F1, 0x4658, 0x4D6A, 0x88C3, 0xA8C9, 0x6D60,
    0x6652, 0xA3FB, 0x7016, 0xB5BF, 0xBE8D, 0x7B24, 0xF6A9, 0x3300, 0x3832, 0xFD9B, 0x2E76, 0xEBDF, 0xE0ED, 0x2544,
    0x054E, 0xC0E7, 0xCBD5, 0x0E7C, 0xDD91, 0x1838, 0x130A, 0xD6A3, 0xB65C, 0x73F5, 0x78C7, 0xBD6E, 0x6E83, 0xAB2A,
    0xA018, 0x65B1, 0x45BB, 0x8012, 0x8B20, 0x4E89, 0x9D64, 0x58CD, 0x53FF, 0x9656, 0x1BDB, 0xDE72, 0xD540, 0x10E9,
    0xC304, 0x06AD, 0x0D9F, 0xC836, 0xE83C, 0x2D95, 0x26A7, 0xE30E, 0x30E3, 0xF54A, 0xFE78, 0x3BD1, 0xED72, 0x28DB,
    0x23E9, 0xE640, 0x35AD, 0xF004, 0xFB36, 0x3E9F, 0x1E95, 0xDB3C, 0xD00E, 0x15A7, 0xC64A, 0x03E3, 0x08D1, 0xCD78,
    0x40F5, 0x855C, 0x8E6E, 0x4BC7, 0x982A, 0x5D83, 0x56B1, 0x9318, 0xB312, 0x76BB, 0x7D89, 0xB820, 0x6BCD, 0xAE64,
    0xA556, 0x60FF, 0x0F80, 0xCA29, 0xC11B, 0x04B2, 0xD75F, 0x12F6, 0x19C4, 0xDC6D, 0xFC67, 0x39CE, 0x32FC, 0xF755,
    0x24B8, 0xE111, 0xEA23, 0x2F8A, 0xA207, 0x67AE, 0x6C9C, 0xA935, 0x7AD8, 0xBF71, 0xB443, 0x71EA, 0x51E0, 0x9449,
    0x9F7B, 0x5AD2, 0x893F, 0x4C96, 0x47A4, 0x820D, 0x54AE, 0x9107, 0x9A35, 0x5F9C, 0x8C71, 0x49D8, 0x42EA, 0x8743,
    0xA749, 0x62E0, 0x69D2, 0xAC7B, 0x7F96, 0xBA3F, 0xB10D, 0x74A4, 0xF929, 0x3C80, 0x37B2, 0xF21B, 0x21F6, 0xE45F,
    0xEF6D, 0x2AC4, 0x0ACE, 0xCF67, 0xC455, 0x01FC, 0xD211, 0x17B8, 0x1C8A, 0xD923, 0xB9DC, 0x7C75, 0x7747, 0xB2EE,
    0x6103, 0xA4AA, 0xAF98, 0x6A31, 0x4A3B, 0x8F92, 0x84A0, 0x4109, 0x92E4, 0x574D, 0x5C7F, 0x99D6, 0x145B, 0xD1F2,
    0xDAC0, 0x1F69, 0xCC84, 0x092D, 0x021F, 0xC7B6, 0xE7BC, 0x2215, 0x2927, 0xEC8E, 0x3F63, 0xFACA, 0xF1F8, 0x3451,
    0xE2F2, 0x275B, 0x2C69, 0xE9C0, 0x3A2D, 0xFF84, 0xF4B6, 0x311F, 0x1115, 0xD4BC, 0xDF8E, 0x1A27, 0xC9CA, 0x0C63,
    0x0751, 0xC2F8, 0x4F75, 0x8ADC, 0x81EE, 0x4447, 0x97AA, 0x5203, 0x5931, 0x9C98, 0xBC92, 0x793B, 0x7209, 0xB7A0,
    0x644D, 0xA1E4, 0xAAD6, 0x6F7F};

namespace espbt = esphome::esp32_ble_tracker;

class LolanBmsBle : public esphome::ble_client::BLEClientNode, public PollingComponent {
 public:
  void gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
                           esp_ble_gattc_cb_param_t *param) override;
  void dump_config() override;
  void update() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void set_charging_binary_sensor(binary_sensor::BinarySensor *charging_binary_sensor) {
    charging_binary_sensor_ = charging_binary_sensor;
  }
  void set_discharging_binary_sensor(binary_sensor::BinarySensor *discharging_binary_sensor) {
    discharging_binary_sensor_ = discharging_binary_sensor;
  }

  void set_total_voltage_sensor(sensor::Sensor *total_voltage_sensor) { total_voltage_sensor_ = total_voltage_sensor; }
  void set_current_sensor(sensor::Sensor *current_sensor) { current_sensor_ = current_sensor; }
  void set_power_sensor(sensor::Sensor *power_sensor) { power_sensor_ = power_sensor; }
  void set_charging_power_sensor(sensor::Sensor *charging_power_sensor) {
    charging_power_sensor_ = charging_power_sensor;
  }
  void set_discharging_power_sensor(sensor::Sensor *discharging_power_sensor) {
    discharging_power_sensor_ = discharging_power_sensor;
  }

  void set_error_bitmask_sensor(sensor::Sensor *error_bitmask_sensor) { error_bitmask_sensor_ = error_bitmask_sensor; }
  void set_state_of_charge_sensor(sensor::Sensor *state_of_charge_sensor) {
    state_of_charge_sensor_ = state_of_charge_sensor;
  }
  void set_charging_cycles_sensor(sensor::Sensor *charging_cycles_sensor) {
    charging_cycles_sensor_ = charging_cycles_sensor;
  }

  void set_min_cell_voltage_sensor(sensor::Sensor *min_cell_voltage_sensor) {
    min_cell_voltage_sensor_ = min_cell_voltage_sensor;
  }
  void set_max_cell_voltage_sensor(sensor::Sensor *max_cell_voltage_sensor) {
    max_cell_voltage_sensor_ = max_cell_voltage_sensor;
  }
  void set_min_voltage_cell_sensor(sensor::Sensor *min_voltage_cell_sensor) {
    min_voltage_cell_sensor_ = min_voltage_cell_sensor;
  }
  void set_max_voltage_cell_sensor(sensor::Sensor *max_voltage_cell_sensor) {
    max_voltage_cell_sensor_ = max_voltage_cell_sensor;
  }
  void set_delta_cell_voltage_sensor(sensor::Sensor *delta_cell_voltage_sensor) {
    delta_cell_voltage_sensor_ = delta_cell_voltage_sensor;
  }
  void set_average_cell_voltage_sensor(sensor::Sensor *average_cell_voltage_sensor) {
    average_cell_voltage_sensor_ = average_cell_voltage_sensor;
  }
  void set_cell_voltage_sensor(uint8_t cell, sensor::Sensor *cell_voltage_sensor) {
    this->cells_[cell].cell_voltage_sensor_ = cell_voltage_sensor;
  }
  void set_temperature_sensor(uint8_t temperature, sensor::Sensor *temperature_sensor) {
    this->temperatures_[temperature].temperature_sensor_ = temperature_sensor;
  }
  void set_total_runtime_sensor(sensor::Sensor *total_runtime_sensor) { total_runtime_sensor_ = total_runtime_sensor; }
  void set_balancer_voltage_sensor(sensor::Sensor *balancer_voltage_sensor) {
    balancer_voltage_sensor_ = balancer_voltage_sensor;
  }
  void set_total_charged_capacity_sensor(sensor::Sensor *total_charged_capacity_sensor) {
    total_charged_capacity_sensor_ = total_charged_capacity_sensor;
  }
  void set_total_discharged_capacity_sensor(sensor::Sensor *total_discharged_capacity_sensor) {
    total_discharged_capacity_sensor_ = total_discharged_capacity_sensor;
  }

  void set_errors_text_sensor(text_sensor::TextSensor *errors_text_sensor) { errors_text_sensor_ = errors_text_sensor; }
  void set_total_runtime_formatted_text_sensor(text_sensor::TextSensor *total_runtime_formatted_text_sensor) {
    total_runtime_formatted_text_sensor_ = total_runtime_formatted_text_sensor;
  }

  void set_charging_switch(switch_::Switch *charging_switch) { charging_switch_ = charging_switch; }
  void set_discharging_switch(switch_::Switch *discharging_switch) { discharging_switch_ = discharging_switch; }

  void write_register(uint8_t address, uint16_t value);
  void on_lolan_bms_ble_data(const uint8_t &handle, const std::vector<uint8_t> &data);
  bool send_command(uint16_t function);
  bool send_factory_reset();

 protected:
  binary_sensor::BinarySensor *charging_binary_sensor_;
  binary_sensor::BinarySensor *discharging_binary_sensor_;

  sensor::Sensor *total_voltage_sensor_;
  sensor::Sensor *current_sensor_;
  sensor::Sensor *power_sensor_;
  sensor::Sensor *charging_power_sensor_;
  sensor::Sensor *discharging_power_sensor_;
  sensor::Sensor *error_bitmask_sensor_;
  sensor::Sensor *state_of_charge_sensor_;
  sensor::Sensor *charging_cycles_sensor_;
  sensor::Sensor *min_cell_voltage_sensor_;
  sensor::Sensor *max_cell_voltage_sensor_;
  sensor::Sensor *min_voltage_cell_sensor_;
  sensor::Sensor *max_voltage_cell_sensor_;
  sensor::Sensor *delta_cell_voltage_sensor_;
  sensor::Sensor *average_cell_voltage_sensor_;
  sensor::Sensor *total_runtime_sensor_;
  sensor::Sensor *balancer_voltage_sensor_;
  sensor::Sensor *total_charged_capacity_sensor_;
  sensor::Sensor *total_discharged_capacity_sensor_;

  switch_::Switch *charging_switch_;
  switch_::Switch *discharging_switch_;

  text_sensor::TextSensor *errors_text_sensor_;
  text_sensor::TextSensor *total_runtime_formatted_text_sensor_;

  struct Cell {
    sensor::Sensor *cell_voltage_sensor_{nullptr};
  } cells_[16];

  struct Temperature {
    sensor::Sensor *temperature_sensor_{nullptr};
  } temperatures_[2];

  std::vector<uint8_t> frame_buffer_;
  uint16_t char_notify_handle_;
  uint16_t char_notify2_handle_;
  uint16_t char_command_handle_;
  uint8_t next_command_{5};

  void decode_settings_data_(const std::vector<uint8_t> &data);
  void decode_status_data_(const std::vector<uint8_t> &data);
  void decode_cell_info_data_(const std::vector<uint8_t> &data);
  void publish_state_(binary_sensor::BinarySensor *binary_sensor, const bool &state);
  void publish_state_(sensor::Sensor *sensor, float value);
  void publish_state_(switch_::Switch *obj, const bool &state);
  void publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state);
  std::string bitmask_to_string_(const char *const messages[], const uint8_t &messages_size, const uint8_t &mask);

  bool check_bit_(uint16_t mask, uint16_t flag) { return (mask & flag) == flag; }

  std::string format_total_runtime_(const uint32_t value) {
    int seconds = (int) value;
    int years = seconds / (24 * 3600 * 365);
    seconds = seconds % (24 * 3600 * 365);
    int days = seconds / (24 * 3600);
    seconds = seconds % (24 * 3600);
    int hours = seconds / 3600;
    return (years ? to_string(years) + "y " : "") + (days ? to_string(days) + "d " : "") +
           (hours ? to_string(hours) + "h" : "");
  }

  float ieee_float_(uint32_t f) {
    static_assert(sizeof(float) == sizeof f, "`float` has a weird size.");
    float ret;
    std::memcpy(&ret, &f, sizeof(float));
    return ret;
  }

  uint16_t crc16(const uint8_t *data, size_t len) {
    uint16_t crc = 16;
    size_t i;

    for (i = 0; i < len; ++i) {
      crc = (crc << 8) ^ crcTable[((crc >> 7) ^ data[i]) & 0xFF];
    }

    return (crc & 0xFFFF) * 2;
  }
};

}  // namespace lolan_bms_ble
}  // namespace esphome

#endif
