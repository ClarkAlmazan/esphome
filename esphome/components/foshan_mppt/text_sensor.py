import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv

from . import CONF_FOSHAN_MPPT_ID, FoshanMPPTComponent

CONF_ERROR = "error"
CONF_CHARGE_MODE = "charge_mode"

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(CONF_FOSHAN_MPPT_ID): cv.use_id(FoshanMPPTComponent),
            cv.Optional(CONF_ERROR): text_sensor.text_sensor_schema(
                text_sensor.TextSensor
            ),
            cv.Optional(CONF_CHARGE_MODE): text_sensor.text_sensor_schema(
                text_sensor.TextSensor
            ),
        }
    ).extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_FOSHAN_MPPT_ID])
    if error_config := config.get(CONF_ERROR):
        sens = await text_sensor.new_text_sensor(error_config)
        cg.add(getattr(hub, f"set_{CONF_ERROR}_text_sensor")(sens))

    if charge_mode_config := config.get(CONF_CHARGE_MODE):
        sens = await text_sensor.new_text_sensor(charge_mode_config)
        cg.add(getattr(hub, f"set_{CONF_CHARGE_MODE}_text_sensor")(sens))
