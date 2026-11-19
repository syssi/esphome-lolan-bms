#include "lolan_switch.h"

namespace esphome::lolan_bms_ble {

void LolanSwitch::write_state(bool state) {
  if (this->parent_->send_command(state ? this->command_turn_on_ : this->command_turn_off_)) {
    this->publish_state(state);
  }
}

}  // namespace esphome::lolan_bms_ble
