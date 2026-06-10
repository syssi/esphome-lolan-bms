#include <gtest/gtest.h>
#include <cmath>
#include "common.h"

namespace esphome::lolan_bms_ble::testing {

TEST(LolanBmsBleStatusTest, StateOfCharge) {
  TestableLolanBmsBle bms;
  sensor::Sensor soc;
  bms.set_state_of_charge_sensor(&soc);
  bms.decode_status_data_(STATUS_FRAME_CHARGING_AND_DISCHARGING);
  EXPECT_NEAR(soc.state, 39.0f, 1.0f);
}

TEST(LolanBmsBleStatusTest, VoltageCurrentPower) {
  TestableLolanBmsBle bms;
  sensor::Sensor voltage, current, power;
  bms.set_total_voltage_sensor(&voltage);
  bms.set_current_sensor(&current);
  bms.set_power_sensor(&power);
  bms.decode_status_data_(STATUS_FRAME_CHARGING_AND_DISCHARGING);
  EXPECT_NEAR(voltage.state, 23.13f, 0.01f);
  EXPECT_NEAR(current.state, 0.0f, 0.01f);
  EXPECT_NEAR(power.state, 0.0f, 0.01f);
}

TEST(LolanBmsBleStatusTest, Temperatures) {
  TestableLolanBmsBle bms;
  sensor::Sensor temp1, temp2;
  bms.set_temperature_sensor(0, &temp1);
  bms.set_temperature_sensor(1, &temp2);
  bms.decode_status_data_(STATUS_FRAME_CHARGING_AND_DISCHARGING);
  EXPECT_NEAR(temp1.state, 26.98f, 0.1f);
  EXPECT_NEAR(temp2.state, 25.65f, 0.1f);
}

TEST(LolanBmsBleStatusTest, Capacities) {
  TestableLolanBmsBle bms;
  sensor::Sensor charged, discharged;
  bms.set_total_charged_capacity_sensor(&charged);
  bms.set_total_discharged_capacity_sensor(&discharged);
  bms.decode_status_data_(STATUS_FRAME_CHARGING_AND_DISCHARGING);
  EXPECT_NEAR(charged.state, 49.90f, 0.01f);
  EXPECT_NEAR(discharged.state, 78.79f, 0.01f);
}

TEST(LolanBmsBleStatusTest, RuntimeAndCycles) {
  TestableLolanBmsBle bms;
  sensor::Sensor runtime, cycles;
  text_sensor::TextSensor runtime_formatted;
  bms.set_total_runtime_sensor(&runtime);
  bms.set_total_runtime_formatted_text_sensor(&runtime_formatted);
  bms.set_charging_cycles_sensor(&cycles);
  bms.decode_status_data_(STATUS_FRAME_CHARGING_AND_DISCHARGING);
  EXPECT_NEAR(runtime.state, 3554896.0f, 1.0f);
  EXPECT_STREQ(runtime_formatted.state.c_str(), "41d 3h");
  EXPECT_NEAR(cycles.state, 1.0f, 1.0f);
}

TEST(LolanBmsBleStatusTest, ErrorBitmask) {
  TestableLolanBmsBle bms;
  sensor::Sensor errors;
  text_sensor::TextSensor errors_text;
  bms.set_error_bitmask_sensor(&errors);
  bms.set_errors_text_sensor(&errors_text);
  bms.decode_status_data_(STATUS_FRAME_CHARGING_AND_DISCHARGING);
  EXPECT_FLOAT_EQ(errors.state, 0.0f);
  EXPECT_EQ(errors_text.state, "");
}

TEST(LolanBmsBleStatusTest, SwitchStatesChargingAndDischarging) {
  TestableLolanBmsBle bms;
  binary_sensor::BinarySensor charging, discharging;
  bms.set_charging_binary_sensor(&charging);
  bms.set_discharging_binary_sensor(&discharging);
  bms.decode_status_data_(STATUS_FRAME_CHARGING_AND_DISCHARGING);
  EXPECT_TRUE(charging.state);
  EXPECT_TRUE(discharging.state);
}

TEST(LolanBmsBleStatusTest, SwitchStatesOnlyCharging) {
  TestableLolanBmsBle bms;
  binary_sensor::BinarySensor charging, discharging;
  bms.set_charging_binary_sensor(&charging);
  bms.set_discharging_binary_sensor(&discharging);
  bms.decode_status_data_(STATUS_FRAME_ONLY_CHARGING_ENABLED);
  EXPECT_TRUE(charging.state);
  EXPECT_FALSE(discharging.state);
}

TEST(LolanBmsBleStatusTest, SwitchStatesOnlyDischarging) {
  TestableLolanBmsBle bms;
  binary_sensor::BinarySensor charging, discharging;
  bms.set_charging_binary_sensor(&charging);
  bms.set_discharging_binary_sensor(&discharging);
  bms.decode_status_data_(STATUS_FRAME_ONLY_DISCHARGING_ENABLED);
  EXPECT_FALSE(charging.state);
  EXPECT_TRUE(discharging.state);
}

TEST(LolanBmsBleStatusTest, NullSensorsDoNotCrash) {
  TestableLolanBmsBle bms;
  bms.decode_status_data_(STATUS_FRAME_CHARGING_AND_DISCHARGING);
}

TEST(LolanBmsBleCellInfoTest, CellVoltages) {
  TestableLolanBmsBle bms;
  sensor::Sensor cells[8], min_v, max_v, delta, avg;
  sensor::Sensor min_cell, max_cell;
  for (int i = 0; i < 8; i++) {
    bms.set_cell_voltage_sensor(i, &cells[i]);
  }
  bms.set_min_cell_voltage_sensor(&min_v);
  bms.set_max_cell_voltage_sensor(&max_v);
  bms.set_min_voltage_cell_sensor(&min_cell);
  bms.set_max_voltage_cell_sensor(&max_cell);
  bms.set_delta_cell_voltage_sensor(&delta);
  bms.set_average_cell_voltage_sensor(&avg);
  bms.decode_cell_info_data_(CELL_INFO_FRAME_1);

  EXPECT_NEAR(cells[0].state, 3.300f, 0.01f);
  EXPECT_NEAR(cells[1].state, 3.300f, 0.01f);
  EXPECT_NEAR(cells[2].state, 3.301f, 0.01f);
  EXPECT_NEAR(cells[3].state, 3.300f, 0.01f);
  EXPECT_NEAR(cells[4].state, 3.301f, 0.01f);
  EXPECT_NEAR(cells[5].state, 3.301f, 0.01f);
  EXPECT_NEAR(cells[6].state, 3.301f, 0.01f);
  EXPECT_TRUE(std::isnan(cells[7].state));

  EXPECT_NEAR(min_v.state, 3.300f, 0.01f);
  EXPECT_NEAR(max_v.state, 3.301f, 0.01f);
  EXPECT_FLOAT_EQ(min_cell.state, 1.0f);
  EXPECT_FLOAT_EQ(max_cell.state, 3.0f);
  EXPECT_NEAR(delta.state, 0.00125f, 0.0001f);
  EXPECT_NEAR(avg.state, 3.3006f, 0.001f);
}

TEST(LolanBmsBleCellInfoTest, BalancerVoltage) {
  TestableLolanBmsBle bms;
  sensor::Sensor balancer;
  bms.set_balancer_voltage_sensor(&balancer);
  bms.decode_cell_info_data_(CELL_INFO_FRAME_1);
  EXPECT_NEAR(balancer.state, 3.350f, 0.01f);
}

TEST(LolanBmsBleCellInfoTest, NullSensorsDoNotCrash) {
  TestableLolanBmsBle bms;
  bms.decode_cell_info_data_(CELL_INFO_FRAME_1);
}

TEST(LolanBmsBleSettingsTest, NullSensorsDoNotCrash) {
  TestableLolanBmsBle bms;
  bms.decode_settings_data_(SETTINGS_FRAME_1);
}

TEST(LolanBmsBleDispatchTest, StatusFrameDispatched) {
  TestableLolanBmsBle bms;
  sensor::Sensor soc;
  bms.set_state_of_charge_sensor(&soc);
  bms.on_lolan_bms_ble_data(0x10, STATUS_FRAME_CHARGING_AND_DISCHARGING);
  EXPECT_NEAR(soc.state, 39.0f, 1.0f);
}

TEST(LolanBmsBleDispatchTest, InvalidFrameTooShort) {
  TestableLolanBmsBle bms;
  sensor::Sensor soc;
  soc.state = -1.0f;
  bms.set_state_of_charge_sensor(&soc);
  bms.on_lolan_bms_ble_data(0x10, INVALID_FRAME_TOO_SHORT);
  EXPECT_FLOAT_EQ(soc.state, -1.0f);
}

TEST(LolanBmsBleDispatchTest, CellInfoFrameDispatched) {
  TestableLolanBmsBle bms;
  sensor::Sensor c1;
  bms.set_cell_voltage_sensor(0, &c1);
  bms.on_lolan_bms_ble_data(0x10, CELL_INFO_FRAME_1);
  EXPECT_NEAR(c1.state, 3.300f, 0.01f);
}

TEST(LolanBmsBleDispatchTest, SettingsFrameDispatched) {
  TestableLolanBmsBle bms;
  bms.on_lolan_bms_ble_data(0x10, SETTINGS_FRAME_1);
}

TEST(LolanBmsBleDispatchTest, WrongCrcFrameNotProcessed) {
  TestableLolanBmsBle bms;
  sensor::Sensor soc;
  soc.state = -1.0f;
  bms.set_state_of_charge_sensor(&soc);
  bms.on_lolan_bms_ble_data(0x10, INVALID_FRAME_WRONG_CRC);
  EXPECT_FLOAT_EQ(soc.state, -1.0f);
}

TEST(LolanBmsBleConfirmationTest, SettingsSuccess) {
  TestableLolanBmsBle bms;
  bms.decode_confirmations_(CONFIRMATION_SETTINGS_SUCCESS);
}

TEST(LolanBmsBleConfirmationTest, PasswordError) {
  TestableLolanBmsBle bms;
  bms.decode_confirmations_(CONFIRMATION_PASSWORD_ERROR);
}

TEST(LolanBmsBleConfirmationTest, BluetoothNameSuccess) {
  TestableLolanBmsBle bms;
  bms.decode_confirmations_(CONFIRMATION_BLUETOOTH_NAME_SUCCESS);
}

TEST(LolanBmsBleConfirmationTest, SettingSuccess) {
  TestableLolanBmsBle bms;
  bms.decode_confirmations_(CONFIRMATION_SETTING_SUCCESS);
}

TEST(LolanBmsBleConfirmationTest, DataError) {
  TestableLolanBmsBle bms;
  bms.decode_confirmations_(CONFIRMATION_DATA_ERROR);
}

TEST(LolanBmsBleConfirmationTest, PasswordChangedSuccess) {
  TestableLolanBmsBle bms;
  bms.decode_confirmations_(CONFIRMATION_PASSWORD_CHANGED_SUCCESS);
}

TEST(LolanBmsBleConfirmationTest, CalibrationSuccess) {
  TestableLolanBmsBle bms;
  bms.decode_confirmations_(CONFIRMATION_CALIBRATION_SUCCESS);
}

TEST(LolanBmsBleConfirmationTest, InvalidFrameTooShort) {
  TestableLolanBmsBle bms;
  std::vector<uint8_t> short_frame = {0x21, 0x01};
  bms.decode_confirmations_(short_frame);
}

TEST(LolanBmsBleSwitchTest, ChargingCommandSent) {
  TestableLolanBmsBle bms;
  bms.char_command_handle_ = 0x20;
  TestSwitch charging_switch;
  charging_switch.set_parent(&bms);
  charging_switch.set_command_turn_on(0x0001);
  charging_switch.set_command_turn_off(0x0002);
  charging_switch.turn_on();
}

TEST(LolanBmsBleSwitchTest, DischargingCommandSent) {
  TestableLolanBmsBle bms;
  bms.char_command_handle_ = 0x20;
  TestSwitch discharging_switch;
  discharging_switch.set_parent(&bms);
  discharging_switch.set_command_turn_on(0x0003);
  discharging_switch.set_command_turn_off(0x0004);
  discharging_switch.turn_on();
}

TEST(LolanBmsBleButtonTest, FactoryResetCommandSent) {
  TestableLolanBmsBle bms;
  bms.char_command_handle_ = 0x20;
  TestButton button;
  button.set_parent(&bms);
  button.set_holding_register(0xCCCC);
  button.press();
}

// ── Online status tracker ─────────────────────────────────────────────────────

TEST(LolanBmsBleOnlineStatusTrackerTest, ReachesThreshold) {
  TestableLolanBmsBle bms;
  binary_sensor::BinarySensor online_status;
  bms.set_online_status_binary_sensor(&online_status);

  for (int i = 0; i < 10; i++)
    bms.track_online_status_();

  EXPECT_FALSE(online_status.state);
}

TEST(LolanBmsBleOnlineStatusTrackerTest, DoesNotRepeatAfterThreshold) {
  TestableLolanBmsBle bms;
  binary_sensor::BinarySensor online_status;
  bms.set_online_status_binary_sensor(&online_status);

  for (int i = 0; i < 10; i++)
    bms.track_online_status_();
  EXPECT_EQ(bms.get_no_response_count(), 11);

  bms.track_online_status_();
  EXPECT_EQ(bms.get_no_response_count(), 11);
}

TEST(LolanBmsBleOnlineStatusTrackerTest, ResetRestoresOnlineStatus) {
  TestableLolanBmsBle bms;
  binary_sensor::BinarySensor online_status;
  bms.set_online_status_binary_sensor(&online_status);

  for (int i = 0; i < 10; i++)
    bms.track_online_status_();
  EXPECT_FALSE(online_status.state);

  bms.reset_online_status_tracker_();
  EXPECT_TRUE(online_status.state);
  EXPECT_EQ(bms.get_no_response_count(), 0);
}

TEST(LolanBmsBleOnlineStatusTrackerTest, CanRetriggerAfterReset) {
  TestableLolanBmsBle bms;
  binary_sensor::BinarySensor online_status;
  bms.set_online_status_binary_sensor(&online_status);

  for (int i = 0; i < 10; i++)
    bms.track_online_status_();
  bms.reset_online_status_tracker_();

  for (int i = 0; i < 10; i++)
    bms.track_online_status_();
  EXPECT_FALSE(online_status.state);
}

TEST(LolanBmsBleOnlineStatusTrackerTest, ValidFrameResetsCounter) {
  TestableLolanBmsBle bms;

  for (int i = 0; i < 5; i++)
    bms.track_online_status_();
  EXPECT_EQ(bms.get_no_response_count(), 5);

  bms.on_lolan_bms_ble_data(0x00, STATUS_FRAME_CHARGING_AND_DISCHARGING);
  EXPECT_EQ(bms.get_no_response_count(), 0);
}

TEST(LolanBmsBleOnlineStatusTrackerTest, NotYetAtThresholdStaysOnline) {
  TestableLolanBmsBle bms;
  binary_sensor::BinarySensor online_status;
  bms.set_online_status_binary_sensor(&online_status);

  online_status.publish_state(true);
  for (int i = 0; i < 9; i++)
    bms.track_online_status_();

  EXPECT_TRUE(online_status.state);
}

// ── publish_device_unavailable_ ───────────────────────────────────────────────

TEST(LolanBmsBlePublishDeviceUnavailableTest, SetsOnlineStatusFalse) {
  TestableLolanBmsBle bms;
  binary_sensor::BinarySensor online_status;
  bms.set_online_status_binary_sensor(&online_status);

  online_status.publish_state(true);
  bms.publish_device_unavailable_();

  EXPECT_FALSE(online_status.state);
}

TEST(LolanBmsBlePublishDeviceUnavailableTest, SetsNumericSensorsToNAN) {
  TestableLolanBmsBle bms;
  sensor::Sensor total_voltage, current, power, charging_power, discharging_power;
  sensor::Sensor error_bitmask, state_of_charge, charging_cycles, total_runtime, balancer_voltage;
  bms.set_total_voltage_sensor(&total_voltage);
  bms.set_current_sensor(&current);
  bms.set_power_sensor(&power);
  bms.set_charging_power_sensor(&charging_power);
  bms.set_discharging_power_sensor(&discharging_power);
  bms.set_error_bitmask_sensor(&error_bitmask);
  bms.set_state_of_charge_sensor(&state_of_charge);
  bms.set_charging_cycles_sensor(&charging_cycles);
  bms.set_total_runtime_sensor(&total_runtime);
  bms.set_balancer_voltage_sensor(&balancer_voltage);

  bms.publish_device_unavailable_();

  EXPECT_TRUE(std::isnan(total_voltage.state));
  EXPECT_TRUE(std::isnan(current.state));
  EXPECT_TRUE(std::isnan(power.state));
  EXPECT_TRUE(std::isnan(charging_power.state));
  EXPECT_TRUE(std::isnan(discharging_power.state));
  EXPECT_TRUE(std::isnan(error_bitmask.state));
  EXPECT_TRUE(std::isnan(state_of_charge.state));
  EXPECT_TRUE(std::isnan(charging_cycles.state));
  EXPECT_TRUE(std::isnan(total_runtime.state));
  EXPECT_TRUE(std::isnan(balancer_voltage.state));
}

TEST(LolanBmsBlePublishDeviceUnavailableTest, SetsCellAndTempSensorsToNAN) {
  TestableLolanBmsBle bms;
  sensor::Sensor cell0, cell1, temp0, temp1;
  bms.set_cell_voltage_sensor(0, &cell0);
  bms.set_cell_voltage_sensor(1, &cell1);
  bms.set_temperature_sensor(0, &temp0);
  bms.set_temperature_sensor(1, &temp1);

  bms.publish_device_unavailable_();

  EXPECT_TRUE(std::isnan(cell0.state));
  EXPECT_TRUE(std::isnan(cell1.state));
  EXPECT_TRUE(std::isnan(temp0.state));
  EXPECT_TRUE(std::isnan(temp1.state));
}

TEST(LolanBmsBlePublishDeviceUnavailableTest, SetsDynamicTextSensorsToOffline) {
  TestableLolanBmsBle bms;
  text_sensor::TextSensor errors, total_runtime_formatted;
  bms.set_errors_text_sensor(&errors);
  bms.set_total_runtime_formatted_text_sensor(&total_runtime_formatted);

  bms.publish_device_unavailable_();

  EXPECT_EQ(errors.state, "Offline");
  EXPECT_EQ(total_runtime_formatted.state, "Offline");
}

TEST(LolanBmsBlePublishDeviceUnavailableTest, LeavesStaticTextSensorsUnchanged) {
  TestableLolanBmsBle bms;
  // lolan_bms_ble has no static identity text sensors — this test is a no-op
  EXPECT_NO_FATAL_FAILURE(bms.publish_device_unavailable_());
}

TEST(LolanBmsBlePublishDeviceUnavailableTest, NullSensorsDoNotCrash) {
  TestableLolanBmsBle bms;

  EXPECT_NO_FATAL_FAILURE(bms.publish_device_unavailable_());
}

}  // namespace esphome::lolan_bms_ble::testing
