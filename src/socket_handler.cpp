#include "socket_handler.hpp"
#include "spdlog/spdlog.h"
#include <sstream>
#include <libconfig.h++>

Socket_handler::Socket_handler() {
	libconfig::Config cfg;
	try {
		cfg.readFile("/etc/sp-config/sp.cfg");
	} catch (const libconfig::FileIOException &fioex) {
		throw config_error("I/O error while reading file.");
	} catch (const libconfig::ParseException &pex) {
		std::stringstream ss;
		ss << "Parse error at " << pex.getFile() << ":" << pex.getLine()
				  << " - " << pex.getError() << std::endl;
		throw config_error(ss.str().c_str());
	}

	const libconfig::Setting& root = cfg.getRoot();
	int port;
	if (!root["cpu_temp"].lookupValue("cpu_temp_port", port)) {
		throw config_error("CPU temperature file not defined in config.");
	}
	spdlog::info("Config parsed.");

	try {
		context = zmq::context_t(1);
		socket = new zmq::socket_t(context, ZMQ_PUB);
		if (socket != nullptr) {
			socket->bind("tcp://*:" + std::to_string(port));
		} else {
			throw network_error("Error while creating socket.");
		}
	} catch (const zmq::error_t &e) {
		throw network_error(e.what());
	}
	spdlog::info("Socket opened at: {}", port);
}

Socket_handler::~Socket_handler() {
	socket->close();
	delete socket;
	context.close();
}

void Socket_handler::send_data(cpu_temp_frame_t& frame) {
	try {
		socket->send(frame_topic::cpu_temp.begin(), frame_topic::cpu_temp.end(), ZMQ_SNDMORE);
		socket->send(zmq::const_buffer(&frame, sizeof(frame)));
	} catch (const zmq::error_t& e) {
		throw network_error(e.what());
	}
}

void Socket_handler::publish(cpu_temp_frame_t& frame) {
	try {
		send_data(frame);
	} catch (const network_error& e) {
		spdlog::error(e.what());
	}
}
