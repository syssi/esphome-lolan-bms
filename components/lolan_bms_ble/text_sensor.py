import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv
from esphome.const import ENTITY_CATEGORY_DIAGNOSTIC, ICON_TIMELAPSE

from . import CONF_LOLAN_BMS_BLE_ID, LOLAN_BMS_BLE_COMPONENT_SCHEMA

DEPENDENCIES = ["lolan_bms_ble"]

CODEOWNERS = ["@syssi"]

CONF_ERRORS = "errors"
CONF_TOTAL_RUNTIME_FORMATTED = "total_runtime_formatted"

ICON_ERRORS = "mdi:alert-circle-outline"

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
    hub = await cg.get_variable(config[CONF_LOLAN_BMS_BLE_ID])
    text_sensors = text_sensor.sub_text_sensors(config)
    await text_sensors(CONF_ERRORS, hub.set_errors_text_sensor)
    await text_sensors(
        CONF_TOTAL_RUNTIME_FORMATTED, hub.set_total_runtime_formatted_text_sensor
    )
