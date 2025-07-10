#include "lolan_bms_ble.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace lolan_bms_ble {

static const char *const TAG = "lolan_bms_ble";

static const uint16_t LOLAN_BMS_SERVICE_UUID = 0xFFE0;
static const uint16_t LOLAN_BMS_SERVICE_UUID2 = 0xFFF0;
static const uint16_t LOLAN_BMS_NOTIFY_CHARACTERISTIC_UUID = 0xFFE1;    // handle 0x10 & 0x11
static const uint16_t LOLAN_BMS_NOTIFY2_CHARACTERISTIC_UUID2 = 0xFFF1;  // handle
static const uint16_t LOLAN_BMS_CONTROL_CHARACTERISTIC_UUID = 0xFFE2;   // handle 0x13
static const uint16_t LOLAN_BMS_CONTROL_CHARACTERISTIC_UUID2 = 0xFFF2;  // handle

static const uint8_t LOLAN_FRAME_TYPE_STATUS = 0x01;     // 40 bytes
static const uint8_t LOLAN_FRAME_TYPE_CELL_INFO = 0x02;  // 40 bytes
static const uint8_t LOLAN_FRAME_TYPE_SETTINGS = 0x03;   // 108 bytes

static const uint16_t LOLAN_COMMAND_REQ_STATUS = 0xc565;     // 0xc501
static const uint16_t LOLAN_COMMAND_REQ_CELL_INFO = 0x5b65;  // 0x5b02
static const uint16_t LOLAN_COMMAND_REQ_SETTINGS = 0x5600;   // 0x5656

static const uint8_t MAX_RESPONSE_SIZE = 108;
static const uint8_t MAX_KNOWN_CELL_COUNT = 16;

static const uint8_t ERRORS_SIZE = 8;
static const char *const ERRORS[ERRORS_SIZE] = {
    "Total voltage overcharge protection",   // 0000 0001
    "Single voltage overcharge protection",  // 0000 0010
    "Charge overcurrent protection",         // 0000 0100
    "Discharge overcurrent protection",      // 0000 1000
    "Total voltage overdischarge",           // 0001 0000
    "Single voltage overdischarge",          // 0010 0000
    "High temperature protection",           // 0100 0000
    "Short circuit protection",              // 1000 0000
};

