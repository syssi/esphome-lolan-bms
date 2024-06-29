import esphome.codegen as cg
from esphome.components import ble_client
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_PASSWORD

CODEOWNERS = ["@syssi"]

AUTO_LOAD = ["binary_sensor", "sensor", "text_sensor"]
MULTI_CONF = True

CONF_LOLAN_BMS_BLE_ID = "lolan_bms_ble_id"

lolan_bms_ble_ns = cg.esphome_ns.namespace("lolan_bms_ble")
LolanBmsBle = lolan_bms_ble_ns.class_(
    "LolanBmsBle", ble_client.BLEClientNode, cg.PollingComponent
)

LOLAN_BMS_BLE_COMPONENT_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_LOLAN_BMS_BLE_ID): cv.use_id(LolanBmsBle),
    }
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(LolanBmsBle),
            cv.Optional(CONF_PASSWORD, default="12345678"): cv.uint32_t,
        }
    )
    .extend(ble_client.BLE_CLIENT_SCHEMA)
    .extend(cv.polling_component_schema("10s"))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await ble_client.register_ble_node(var, config)

    cg.add(var.set_password(config[CONF_PASSWORD]))
