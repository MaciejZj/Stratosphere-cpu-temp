#pragma once
#include <cstdint>
#include <string>

namespace frame_topic {
  static const std::string cpu_temp = "cpu_temp";
}

struct cpu_temp_frame_t {
  float temperature;

  cpu_temp_frame_t() {}

  cpu_temp_frame_t(
    float temperature
  ) 
  : temperature(temperature)
  {}
};