static const uint16_t CRC_TABLE[256] = {
    0x0000, 0xC599, 0xCEAB, 0x0B32, 0xD8CF, 0x1D56, 0x1664, 0xD3FD, 0xF407, 0x319E, 0x3AAC, 0xFF35, 0x2CC8, 0xE951,
    0xE263, 0x27FA, 0xAD97, 0x680E, 0x633C, 0xA6A5, 0x7558, 0xB0C1, 0xBBF3, 0x7E6A, 0x5990, 0x9C09, 0x973B, 0x52A2,
    0x815F, 0x44C6, 0x4FF4, 0x8A6D, 0x5B2E, 0x9EB7, 0x9585, 0x501C, 0x83E1, 0x4678, 0x4D4A, 0x88D3, 0xAF29, 0x6AB0,
    0x6182, 0xA41B, 0x77E6, 0xB27F, 0xB94D, 0x7CD4, 0xF6B9, 0x3320, 0x3812, 0xFD8B, 0x2E76, 0xEBEF, 0xE0DD, 0x2544,
    0x02BE, 0xC727, 0xCC15, 0x098C, 0xDA71, 0x1FE8, 0x14DA, 0xD143, 0xF3C5, 0x365C, 0x3D6E, 0xF8F7, 0x2B0A, 0xEE93,
    0xE5A1, 0x2038, 0x07C2, 0xC25B, 0xC969, 0x0CF0, 0xDF0D, 0x1A94, 0x11A6, 0xD43F, 0x5E52, 0x9BCB, 0x90F9, 0x5560,
    0x869D, 0x4304, 0x4836, 0x8DAF, 0xAA55, 0x6FCC, 0x64FE, 0xA167, 0x729A, 0xB703, 0xBC31, 0x79A8, 0xA8EB, 0x6D72,
    0x6640, 0xA3D9, 0x7024, 0xB5BD, 0xBE8F, 0x7B16, 0x5CEC, 0x9975, 0x9247, 0x57DE, 0x8423, 0x41BA, 0x4A88, 0x8F11,
    0x057C, 0xC0E5, 0xCBD7, 0x0E4E, 0xDDB3, 0x182A, 0x1318, 0xD681, 0xF17B, 0x34E2, 0x3FD0, 0xFA49, 0x29B4, 0xEC2D,
    0xE71F, 0x2286, 0xA213, 0x678A, 0x6CB8, 0xA921, 0x7ADC, 0xBF45, 0xB477, 0x71EE, 0x5614, 0x938D, 0x98BF, 0x5D26,
    0x8EDB, 0x4B42, 0x4070, 0x85E9, 0x0F84, 0xCA1D, 0xC12F, 0x04B6, 0xD74B, 0x12D2, 0x19E0, 0xDC79, 0xFB83, 0x3E1A,
    0x3528, 0xF0B1, 0x234C, 0xE6D5, 0xEDE7, 0x287E, 0xF93D, 0x3CA4, 0x3796, 0xF20F, 0x21F2, 0xE46B, 0xEF59, 0x2AC0,
    0x0D3A, 0xC8A3, 0xC391, 0x0608, 0xD5F5, 0x106C, 0x1B5E, 0xDEC7, 0x54AA, 0x9133, 0x9A01, 0x5F98, 0x8C65, 0x49FC,
    0x42CE, 0x8757, 0xA0AD, 0x6534, 0x6E06, 0xAB9F, 0x7862, 0xBDFB, 0xB6C9, 0x7350, 0x51D6, 0x944F, 0x9F7D, 0x5AE4,
    0x8919, 0x4C80, 0x47B2, 0x822B, 0xA5D1, 0x6048, 0x6B7A, 0xAEE3, 0x7D1E, 0xB887, 0xB3B5, 0x762C, 0xFC41, 0x39D8,
    0x32EA, 0xF773, 0x248E, 0xE117, 0xEA25, 0x2FBC, 0x0846, 0xCDDF, 0xC6ED, 0x0374, 0xD089, 0x1510, 0x1E22, 0xDBBB,
    0x0AF8, 0xCF61, 0xC453, 0x01CA, 0xD237, 0x17AE, 0x1C9C, 0xD905, 0xFEFF, 0x3B66, 0x3054, 0xF5CD, 0x2630, 0xE3A9,
    0xE89B, 0x2D02, 0xA76F, 0x62F6, 0x69C4, 0xAC5D, 0x7FA0, 0xBA39, 0xB10B, 0x7492, 0x5368, 0x96F1, 0x9DC3, 0x585A,
    0x8BA7, 0x4E3E, 0x450C, 0x8095};

static uint16_t crc16_lolan(const uint8_t *data, size_t size) {
  uint16_t crc = 16;
  uint8_t r = 0;

  for (size_t n = 0; n < size; n++) {
    r = (crc >> 7) ^ data[n];
    r = r & 0xFF;
    crc = (crc << 8) ^ CRC_TABLE[r];
    crc = crc & 0xFFFF;
  }

  crc = 2 * (crc & 0xFFFF);

  // Apply 16-bit left rotation by 3 positions
  return ((crc << 3) | (crc >> 13)) & 0xFFFF;
}

void LolanBmsBle::gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
                                      esp_ble_gattc_cb_param_t *param) {
  switch (event) {
    case ESP_GATTC_OPEN_EVT: {
      break;
    }
    case ESP_GATTC_DISCONNECT_EVT: {
      this->node_state = espbt::ClientState::IDLE;

      // this->publish_state_(this->voltage_sensor_, NAN);

      if (this->char_notify_handle_ != 0) {
        auto status = esp_ble_gattc_unregister_for_notify(this->parent()->get_gattc_if(),
                                                          this->parent()->get_remote_bda(), this->char_notify_handle_);
        if (status) {
          ESP_LOGW(TAG, "esp_ble_gattc_unregister_for_notify failed, status=%d", status);
        }
      }
      this->char_notify_handle_ = 0;
      this->char_command_handle_ = 0;

      break;
    }
    case ESP_GATTC_SEARCH_CMPL_EVT: {
      auto *char_notify =
          this->parent_->get_characteristic(LOLAN_BMS_SERVICE_UUID, LOLAN_BMS_NOTIFY_CHARACTERISTIC_UUID);
      if (char_notify == nullptr) {
        ESP_LOGE(TAG, "[%s] No notify service found at device, not an Lolan BMS..?",
                 this->parent_->address_str().c_str());
        break;
      }
      this->char_notify_handle_ = char_notify->handle;

      auto status = esp_ble_gattc_register_for_notify(this->parent()->get_gattc_if(), this->parent()->get_remote_bda(),
                                                      char_notify->handle);
      if (status) {
        ESP_LOGW(TAG, "esp_ble_gattc_register_for_notify failed, status=%d", status);
      }

      auto *char_command =
          this->parent_->get_characteristic(LOLAN_BMS_SERVICE_UUID, LOLAN_BMS_CONTROL_CHARACTERISTIC_UUID);
      if (char_command == nullptr) {
        ESP_LOGE(TAG, "[%s] No control service found at device, not an Lolan BMS..?",
                 this->parent_->address_str().c_str());
        break;
      }
      this->char_command_handle_ = char_command->handle;
      break;
    }
    case ESP_GATTC_REG_FOR_NOTIFY_EVT: {
      this->node_state = espbt::ClientState::ESTABLISHED;

      this->send_command(LOLAN_COMMAND_REQ_STATUS);
      break;
    }
    case ESP_GATTC_NOTIFY_EVT: {
      ESP_LOGV(TAG, "Notification received (handle 0x%02X): %s", param->notify.handle,
               format_hex_pretty(param->notify.value, param->notify.value_len).c_str());

      std::vector<uint8_t> data(param->notify.value, param->notify.value + param->notify.value_len);

      this->on_lolan_bms_ble_data(param->notify.handle, data);
      break;
    }
    default:
      break;
  }
}

