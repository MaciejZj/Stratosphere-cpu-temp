#include "cpu_temp_reader.hpp"
#include "spdlog/spdlog.h"
#include <sstream>

Cpu_temp_reader::Cpu_temp_reader(std::string file_path) {
	cpu_temp_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		cpu_temp_file.open(file_path);
	} catch (const std::runtime_error &e) {
		std::stringstream err_msg;
		err_msg << "Could not open cpu temperature file at: " << file_path
		        << ", " << e.what();
		throw(std::runtime_error(err_msg.str()));
	}
}

Cpu_temp_reader::~Cpu_temp_reader() {
	cpu_temp_file.close();
}

cpu_temp_frame_t Cpu_temp_reader::read() {
	std::string readout;
	try {
		cpu_temp_file >> readout;
	} catch (const std::runtime_error &e) {
		std::stringstream err_msg;
		err_msg << "Could not read cpu temperature file, " << e.what();
		throw(std::runtime_error(err_msg.str()));
	}
	cpu_temp_file.seekg(0, std::ios::beg);
	return build_frame(readout);
}

cpu_temp_frame_t Cpu_temp_reader::build_frame(std::string data) {
	float temperature = std::stof(data)/1000;
	spdlog::info("Cpu temperature: {}", temperature);
	return cpu_temp_frame_t(temperature);
}
