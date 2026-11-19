import esphome.codegen as cg
from esphome.components import binary_sensor
import esphome.config_validation as cv
from esphome.const import DEVICE_CLASS_CONNECTIVITY, ENTITY_CATEGORY_DIAGNOSTIC

from . import CONF_LOLAN_BMS_BLE_ID, LOLAN_BMS_BLE_COMPONENT_SCHEMA
from .const import CONF_CHARGING, CONF_DISCHARGING

DEPENDENCIES = ["lolan_bms_ble"]

CODEOWNERS = ["@syssi"]

CONF_ONLINE_STATUS = "online_status"

# key: binary_sensor_schema kwargs
BINARY_SENSOR_DEFS = {
    CONF_ONLINE_STATUS: {
        "device_class": DEVICE_CLASS_CONNECTIVITY,
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC,
    },
    CONF_CHARGING: {
        "icon": "mdi:battery-charging",
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC,
    },
    CONF_DISCHARGING: {
        "icon": "mdi:power-plug",
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC,
    },
}

CONFIG_SCHEMA = LOLAN_BMS_BLE_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(key): binary_sensor.binary_sensor_schema(**kwargs)
        for key, kwargs in BINARY_SENSOR_DEFS.items()
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_LOLAN_BMS_BLE_ID])
    binary_sensors = binary_sensor.sub_binary_sensors(config)
    await binary_sensors(CONF_ONLINE_STATUS, hub.set_online_status_binary_sensor)
    await binary_sensors(CONF_CHARGING, hub.set_charging_binary_sensor)
    await binary_sensors(CONF_DISCHARGING, hub.set_discharging_binary_sensor)
