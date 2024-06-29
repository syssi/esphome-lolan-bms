#pragma once

#include "../lolan_bms_ble.h"
#include "esphome/core/component.h"
#include "esphome/components/button/button.h"

namespace esphome {
namespace lolan_bms_ble {

class LolanBmsBle;
class LolanButton : public button::Button, public Component {
 public:
  void set_parent(LolanBmsBle *parent) { this->parent_ = parent; };
  void set_holding_register(uint16_t holding_register) { this->holding_register_ = holding_register; };
  void dump_config() override;
  void loop() override {}
  float get_setup_priority() const override { return setup_priority::DATA; }

 protected:
  void press_action() override;
  LolanBmsBle *parent_;
  uint16_t holding_register_;
};

}  // namespace lolan_bms_ble
}  // namespace esphome
