import esphome.codegen as cg
from esphome.components import switch
import esphome.config_validation as cv

from .. import CONF_LOLAN_BMS_BLE_ID, LOLAN_BMS_BLE_COMPONENT_SCHEMA
from ..const import CONF_CHARGING, CONF_DISCHARGING

DEPENDENCIES = ["lolan_bms_ble"]

CODEOWNERS = ["@syssi"]


ICON_CHARGING = "mdi:battery-charging-50"
ICON_DISCHARGING = "mdi:battery-charging-50"

# key: (command turn on, command turn off)
SWITCHES = {
    CONF_CHARGING: (0xD888, 0xD900),
    CONF_DISCHARGING: (0x8555, 0x8600),
}

CONFIG_SCHEMA = LOLAN_BMS_BLE_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_CHARGING): switch.switch_schema(
            switch.ApplySwitch, icon=ICON_CHARGING
        ),
        cv.Optional(CONF_DISCHARGING): switch.switch_schema(
            switch.ApplySwitch, icon=ICON_DISCHARGING
        ),
    }
)


def _send_command(key: str) -> str:
    turn_on, turn_off = SWITCHES[key]
    return f"send_command(state ? 0x{turn_on:04X} : 0x{turn_off:04X})"


async def to_code(config):
    hub = await cg.get_variable(config[CONF_LOLAN_BMS_BLE_ID])
    switches = switch.sub_switches(config, parent=hub)
    await switches(
        CONF_CHARGING,
        hub.set_charging_switch,
        apply=_send_command(CONF_CHARGING),
        publish="on_success",
    )
    await switches(
        CONF_DISCHARGING,
        hub.set_discharging_switch,
        apply=_send_command(CONF_DISCHARGING),
        publish="on_success",
    )
