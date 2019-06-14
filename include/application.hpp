#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "cpu_temp_reader.hpp"
#include "socket_handler.hpp"
#include <fstream>
#include <thread>
#include <string>
#include <csignal>

class Application {
private:
	inline static bool running = false;
	Cpu_temp_reader* cpu_temp_reader;
	Socket_handler* socket_handler;

	static void termination_handler(int) {
		spdlog::info("Termination requested");
		running = false;
	}
	
public:
	void init_log() {
		auto file_logger = spdlog::basic_logger_mt("cpu_temp", "/tmp/cpu_temp.log");
		spdlog::set_default_logger(file_logger);
		spdlog::default_logger()->flush_on(spdlog::level::level_enum::info);
	}

	void init_signals() {
		std::signal(SIGTERM, Application::termination_handler);
	}
	
	int exec() {
		cpu_temp_frame_t cpu_temp_frame;
		
		init_log();
		init_signals();
		spdlog::info("Starting app...");
		
		try {
			socket_handler = new Socket_handler();
			cpu_temp_reader = new Cpu_temp_reader("/sys/class/thermal/thermal_zone0/temp");
			running = true;
		} catch (const std::exception& e) {
			spdlog::error(e.what());
		}
		
		while (running) {
			std::this_thread::sleep_for(std::chrono::seconds(1));

			try {
				cpu_temp_frame = cpu_temp_reader->read();
				socket_handler->publish(cpu_temp_frame);
			} catch (const std::exception& e) {
				spdlog::error(e.what());
				running = false;
			}
		}
		
		return 0;
	}
};