void LolanBmsBle::update() {
  if (this->node_state != espbt::ClientState::ESTABLISHED) {
    ESP_LOGW(TAG, "[%s] Not connected", this->parent_->address_str().c_str());
    return;
  }

  this->send_command(LOLAN_COMMAND_REQ_STATUS);
}

void LolanBmsBle::on_lolan_bms_ble_data(const uint8_t &handle, const std::vector<uint8_t> &data) {
  if (data.size() > MAX_RESPONSE_SIZE) {
    ESP_LOGW(TAG, "Invalid response received: %s", format_hex_pretty(&data.front(), data.size()).c_str());
    return;
  }

  uint8_t frame_type = data[0];

  switch (frame_type) {
    case LOLAN_FRAME_TYPE_STATUS:
      this->decode_status_data_(data);
      this->send_command(LOLAN_COMMAND_REQ_CELL_INFO);
      break;
    case LOLAN_FRAME_TYPE_CELL_INFO:
      this->decode_cell_info_data_(data);
      break;
    case LOLAN_FRAME_TYPE_SETTINGS:
      this->decode_settings_data_(data);
      break;
    case 0x35:
    case 0x21:
    case 0x2f:
      this->decode_confirmations_(data);
      break;
    default:
      ESP_LOGW(TAG, "Unhandled response received (frame_type 0x%02X): %s", frame_type,
               format_hex_pretty(&data.front(), data.size()).c_str());
  }
}

