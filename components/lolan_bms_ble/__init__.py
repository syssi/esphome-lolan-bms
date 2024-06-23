import esphome.codegen as cg
from esphome.components import ble_client
import esphome.config_validation as cv
from esphome.const import CONF_ID

CODEOWNERS = ["@syssi"]

AUTO_LOAD = ["binary_sensor", "sensor", "text_sensor"]
MULTI_CONF = True

CONF_LOLAN_BMS_BLE_ID = "lolan_bms_ble_id"

lolan_bms_ble_ns = cg.esphome_ns.namespace("lolan_bms_ble")
LolanBmsBle = lolan_bms_ble_ns.class_(
    "LolanBmsBle", ble_client.BLEClientNode, cg.PollingComponent
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(LolanBmsBle),
        }
    )
    .extend(ble_client.BLE_CLIENT_SCHEMA)
    .extend(cv.polling_component_schema("10s"))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await ble_client.register_ble_node(var, config)
