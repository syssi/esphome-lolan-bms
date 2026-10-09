#include "lolan_button.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

// Fallback for ESPHome < 2026.10.0
#ifndef ESPHOME_LOG_TAG
#define ESPHOME_LOG_TAG(name, tag) static const char *const name = tag
#endif

namespace esphome::lolan_bms_ble {

ESPHOME_LOG_TAG(TAG, "lolan_bms_ble.button");

static const uint16_t LOLAN_COMMAND_FACTORY_RESET = 0xCCCC;

void LolanButton::dump_config() { LOG_BUTTON("", "LolanBmsBle Button", this); }
void LolanButton::press_action() {
  if (this->holding_register_ == LOLAN_COMMAND_FACTORY_RESET) {
    this->parent_->send_factory_reset();
    return;
  }

  this->parent_->send_command(this->holding_register_);
}

}  // namespace esphome::lolan_bms_ble