void LolanBmsBle::decode_status_data_(const std::vector<uint8_t> &data) {
  auto lolan_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 0]) << 8) | (uint16_t(data[i + 1]) << 0);
  };
  auto lolan_get_32bit = [&](size_t i) -> uint32_t {
    return (uint32_t(lolan_get_16bit(i + 0)) << 16) | (uint32_t(lolan_get_16bit(i + 2)) << 0);
  };

  ESP_LOGI(TAG, "Status frame received");
  ESP_LOGD(TAG, "  %s", format_hex_pretty(&data.front(), data.size()).c_str());

  if (data.size() < 40) {
    ESP_LOGW(TAG, "Invalid status frame length: %d", data.size());
    return;
  }

  // Byte Len Payload              Description                      Unit  Precision
  //  0    1  0x01                   Frame type
  //  1    1  0x00
  //  2    1  0x03                   Switch bitmask
  this->publish_state_(this->discharging_binary_sensor_, (bool) check_bit_(data[2], 1));
  this->publish_state_(this->charging_binary_sensor_, (bool) check_bit_(data[2], 2));
  this->publish_state_(this->discharging_switch_, (bool) check_bit_(data[2], 1));
  this->publish_state_(this->charging_switch_, (bool) check_bit_(data[2], 2));

  //  3    1  0x00                   Status bitmask
  this->publish_state_(this->error_bitmask_sensor_, data[3] * 1.0f);
  this->publish_state_(this->errors_text_sensor_, bitmask_to_string_(ERRORS, ERRORS_SIZE, data[3]));

  //  4    4  0x41 0xb8 0xe6 0xb8    Total voltage
  float total_voltage = ieee_float_(lolan_get_32bit(4));
  this->publish_state_(this->total_voltage_sensor_, total_voltage);

  //  8    4  0x00 0x00 0x00 0x00    Negative current
  //  12   4  0x00 0x00 0x00 0x00    Current
  float negative_current = ieee_float_(lolan_get_32bit(8));
  float positive_current = ieee_float_(lolan_get_32bit(12));
  float current = (negative_current > positive_current) ? negative_current * -1.0f : positive_current;
  this->publish_state_(this->current_sensor_, current);

  float power = total_voltage * current;
  this->publish_state_(this->power_sensor_, power);
  this->publish_state_(this->charging_power_sensor_, std::max(0.0f, power));               // 500W vs 0W -> 500W
  this->publish_state_(this->discharging_power_sensor_, std::abs(std::min(0.0f, power)));  // -500W vs 0W -> 500W

  //  16   4  0x41 0xd5 0x9f 0x30    Temperature 1
  this->publish_state_(this->temperatures_[0].temperature_sensor_, ieee_float_(lolan_get_32bit(16)));

  //  20   4  0x41 0xcd 0x0b 0x30    Temperature 2
  this->publish_state_(this->temperatures_[1].temperature_sensor_, ieee_float_(lolan_get_32bit(20)));

  //  24   4  0x42 0x9d 0x93 0x20    Total discharged capacity
  this->publish_state_(this->total_discharged_capacity_sensor_, ieee_float_(lolan_get_32bit(24)));

  //  28   4  0x42 0x47 0x53 0xec    Total charged capacity
  this->publish_state_(this->total_charged_capacity_sensor_, ieee_float_(lolan_get_32bit(28)));

  //  32   4  0x00 0x34 0x3f 0x6e    Total runtime
  this->publish_state_(this->total_runtime_sensor_, lolan_get_32bit(32) * 1.0f);
  this->publish_state_(this->total_runtime_formatted_text_sensor_, format_total_runtime_(lolan_get_32bit(32)));

  //  36   2  0x00 0x01              Charging cycles
  this->publish_state_(this->charging_cycles_sensor_, lolan_get_16bit(36) * 1.0f);

  //  38   1  0x00 0x27              State of charge
  this->publish_state_(this->state_of_charge_sensor_, lolan_get_16bit(38) * 1.0f);
}

