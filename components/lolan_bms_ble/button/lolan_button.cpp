#include "lolan_button.h"

namespace esphome::lolan_bms_ble {

static const uint16_t LOLAN_COMMAND_FACTORY_RESET = 0xCCCC;

void LolanButton::press_action() {
  if (this->holding_register_ == LOLAN_COMMAND_FACTORY_RESET) {
    this->parent_->send_factory_reset();
    return;
  }

  this->parent_->send_command(this->holding_register_);
}

}  // namespace esphome::lolan_bms_ble
