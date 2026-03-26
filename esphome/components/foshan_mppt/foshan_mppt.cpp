#include "foshan_mppt.h"

#include "esphome/core/log.h"
#include <cinttypes>
#include <cstring>

// error codes
#define BIT_CONTROLLER_OVERHEATING_STATUS 0x1  // 00000001
#define BIT_BATTERY_OVERHEATING_STATUS 0x2     // 00000010
#define BIT_DC_OUTPUT_OVERCURRENT_STATUS 0x4   // 00000100
#define BIT_PV_OVERVOLTAGE_STATUS 0x8          // 00001000
#define BIT_PV_UNDERVOLTAGE_STATUS 0x10        // 00010000
#define BIT_CHARGING_OVERVOLTAGE_STATUS 0x20   // 00100000
namespace esphome {
namespace foshan_mppt {

static const char *const TAG = "foshan_mppt";
static const uint8_t REQUEST_LENGTH = 8;
static const uint8_t RESPONSE_LENGTH = 45;
static const uint8_t DATA_REQUEST[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x14, 0x45, 0xC5};

void FoshanMPPT::setup() {}

void FoshanMPPT::update() {
  uint8_t response[RESPONSE_LENGTH];

  // // Empty RX Buffer
  // while (this->available())
  //   this->read();
  this->write_array(DATA_REQUEST, REQUEST_LENGTH);
  bool ret = this->read_array(response, RESPONSE_LENGTH);
  this->flush();

  if (!ret) {
    ESP_LOGW(TAG, "Reading data from MPPT controller failed!");
    this->status_set_warning();
    return;
  }

  // assume controller id is 1, function code 0x03 when reading data
  if (response[0] != 0x01 & response[1] != 0x03) {
    ESP_LOGW(TAG, "Controller mismatch or wrong function code, possible data corruption");
    while (this->available())
      this->read();
    this->status_set_warning();
    return;
  }

  this->status_clear_warning();
  // Read response
  const float pv_voltage = float(response[25] << 8 | response[26]);
  if (this->pv_voltage_ != nullptr) {
    this->pv_voltage_->publish_state(pv_voltage / 10);
  }

  const float charging_voltage = float(response[27] << 8 | response[28]);
  if (this->charging_voltage_ != nullptr) {
    this->charging_voltage_->publish_state(charging_voltage / 10);
  }

  const float charging_current = float(response[29] << 8 | response[30]);
  if (this->charging_current_ != nullptr) {
    this->charging_current_->publish_state(charging_current / 10);
  }

  const float battery_voltage = float(response[31] << 8 | response[32]);
  if (this->battery_voltage_ != nullptr) {
    this->battery_voltage_->publish_state(battery_voltage / 10);
  }

  const uint16_t mppt_temperature = (uint16_t(response[35]) << 8) | response[36];
  if (this->mppt_temperature_ != nullptr) {
    this->mppt_temperature_->publish_state(mppt_temperature);
  }

  const double total_solar_generation =
      double(response[39] << 24 | response[40] << 16 | response[41] << 8 | response[42]);
  if (this->total_energy_production_ != nullptr) {
    this->total_energy_production_->publish_state(total_solar_generation / 1000);
  }

  // status bytes
  uint8_t low_status_byte = (uint8_t) (response[8]);
  uint8_t high_status_byte = (uint8_t) (response[9]);

// charging modes - low
#define BIT_BOOST_CHARGE_STATUS 0x40     // 01000000
#define BIT_EQUALIZE_CHARGE_STATUS 0x80  // 10000000
// charging modes - high
#define BIT_FLOAT_CHARGE_STATUS 0x1  // 00000001
#define BIT_MPPT_CHARGE_STATUS 0x2   // 00000010
// standby
#define BIT_STANDBY_STATUS 0x80  // 10000000

  std::string error = "";
  if (low_status_byte & BIT_CONTROLLER_OVERHEATING_STATUS) {
    error = error + "Controller overheating ";
  }
  if (low_status_byte & BIT_BATTERY_OVERHEATING_STATUS) {
    error = error + "Battery overheating ";
  }
  if (low_status_byte & BIT_DC_OUTPUT_OVERCURRENT_STATUS) {
    error = error + "DC output overcurrent ";
  }
  if (low_status_byte & BIT_PV_OVERVOLTAGE_STATUS) {
    error = error + "PV overvoltage ";
  }
  if (low_status_byte & BIT_PV_UNDERVOLTAGE_STATUS) {
    error = error + "PV undervoltage";
  }
  if (low_status_byte & BIT_CHARGING_OVERVOLTAGE_STATUS) {
    error = error + "Charging voltage too high ";
  }

  if (error.length() & this->error_ != nullptr) {
    this->error_->publish_state(error);
  }

  std::string charge_mode = "";
  if (low_status_byte & BIT_BOOST_CHARGE_STATUS) {
    charge_mode = "Boost charge";
  }
  if (low_status_byte & BIT_EQUALIZE_CHARGE_STATUS) {
    charge_mode = "Equalizing charge";
  }
  if (high_status_byte & BIT_FLOAT_CHARGE_STATUS) {
    charge_mode = "Float charge";
  }
  if (high_status_byte & BIT_MPPT_CHARGE_STATUS) {
    charge_mode = "MPPT charge";
  }
  if (high_status_byte & BIT_STANDBY_STATUS) {
    charge_mode = "Standby";
  }

  if (charge_mode.length() & this->charge_mode_ != nullptr) {
    this->charge_mode_->publish_state(charge_mode);
  }
}

void FoshanMPPT::dump_config() { this->check_uart_settings(2400); }

}  // namespace foshan_mppt
}  // namespace esphome