void LolanBmsBle::decode_cell_info_data_(const std::vector<uint8_t> &data) {
  auto lolan_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 0]) << 8) | (uint16_t(data[i + 1]) << 0);
  };
  auto lolan_get_32bit = [&](size_t i) -> uint32_t {
    return (uint32_t(lolan_get_16bit(i + 0)) << 16) | (uint32_t(lolan_get_16bit(i + 2)) << 0);
  };

  ESP_LOGI(TAG, "Cell info frame received");
  ESP_LOGD(TAG, "  %s", format_hex_pretty(&data.front(), data.size()).c_str());

  if (data.size() < 40) {
    ESP_LOGW(TAG, "Invalid cell info frame length: %d", data.size());
    return;
  }

  // Byte Len Payload              Description                      Unit  Precision
  //  0    1  0x02                 Frame type
  //  1    1  0x00
  //  2    1  0x07                 Cell count
  ESP_LOGI(TAG, "  Cell count: %d", data[2]);
  uint8_t cell_count = data[2];

  //  3    1  0x00
  //  4    1  0x00
  //  5    1  0x00
  //  6    1  0x00
  //  7    1  0x00
  //  8    4  0x00 0x00 0x00 0x00  Balancing mask
  ESP_LOGI(TAG, "  Balancing mask: %lu", (unsigned long) lolan_get_32bit(8));

  //  12   1  0x00
  //  13   1  0x00
  //  14   1  0x00
  //  15   1  0x00
  //  16   1  0x00
  //  17   1  0x00
  //  18   1  0x00
  //  19   1  0x00
  //  20   4  0x40 0x56 0x66 0x66  Balancer voltage
  this->publish_state_(this->balancer_voltage_sensor_, ieee_float_(lolan_get_32bit(20)));

  //  24   4  0x40 0x53 0x33 0x33  Cell voltage 1
  //  28   4  0x40 0x53 0x3a 0xe1  Cell voltage 2
  //  32   4  0x40 0x53 0x47 0xae  Cell voltage 3
  //  36   4  0x40 0x53 0x3a 0xe1  Cell voltage 4
  //  40   4  0x40 0x53 0x42 0x8f  Cell voltage 5
  //  44   4  0x40 0x53 0x40 0x00  Cell voltage 6
  //  48   4  0x40 0x53 0x3d 0x71  Cell voltage 7
  //  52   4  0x00 0x00 0x00 0x00  Cell voltage 8
  //  56   4  0x00 0x00 0x00 0x00  Cell voltage 9
  //  60   4  0x00 0x00 0x00 0x00  Cell voltage 10
  //  64   4  0x00 0x00 0x00 0x00  Cell voltage 11
  //  68   4  0x00 0x00 0x00 0x00  Cell voltage 12
  //  72   4  0x00 0x00 0x00 0x00  Cell voltage 13
  //  76   4  0x00 0x00 0x00 0x00  Cell voltage 14
  //  80   4  0x00 0x00 0x00 0x00  Cell voltage 15
  //  84   4  0x00 0x00 0x00 0x00  Cell voltage 16
  float min_cell_voltage = 100.0f;
  float max_cell_voltage = -100.0f;
  float average_cell_voltage = 0.0f;
  uint8_t min_voltage_cell = 0;
  uint8_t max_voltage_cell = 0;
  uint8_t cells = std::min(MAX_KNOWN_CELL_COUNT, cell_count);
  for (uint8_t i = 0; i < cells; i++) {
    float cell_voltage = ieee_float_(lolan_get_32bit((i * 4) + 24));
    average_cell_voltage = average_cell_voltage + cell_voltage;
    if (cell_voltage > 0 && cell_voltage < min_cell_voltage) {
      min_cell_voltage = cell_voltage;
      min_voltage_cell = i + 1;
    }
    if (cell_voltage > max_cell_voltage) {
      max_cell_voltage = cell_voltage;
      max_voltage_cell = i + 1;
    }
    this->publish_state_(this->cells_[i].cell_voltage_sensor_, cell_voltage);
  }
  average_cell_voltage = average_cell_voltage / cells;

  this->publish_state_(this->min_cell_voltage_sensor_, min_cell_voltage);
  this->publish_state_(this->max_cell_voltage_sensor_, max_cell_voltage);
  this->publish_state_(this->max_voltage_cell_sensor_, (float) max_voltage_cell);
  this->publish_state_(this->min_voltage_cell_sensor_, (float) min_voltage_cell);
  this->publish_state_(this->delta_cell_voltage_sensor_, max_cell_voltage - min_cell_voltage);
  this->publish_state_(this->average_cell_voltage_sensor_, average_cell_voltage);
}

