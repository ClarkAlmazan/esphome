#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace hc8 {

class HC8Component : public PollingComponent, public uart::UARTDevice {
 public:
  float get_setup_priority() const override;
  
  void setup() override;
  void update() override;
  
  void dump_config() override;

  void set_co2_sensor(sensor::Sensor *co2_sensor) { co2_sensor_ = co2_sensor; }

  protected:
    bool hc8_read_command_(uint8_t *response);

  sensor::Sensor *co2_sensor_{nullptr};
};

}
}
