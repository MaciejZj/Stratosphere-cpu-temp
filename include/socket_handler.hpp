#pragma once
#include <zmq.hpp>
#include <string>
#include "gpsframe.hpp"

class Socket_handler {
	private:
		zmq::socket_t* socket;
		zmq::context_t context;
		
		cpu_temp_frame_t process_data(std::string data);
		void send_data(cpu_temp_frame_t& cpu_temp_frame);

	public:
		Socket_handler();
		~Socket_handler();
		void publish();
};