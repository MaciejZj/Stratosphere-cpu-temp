#pragma once
#include <zmq.hpp>
#include <string>
#include "cpu_temp_frame.hpp"

class config_error : public std::runtime_error {
	public:
		config_error(const char* what_arg) : std::runtime_error(what_arg) {}
	};

class network_error : public std::runtime_error {
	public:
		network_error(const char* what_arg) : std::runtime_error(what_arg) {}
	};

class Socket_handler {
	private:
		zmq::socket_t* socket;
		zmq::context_t context;
		
		void send_data(cpu_temp_frame_t& frame);

	public:
		Socket_handler();
		~Socket_handler();
		void publish(cpu_temp_frame_t& frame);
};