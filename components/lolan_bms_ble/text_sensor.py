import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv
from esphome.const import CONF_ID, ICON_TIMELAPSE

from . import CONF_LOLAN_BMS_BLE_ID, LolanBmsBle

DEPENDENCIES = ["lolan_bms_ble"]

CODEOWNERS = ["@syssi"]

CONF_ERRORS = "errors"
CONF_TOTAL_RUNTIME_FORMATTED = "total_runtime_formatted"

ICON_ERRORS = "mdi:alert-circle-outline"

TEXT_SENSORS = [
    CONF_ERRORS,
    CONF_TOTAL_RUNTIME_FORMATTED,
]

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_LOLAN_BMS_BLE_ID): cv.use_id(LolanBmsBle),
        cv.Optional(CONF_ERRORS): text_sensor.text_sensor_schema(
            text_sensor.TextSensor,
            icon=ICON_ERRORS,
        ),
        cv.Optional(CONF_TOTAL_RUNTIME_FORMATTED): text_sensor.text_sensor_schema(
            text_sensor.TextSensor,
            icon=ICON_TIMELAPSE,
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_LOLAN_BMS_BLE_ID])
    for key in TEXT_SENSORS:
        if key in config:
            conf = config[key]
            sens = cg.new_Pvariable(conf[CONF_ID])
            await text_sensor.register_text_sensor(sens, conf)
            cg.add(getattr(hub, f"set_{key}_text_sensor")(sens))
