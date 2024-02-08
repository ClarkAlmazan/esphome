#include "hc8.h"
#include "esphome/core/log.h"

namespace esphome {
namespace hc8 {

static const char *TAG = "hc8";
// active output

static const uint8_t HC8_RESPONSE_LENGTH = 16;


uint8_t hc8_checksum(const uint8_t *command) {
  uint8_t sum = 0;
  for (uint8_t i = 0; i < HC8_RESPONSE_LENGTH - 1; i++) {
    sum += command[i];
  }
  return sum;
}

void HC8Component::setup() {
    // mhz19 has abc enable/disable stuff here but we dont have that
}

void HC8Component::update() {
  uint8_t response[HC8_RESPONSE_LENGTH];
  if (this->available() < HC8_RESPONSE_LENGTH) {
    return;
  }

  if (!this->hc8_read_command_(response)) {
    ESP_LOGW(TAG, "Reading data from HC8 failed!");
    this->status_set_warning();
    return;
  }


  if (response[0] != 0x42 || response[1] != 0x4d) {
    ESP_LOGW(TAG, "Invalid preamble from HC8!");
    ESP_LOGW(TAG, "The rest of the response: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x ",
             response[0], response[1], response[2], response[3], response[4], response[5], response[6], response[7],
             response[8], response[9], response[10], response[11], response[12], response[13], response[14]);
    this->status_set_warning();
    return;
  }

  uint8_t checksum = hc8_checksum(response);
  if (response[15] != checksum) {
    ESP_LOGW(TAG, "HC8 Checksum doesn't match: 0x%02X!=0x%02X", response[15], checksum);
    ESP_LOGW(TAG, "The rest of the response: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x ",
             response[0], response[1], response[2], response[3], response[4], response[5], response[6], response[7],
             response[8], response[9], response[10], response[11], response[12], response[13], response[14]);
    this->status_set_warning();
    return;
  }

  this->status_clear_warning();
  const uint16_t ppm = (uint16_t(response[6]) << 8) | response[7];

  ESP_LOGD(TAG, "HC8 Received CO₂=%uppm", ppm);
  if (this->co2_sensor_ != nullptr)
    this->co2_sensor_->publish_state(ppm);
}

// reading from active output
bool HC8Component::hc8_read_command_(uint8_t *response) {
  // Empty RX Buffer until we find header
  while (this->available()) {
    response[0] = this->read();
    if (response[0] == 0x42) {
      break;
    }
  }

  for (int i = 1; i < HC8_RESPONSE_LENGTH; i++) {
    response[i] = this->read();
  }

  if (response == nullptr)
    return false;

  // return this->read_array(response, HC8_RESPONSE_LENGTH);
  return true;
}

float HC8Component::get_setup_priority() const { return setup_priority::DATA; }

void HC8Component::dump_config(){
    ESP_LOGCONFIG(TAG, "HC8:");
    LOG_SENSOR(" ", "CO2", this->co2_sensor_);
    this->check_uart_settings(9600);
}

}
}
