#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "strato-frames/cpu_temp_frame.hpp"
#include "strato-frames-publisher/publisher.hpp"
#include "cpu_temp_reader.hpp"
#include <thread>
#include <csignal>

class Application {
private:
	inline static bool running = false;
	constexpr static const auto publishing_period = std::chrono::seconds(1);
	std::unique_ptr<Cpu_temp_reader> cpu_temp_reader;
	std::unique_ptr<Publisher<cpu_temp_frame_t>> cpu_temp_publisher;

	static void termination_handler(int) {
		spdlog::info("Termination requested");
		running = false;
	}
	
public:
	void init_log() {
		auto file_logger = spdlog::basic_logger_mt(
			"cpu_temp", "/var/log/balloon/cpu_temp.log");
		spdlog::set_default_logger(file_logger);
		spdlog::default_logger()->flush_on(spdlog::level::level_enum::info);
	}

	void init_signals() {
		std::signal(SIGTERM, Application::termination_handler);
	}
	
	void init_network_publisher() {
		cpu_temp_publisher = std::make_unique<Publisher<cpu_temp_frame_t>>(
			"/etc/sp-config/sp.cfg", "cpu_temp", "cpu_temp_port", "cpu_temp");
	}
	
	void init() {
		init_log();
		init_signals();
		spdlog::info("Starting app...");
		
		try {
			init_network_publisher();				
			cpu_temp_reader = std::make_unique<Cpu_temp_reader>(
				"/sys/class/thermal/thermal_zone0/temp");	
			running = true;
		} catch (const std::exception& e) {
			spdlog::error(e.what());
		}
	}
	
	int exec() {			
		init();
		
		cpu_temp_frame_t cpu_temp_frame;		
		while (running) {
			std::this_thread::sleep_for(publishing_period);
			
			try {
				cpu_temp_frame = cpu_temp_reader->read();
				cpu_temp_publisher->send(cpu_temp_frame);
			} catch (const std::exception& e) {
				spdlog::error(e.what());
				running = false;
			}
		}	
		return 0;
	}
};
