#include "lolan_button.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

namespace esphome {
namespace lolan_bms_ble {

static const char *const TAG = "lolan_bms_ble.button";

static const uint16_t LOLAN_COMMAND_FACTORY_RESET = 0xCCCC;

void LolanButton::dump_config() { LOG_BUTTON("", "LolanBmsBle Button", this); }
void LolanButton::press_action() {
  if (this->holding_register_ == LOLAN_COMMAND_FACTORY_RESET) {
    this->parent_->send_factory_reset();
    return;
  }

  this->parent_->send_command(this->holding_register_);
}

}  // namespace lolan_bms_ble
}  // namespace esphome
