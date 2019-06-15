#include "cpu_temp_reader.hpp"
#include "spdlog/spdlog.h"

Cpu_temp_reader::Cpu_temp_reader(std::string file_path) {
	cpu_temp_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	cpu_temp_file.open(file_path);
}

Cpu_temp_reader::~Cpu_temp_reader() {
	cpu_temp_file.close();
}

cpu_temp_frame_t Cpu_temp_reader::read() {
	std::string readout;
	cpu_temp_file >> readout;
	cpu_temp_file.seekg(0, std::ios::beg);
	return build_frame(readout);
}

cpu_temp_frame_t Cpu_temp_reader::build_frame(std::string data) {
	float temperature = std::stof(data)/1000;
	spdlog::info("Cpu temperature: {}", temperature);
	return cpu_temp_frame_t(temperature);
}
