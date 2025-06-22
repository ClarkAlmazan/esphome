import esphome.codegen as cg
from esphome.components import uart
import esphome.config_validation as cv
from esphome.const import CONF_ID

MULTI_CONF = True
DEPENDENCIES = ["uart"]

CONF_FOSHAN_MPPT_ID = "foshan_mppt_id"

foshanmppt_ns = cg.esphome_ns.namespace("foshan_mppt")
FoshanMPPTComponent = foshanmppt_ns.class_(
    "FoshanMPPT", cg.PollingComponent, uart.UARTDevice
)


CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(FoshanMPPTComponent),
        }
    )
    .extend(uart.UART_DEVICE_SCHEMA)
    .extend(cv.polling_component_schema("60s"))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
    # cg.add(var.set_address(config[CONF_ADDRESS]))
