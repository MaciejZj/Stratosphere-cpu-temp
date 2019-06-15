#include "strato-frames/cpu_temp_frame.hpp"
#include <fstream>
#include <iostream>
#include <string>

class Cpu_temp_reader {
	public:
		Cpu_temp_reader(std::string file_path);
		~Cpu_temp_reader();
		inline static cpu_temp_frame_t build_frame(std::string data);
		cpu_temp_frame_t read();
		
	private:
		std::ifstream cpu_temp_file;
};
