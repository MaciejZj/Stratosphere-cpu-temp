#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <fstream>
#include <thread>
#include <csignal>

class Application {
private:
	inline static bool running = false;
	std::fstream cpu_temp_file;
	
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
		string cpu_temp;
		
		init_log();
		init_signals();
		spdlog::info("Starting app...");
		
		myfile.open("example.txt", std::fstream::in);
		
		while (running) {
			std::this_thread::sleep_for(std::chrono::seconds(1));
			if (cpu_temp_file) {
				getline(cpu_temp_file, cpu_temp);
				// TODO: Process temp and send it to socket
			} else {
				spdlog::critical("Failed to read cpu temp file, status flags: " +
				                  std::to_string(cpu_temp_file.rdstate()));
				running = false;
			}
		}
		
		cpu_temp_file.close();
		return 0;
	}

};