void LolanBmsBle::decode_settings_data_(const std::vector<uint8_t> &data) {
  auto lolan_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 0]) << 8) | (uint16_t(data[i + 1]) << 0);
  };
  auto lolan_get_32bit = [&](size_t i) -> uint32_t {
    return (uint32_t(lolan_get_16bit(i + 0)) << 16) | (uint32_t(lolan_get_16bit(i + 2)) << 0);
  };

  ESP_LOGI(TAG, "Settings frame received");
  ESP_LOGD(TAG, "  %s", format_hex_pretty(&data.front(), data.size()).c_str());

  if (data.size() < 108) {
    ESP_LOGW(TAG, "Invalid settings frame length: %d", data.size());
    return;
  }

  uint16_t remote_crc = lolan_get_16bit(104);
  uint16_t computed_crc = crc16_lolan(data.data(), 104);
  if (remote_crc != computed_crc) {
    ESP_LOGW(TAG, "Settings frame CRC validation failed! Remote: 0x%04X, Computed: 0x%04X", remote_crc, computed_crc);
    return;
  }

  // Byte Len Payload              Description                      Unit  Precision
  //   0   1  0x03  Frame type                uint8
  //   1   1  0x6c
  ESP_LOGI(TAG, "  Unknown1: %d", data[1]);

  //   2   4  0x00 0xbc 0x61 0x4e  BLE password
  ESP_LOGI(TAG, "  BLE password: %lu", (unsigned long) lolan_get_32bit(2));

  //   6   1  0x0c                 Output voltage offset
  ESP_LOGI(TAG, "  Output voltage offset: %d", data[6]);

  //   7   1  0x00                 Input voltage offset
  ESP_LOGI(TAG, "  Input voltage offset: %d", data[7]);

  //   8   2  0x00 0x32            Max charging current
  ESP_LOGI(TAG, "  Max charging current: %d A", lolan_get_16bit(8));

  //  10   2  0x00 0x50            Max discharging current
  ESP_LOGI(TAG, "  Max discharging current: %d A", lolan_get_16bit(10));

  //  12   1  0x08                 Charging current zero
  ESP_LOGI(TAG, "  Charging current zero: %d", data[12]);

  //  13   1  0x0f                 Discharging current zero
  ESP_LOGI(TAG, "  Discharging current zero: %d", data[13]);

  //  14   1  0x07                 Cell count
  ESP_LOGI(TAG, "  Cell count: %d", data[14]);

  //  15   1  0x01                 Battery type
  ESP_LOGI(TAG, "  Battery type: %d", data[15]);

  //  16   2  0xff 0xfa            mNC2?
  ESP_LOGI(TAG, "  mNC2: %d", data[16]);

  //  18   2  0x00 0x01            Cycle count
  ESP_LOGI(TAG, "  Cycle count: %d", lolan_get_16bit(18));

  //  20   4  0x42 0x51 0x09 0x41  Battery capacity
  ESP_LOGI(TAG, "  Battery capacity: %.3f", ieee_float_(lolan_get_32bit(20)));

  //  24   4  0x40 0x56 0x66 0x66  Start balancing voltage
  ESP_LOGI(TAG, "  Start balancing voltage: %.3f", ieee_float_(lolan_get_32bit(24)));

  //  28   4  0x3f 0x80 0x00 0x00
  ESP_LOGI(TAG, "  Unknown28: %f", ieee_float_(lolan_get_32bit(28)));

  //  32   4  0x40 0xb7 0x95 0x81  Reference discharging voltage
  ESP_LOGI(TAG, "  Reference discharging voltage: %.3f V", ieee_float_(lolan_get_32bit(32)));

  //  36   4  0x40 0xb2 0xb8 0x52  Reference charging voltage
  ESP_LOGI(TAG, "  Reference charging voltage: %.3f V", ieee_float_(lolan_get_32bit(36)));

  //  40   4  0x41 0x9a 0x00 0x00  Pack undervoltage protection
  ESP_LOGI(TAG, "  Pack undervoltage protection: %.3f V", ieee_float_(lolan_get_32bit(40)));

  //  44   4  0x41 0xcc 0x00 0x00  Pack overvoltage protection
  ESP_LOGI(TAG, "  Pack overvoltage protection: %.3f V", ieee_float_(lolan_get_32bit(44)));

  //  48   4  0x40 0x30 0x00 0x00  Cell undervoltage protection
  ESP_LOGI(TAG, "  Cell undervoltage protection: %.3f V", ieee_float_(lolan_get_32bit(48)));

  //  52   4  0x40 0x69 0x99 0x9a  Cell overvoltage protection
  ESP_LOGI(TAG, "  Cell overvoltage protection: %.3f V", ieee_float_(lolan_get_32bit(52)));

  //  56   4  0x42 0x82 0x00 0x00  Mosfet overtemperature protection
  ESP_LOGI(TAG, "  Mosfet overtemperature protection: %.3f °C", ieee_float_(lolan_get_32bit(56)));

  //  60   4  0x42 0x82 0x00 0x00  Circuit temperature protect
  ESP_LOGI(TAG, "  Circuit temperature protection: %.3f °C", ieee_float_(lolan_get_32bit(60)));

  //  64   4  0x46 0x1c 0x40 0x00  Temperature adjustment
  ESP_LOGI(TAG, "  Temperature adjustment: %.3f", ieee_float_(lolan_get_32bit(64)));

  //  68   4  0xcc 0x10 0xf1 0x52  Serial number (uint32)
  ESP_LOGI(TAG, "  Serial number: %lu", (unsigned long) lolan_get_32bit(68));

  //  72   4  0x01 0x34 0x8b 0x9a  Software date (uint32)
  ESP_LOGI(TAG, "  Software date: %lu", (unsigned long) lolan_get_32bit(72));

  //  76   4  0x40 0x30 0x00 0x00  Lowpower
  ESP_LOGI(TAG, "  Lowpower: %.3f V", ieee_float_(lolan_get_32bit(76)));

  //  80   4  0x40 0x53 0x33 0x33  Battery4 Adjustment
  ESP_LOGI(TAG, "  Battery4 adjustment: %.3f", ieee_float_(lolan_get_32bit(80)));

  //  84   4  0x3c 0x23 0xd7 0x0a  Voltage Equal
  ESP_LOGI(TAG, "  Voltage Equal: %.3f V", ieee_float_(lolan_get_32bit(84)));

  //  88   4  0x41 0xc0 0xcc 0xcd  Overcharge release voltage
  ESP_LOGI(TAG, "  Overcharge release voltage: %.3f V", ieee_float_(lolan_get_32bit(88)));

  //  92   4  0x40 0x5c 0xcc 0xcd  Overcharge cell voltage
  ESP_LOGI(TAG, "  Overcharge cell voltage: %.3f V", ieee_float_(lolan_get_32bit(92)));

  //  96   4  0x41 0xa2 0x66 0x66  Discharge release voltage
  ESP_LOGI(TAG, "  Discharge release voltage: %.3f V", ieee_float_(lolan_get_32bit(96)));

  // 100   4  0x40 0x39 0x99 0x9a  Discharge cell voltage
  ESP_LOGI(TAG, "  Discharge cell voltage: %.3f V", ieee_float_(lolan_get_32bit(100)));

  // 104   2  0x35 0xe2            CRC?
  // 106   2  0x5a 0xa5
}

