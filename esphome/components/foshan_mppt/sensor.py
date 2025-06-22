import esphome.codegen as cg
from esphome.components import sensor
import esphome.config_validation as cv
from esphome.const import (
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_ENERGY,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_VOLTAGE,
    STATE_CLASS_MEASUREMENT,
    UNIT_AMPERE,
    UNIT_CELSIUS,
    UNIT_KILOWATT_HOURS,
    UNIT_VOLT,
)

from . import CONF_FOSHAN_MPPT_ID, FoshanMPPTComponent

CONF_PV = "pv"
CONF_PV_VOLTAGE = "pv_voltage"
CONF_BATTERY = "battery"
CONF_BATTERY_VOLTAGE = "battery_voltage"
CONF_CHARGE_VOLTAGE = "charging_voltage"
CONF_CHARGE_CURRENT = "charging_current"
CONF_DC_OUTPUT_CURRENT = "dc_output_current"
CONF_BATTERY_TEMPERATURE = "battery_temperature"
CONF_MPPT_TEMPERATURE = "mppt_temperature"
CONF_TOTAL_ENERGY_PRODUCTION = "total_energy_production"


DEPENDENCIES = ["uart"]


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_FOSHAN_MPPT_ID): cv.declare_id(FoshanMPPTComponent),
        cv.Optional(CONF_PV_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CHARGE_CURRENT): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_CURRENT,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CHARGE_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_BATTERY_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_BATTERY_TEMPERATURE): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        # controller temperature
        cv.Optional(CONF_MPPT_TEMPERATURE): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_TOTAL_ENERGY_PRODUCTION): sensor.sensor_schema(
            unit_of_measurement=UNIT_KILOWATT_HOURS,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_ENERGY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    # var = cg.new_Pvariable(config[CONF_ID])
    # await cg.register_component(var, config)
    # await uart.register_uart_device(var, config)
    var = await cg.get_variable(config[CONF_FOSHAN_MPPT_ID])

    for key in [
        CONF_PV_VOLTAGE,
        CONF_CHARGE_CURRENT,
        CONF_CHARGE_VOLTAGE,
        CONF_BATTERY_VOLTAGE,
        CONF_BATTERY_TEMPERATURE,
        CONF_MPPT_TEMPERATURE,
        CONF_TOTAL_ENERGY_PRODUCTION,
    ]:
        if key not in config:
            continue
        conf = config[key]
        sens = await sensor.new_sensor(conf)
        cg.add(getattr(var, f"set_{key}_sensor")(sens))
