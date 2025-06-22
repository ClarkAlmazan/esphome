#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace foshan_mppt {

class FoshanMPPT : public uart::UARTDevice, public PollingComponent {
 public:
  void setup() override;
  void update() override;
  void dump_config() override;

  void set_pv_voltage_sensor(sensor::Sensor *pv_voltage) { pv_voltage_ = pv_voltage; }
  void set_charging_voltage_sensor(sensor::Sensor *charging_voltage) { charging_voltage_ = charging_voltage; }
  void set_charging_current_sensor(sensor::Sensor *charging_current) { charging_current_ = charging_current; }
  void set_battery_voltage_sensor(sensor::Sensor *battery_voltage) { battery_voltage_ = battery_voltage; }
  void set_mppt_temperature_sensor(sensor::Sensor *mppt_temperature) { mppt_temperature_ = mppt_temperature; }
  void set_total_energy_production_sensor(sensor::Sensor *sensor) { total_energy_production_ = sensor; }
  void set_charge_mode_text_sensor(text_sensor::TextSensor *sensor) { this->charge_mode_ = sensor; }
  void set_error_text_sensor(text_sensor::TextSensor *sensor) { this->error_ = sensor; }

 protected:
  sensor::Sensor *pv_voltage_{nullptr};
  sensor::Sensor *charging_voltage_{nullptr};
  sensor::Sensor *charging_current_{nullptr};
  sensor::Sensor *battery_voltage_{nullptr};
  sensor::Sensor *mppt_temperature_{nullptr};
  sensor::Sensor *battery_temperature_{nullptr};
  sensor::Sensor *total_energy_production_{nullptr};
  text_sensor::TextSensor *charge_mode_{nullptr};
  text_sensor::TextSensor *error_{nullptr};
};

}  // namespace foshan_mppt
}  // namespace esphome