void LolanBmsBle::decode_confirmations_(const std::vector<uint8_t> &data) {
  auto lolan_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 0]) << 8) | (uint16_t(data[i + 1]) << 0);
  };
  auto lolan_get_32bit = [&](size_t i) -> uint32_t {
    return (uint32_t(lolan_get_16bit(i + 0)) << 16) | (uint32_t(lolan_get_16bit(i + 2)) << 0);
  };

  if (data.size() < 4) {
    ESP_LOGW(TAG, "Invalid confirmation frame length: %d", data.size());
    return;
  }

  switch (lolan_get_32bit(0)) {
    case 0x211d1ae3:
      ESP_LOGE(TAG, "Invalid password");
      break;
    case 0x21b73331:
      ESP_LOGI(TAG, "Settings updated successfully");
      break;
    case 0x2198aec5:
      ESP_LOGI(TAG, "Bluetooth name updated successfully");
      break;
    case 0x2f9983ca:
      ESP_LOGI(TAG, "Setting successful");
      break;
    case 0x358f0322:
      ESP_LOGW(TAG, "Data error");
      break;
    case 0x35957f4a:
      ESP_LOGI(TAG, "Password changed successfully");
      break;
    case 0x358b7635:
      ESP_LOGI(TAG, "Current calibration successful");
      break;
    default:
      ESP_LOGW(TAG, "Unhandled confirmation received: %s", format_hex_pretty(&data.front(), data.size()).c_str());
  }
}

