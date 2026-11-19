#pragma once

#include "esphome/core/component.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/text_sensor/text_sensor.h"

#ifdef USE_ESP32
#include "esphome/components/ble_client/ble_client.h"
#include "esphome/components/esp32_ble_tracker/esp32_ble_tracker.h"
#include <esp_gattc_api.h>
namespace espbt = esphome::esp32_ble_tracker;
#endif

namespace esphome::lolan_bms_ble {

class LolanBmsBle :
#ifdef USE_ESP32
    public esphome::ble_client::BLEClientNode,
#endif
    public PollingComponent {
 public:
#ifdef USE_ESP32
  void gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
                           esp_ble_gattc_cb_param_t *param) override;
#endif
  void dump_config() override;
  void update() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  SUB_BINARY_SENSOR(online_status)
  SUB_BINARY_SENSOR(charging)
  SUB_BINARY_SENSOR(discharging)

  SUB_SENSOR(total_voltage)
  SUB_SENSOR(current)
  SUB_SENSOR(power)
  SUB_SENSOR(charging_power)
  SUB_SENSOR(discharging_power)
  SUB_SENSOR(error_bitmask)
  SUB_SENSOR(state_of_charge)
  SUB_SENSOR(charging_cycles)
  SUB_SENSOR(min_cell_voltage)
  SUB_SENSOR(max_cell_voltage)
  SUB_SENSOR(min_voltage_cell)
  SUB_SENSOR(max_voltage_cell)
  SUB_SENSOR(delta_cell_voltage)
  SUB_SENSOR(average_cell_voltage)
  SUB_SENSOR(total_runtime)
  SUB_SENSOR(balancer_voltage)
  SUB_SENSOR(total_charged_capacity)
  SUB_SENSOR(total_discharged_capacity)

  SUB_TEXT_SENSOR(errors)
  SUB_TEXT_SENSOR(total_runtime_formatted)

  SUB_SWITCH(charging)
  SUB_SWITCH(discharging)

  void set_cell_voltage_sensor(uint8_t cell, sensor::Sensor *cell_voltage_sensor) {
    this->cells_[cell].cell_voltage_sensor_ = cell_voltage_sensor;
  }
  void set_temperature_sensor(uint8_t temperature, sensor::Sensor *temperature_sensor) {
    this->temperatures_[temperature].temperature_sensor_ = temperature_sensor;
  }

  void on_lolan_bms_ble_data(const uint8_t &handle, const std::vector<uint8_t> &data);
  bool send_command(uint16_t function);
  bool send_factory_reset();
  void set_password(uint32_t password) { this->password_ = password; }

 protected:
  struct Cell {
    sensor::Sensor *cell_voltage_sensor_{nullptr};
  } cells_[16];

  struct Temperature {
    sensor::Sensor *temperature_sensor_{nullptr};
  } temperatures_[2];

  uint8_t no_response_count_{0};
  uint16_t char_notify_handle_{0};
  uint16_t char_command_handle_{0};
  uint32_t password_ = 12345678;

  void decode_settings_data_(const std::vector<uint8_t> &data);
  void decode_status_data_(const std::vector<uint8_t> &data);
  void decode_cell_info_data_(const std::vector<uint8_t> &data);
  void decode_confirmations_(const std::vector<uint8_t> &data);
  void publish_device_unavailable_();
  void reset_online_status_tracker_();
  void track_online_status_();
  void invalidate_binary_sensor_(binary_sensor::BinarySensor *binary_sensor);
  std::string bitmask_to_string_(const char *const messages[], const uint8_t &messages_size, const uint8_t &mask);

  bool check_bit_(uint16_t mask, uint16_t flag) { return (mask & flag) == flag; }

  std::string format_total_runtime_(const uint32_t value) {
    int seconds = (int) value;
    int years = seconds / (24 * 3600 * 365);
    seconds = seconds % (24 * 3600 * 365);
    int days = seconds / (24 * 3600);
    seconds = seconds % (24 * 3600);
    int hours = seconds / 3600;

    char buf[16];
    int len = 0;
    if (years)
      len += snprintf(buf + len, sizeof(buf) - len, "%dy ", years);
    if (days)
      len += snprintf(buf + len, sizeof(buf) - len, "%dd ", days);
    if (hours)
      len += snprintf(buf + len, sizeof(buf) - len, "%dh", hours);

    return std::string(buf, len);
  }

  float ieee_float_(uint32_t f) {
    static_assert(sizeof(float) == sizeof f, "`float` has a weird size.");
    float ret;
    std::memcpy(&ret, &f, sizeof(float));
    return ret;
  }
};

}  // namespace esphome::lolan_bms_ble
