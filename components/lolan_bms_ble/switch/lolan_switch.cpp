#include "lolan_switch.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

namespace esphome {
namespace lolan_bms_ble {

static const char *const TAG = "lolan_bms_ble.switch";

void LolanSwitch::dump_config() { LOG_SWITCH("", "LolanBmsBle Switch", this); }
void LolanSwitch::write_state(bool state) {
  if (this->parent_->send_command(state ? this->command_turn_on_ : this->command_turn_off_)) {
    this->publish_state(state);
  }
}

}  // namespace lolan_bms_ble
}  // namespace esphome
