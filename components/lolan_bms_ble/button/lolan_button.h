#pragma once

#include "../lolan_bms_ble.h"
#include "esphome/components/button/button.h"

namespace esphome::lolan_bms_ble {

class LolanBmsBle;
class LolanButton : public button::Button {
 public:
  void set_parent(LolanBmsBle *parent) { this->parent_ = parent; };
  void set_holding_register(uint16_t holding_register) { this->holding_register_ = holding_register; };

 protected:
  void press_action() override;
  LolanBmsBle *parent_;
  uint16_t holding_register_;
};

}  // namespace esphome::lolan_bms_ble
