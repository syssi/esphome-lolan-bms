import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv
from esphome.const import ENTITY_CATEGORY_DIAGNOSTIC, ICON_TIMELAPSE

from . import LOLAN_BMS_BLE_COMPONENT_SCHEMA

DEPENDENCIES = ["lolan_bms_ble"]

CODEOWNERS = ["@syssi"]

CONF_ERRORS = "errors"
CONF_TOTAL_RUNTIME_FORMATTED = "total_runtime_formatted"

ICON_ERRORS = "mdi:alert-circle-outline"

TEXT_SENSORS = [
    CONF_ERRORS,
    CONF_TOTAL_RUNTIME_FORMATTED,
]

CONFIG_SCHEMA = LOLAN_BMS_BLE_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_ERRORS): text_sensor.text_sensor_schema(
            icon=ICON_ERRORS,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_TOTAL_RUNTIME_FORMATTED): text_sensor.text_sensor_schema(
            icon=ICON_TIMELAPSE,
        ),
    }
)


async def to_code(config):
    from . import CONF_LOLAN_BMS_BLE_ID

    hub = await cg.get_variable(config[CONF_LOLAN_BMS_BLE_ID])
    for key in TEXT_SENSORS:
        if key in config:
            conf = config[key]
            sens = await text_sensor.new_text_sensor(conf)
            cg.add(getattr(hub, f"set_{key}_text_sensor")(sens))
