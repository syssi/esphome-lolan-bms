import esphome.codegen as cg
from esphome.components import button
import esphome.config_validation as cv
from esphome.const import CONF_FACTORY_RESET, CONF_ICON, CONF_ID

from .. import CONF_LOLAN_BMS_BLE_ID, LOLAN_BMS_BLE_COMPONENT_SCHEMA, lolan_bms_ble_ns

DEPENDENCIES = ["lolan_bms_ble"]

CODEOWNERS = ["@syssi"]

CONF_RETRIEVE_SETTINGS = "retrieve_settings"
# CONF_FACTORY_RESET = "factory_reset"
CONF_RESET_CHARGING_CYCLES = "reset_charging_cycles"
CONF_RESET_TOTAL_CHARGING_ENERGY = "reset_total_charging_energy"
CONF_RESET_TOTAL_DISCHARGING_ENERGY = "reset_total_discharging_energy"

ICON_RETRIEVE_SETTINGS = "mdi:cog"
ICON_FACTORY_RESET = "mdi:factory"
ICON_RESET_CHARGING_CYCLES = "mdi:counter"
ICON_RESET_TOTAL_CHARGING_ENERGY = "mdi:counter"
ICON_RESET_TOTAL_DISCHARGING_ENERGY = "mdi:counter"

BUTTONS = {
    CONF_RETRIEVE_SETTINGS: 0x5600,
    CONF_FACTORY_RESET: 0xCCCC,
    CONF_RESET_CHARGING_CYCLES: 0xAA55,
    CONF_RESET_TOTAL_CHARGING_ENERGY: 0xCB00,
    CONF_RESET_TOTAL_DISCHARGING_ENERGY: 0xCA00,
}

LolanButton = lolan_bms_ble_ns.class_("LolanButton", button.Button, cg.Component)

CONFIG_SCHEMA = LOLAN_BMS_BLE_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_RETRIEVE_SETTINGS): button.BUTTON_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(LolanButton),
                cv.Optional(CONF_ICON, default=ICON_RETRIEVE_SETTINGS): cv.icon,
            }
        ).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_FACTORY_RESET): button.BUTTON_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(LolanButton),
                cv.Optional(CONF_ICON, default=ICON_FACTORY_RESET): cv.icon,
            }
        ).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_RESET_CHARGING_CYCLES): button.BUTTON_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(LolanButton),
                cv.Optional(CONF_ICON, default=ICON_RESET_CHARGING_CYCLES): cv.icon,
            }
        ).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_RESET_TOTAL_CHARGING_ENERGY): button.BUTTON_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(LolanButton),
                cv.Optional(
                    CONF_ICON, default=ICON_RESET_TOTAL_CHARGING_ENERGY
                ): cv.icon,
            }
        ).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_RESET_TOTAL_DISCHARGING_ENERGY): button.BUTTON_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(LolanButton),
                cv.Optional(
                    CONF_ICON, default=ICON_RESET_TOTAL_DISCHARGING_ENERGY
                ): cv.icon,
            }
        ).extend(cv.COMPONENT_SCHEMA),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_LOLAN_BMS_BLE_ID])
    for key, address in BUTTONS.items():
        if key in config:
            conf = config[key]
            var = cg.new_Pvariable(conf[CONF_ID])
            await cg.register_component(var, conf)
            await button.register_button(var, conf)
            cg.add(var.set_parent(hub))
            cg.add(var.set_holding_register(address))