void LolanBmsBle::dump_config() {  // NOLINT(google-readability-function-size,readability-function-size)
  ESP_LOGCONFIG(TAG, "LolanBmsBle:");

  LOG_BINARY_SENSOR("", "Charging", this->charging_binary_sensor_);
  LOG_BINARY_SENSOR("", "Discharging", this->discharging_binary_sensor_);

  LOG_SENSOR("", "Total voltage", this->total_voltage_sensor_);
  LOG_SENSOR("", "Current", this->current_sensor_);
  LOG_SENSOR("", "Power", this->power_sensor_);
  LOG_SENSOR("", "Charging power", this->charging_power_sensor_);
  LOG_SENSOR("", "Discharging power", this->discharging_power_sensor_);
  LOG_SENSOR("", "Error bitmask", this->error_bitmask_sensor_);
  LOG_SENSOR("", "State of charge", this->state_of_charge_sensor_);
  LOG_SENSOR("", "Charging cycles", this->charging_cycles_sensor_);
  LOG_SENSOR("", "Min cell voltage", this->min_cell_voltage_sensor_);
  LOG_SENSOR("", "Max cell voltage", this->max_cell_voltage_sensor_);
  LOG_SENSOR("", "Min voltage cell", this->min_voltage_cell_sensor_);
  LOG_SENSOR("", "Max voltage cell", this->max_voltage_cell_sensor_);
  LOG_SENSOR("", "Delta cell voltage", this->delta_cell_voltage_sensor_);
  LOG_SENSOR("", "Temperature 1", this->temperatures_[0].temperature_sensor_);
  LOG_SENSOR("", "Temperature 2", this->temperatures_[1].temperature_sensor_);
  LOG_SENSOR("", "Cell Voltage 1", this->cells_[0].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 2", this->cells_[1].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 3", this->cells_[2].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 4", this->cells_[3].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 5", this->cells_[4].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 6", this->cells_[5].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 7", this->cells_[6].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 8", this->cells_[7].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 9", this->cells_[8].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 10", this->cells_[9].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 11", this->cells_[10].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 12", this->cells_[11].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 13", this->cells_[12].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 14", this->cells_[13].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 15", this->cells_[14].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 16", this->cells_[15].cell_voltage_sensor_);
  LOG_SENSOR("", "Balancer voltage", this->balancer_voltage_sensor_);
  LOG_SENSOR("", "Total charged capacity", this->total_charged_capacity_sensor_);
  LOG_SENSOR("", "Total discharged capacity", this->total_discharged_capacity_sensor_);
  LOG_SENSOR("", "Total runtime", this->total_runtime_sensor_);
  LOG_SENSOR("", "Average cell voltage", this->average_cell_voltage_sensor_);

  LOG_TEXT_SENSOR("", "Errors", this->errors_text_sensor_);
  LOG_TEXT_SENSOR("", "Total runtime formatted", this->total_runtime_formatted_text_sensor_);
}

void LolanBmsBle::publish_state_(binary_sensor::BinarySensor *binary_sensor, const bool &state) {
  if (binary_sensor == nullptr)
    return;

  binary_sensor->publish_state(state);
}

void LolanBmsBle::publish_state_(sensor::Sensor *sensor, float value) {
  if (sensor == nullptr)
    return;

  sensor->publish_state(value);
}

void LolanBmsBle::publish_state_(switch_::Switch *obj, const bool &state) {
  if (obj == nullptr)
    return;

  obj->publish_state(state);
}

void LolanBmsBle::publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state) {
  if (text_sensor == nullptr)
    return;

  text_sensor->publish_state(state);
}

bool LolanBmsBle::send_command(uint16_t function) {
  uint8_t frame[6];

  frame[0] = function >> 8;  // function
  frame[1] = function >> 0;  // function
  frame[2] = this->password_ >> 24;
  frame[3] = this->password_ >> 16;
  frame[4] = this->password_ >> 8;
  frame[5] = this->password_ >> 0;

  ESP_LOGD(TAG, "Send command (handle 0x%02X): %s", this->char_command_handle_,
           format_hex_pretty(frame, sizeof(frame)).c_str());

  auto status =
      esp_ble_gattc_write_char(this->parent_->get_gattc_if(), this->parent_->get_conn_id(), this->char_command_handle_,
                               sizeof(frame), frame, ESP_GATT_WRITE_TYPE_NO_RSP, ESP_GATT_AUTH_REQ_NONE);

  if (status) {
    ESP_LOGW(TAG, "[%s] esp_ble_gattc_write_char failed, status=%d", this->parent_->address_str().c_str(), status);
  }

  return (status == 0);
}

bool LolanBmsBle::send_factory_reset() {
  uint8_t frame[8];

  frame[0] = 0xCC;
  frame[1] = 0xCC;
  frame[2] = 0x00;
  frame[3] = 0x00;
  frame[4] = 0x00;
  frame[5] = 0x00;
  frame[6] = 0x00;
  frame[7] = 0x00;

  ESP_LOGD(TAG, "Send factory reset (handle 0x%02X): %s", this->char_command_handle_,
           format_hex_pretty(frame, sizeof(frame)).c_str());

  auto status =
      esp_ble_gattc_write_char(this->parent_->get_gattc_if(), this->parent_->get_conn_id(), this->char_command_handle_,
                               sizeof(frame), frame, ESP_GATT_WRITE_TYPE_NO_RSP, ESP_GATT_AUTH_REQ_NONE);

  if (status) {
    ESP_LOGW(TAG, "[%s] esp_ble_gattc_write_char failed, status=%d", this->parent_->address_str().c_str(), status);
  }

  return (status == 0);
}

std::string LolanBmsBle::bitmask_to_string_(const char *const messages[], const uint8_t &messages_size,
                                            const uint8_t &mask) {
  std::string values = "";
  if (mask) {
    for (int i = 0; i < messages_size; i++) {
      if (mask & (1 << i)) {
        values.append(messages[i]);
        values.append(";");
      }
    }
    if (!values.empty()) {
      values.pop_back();
    }
  }
  return values;
}

}  // namespace lolan_bms_ble
}  // namespace esphome
