#include "spdlog/spdlog.h"
#include <libconfig.h++>

Socket_handler::Socket_handler() {
	libconfig::Config cfg;
	try {
		cfg.readFile("/etc/sp/sp.cfg");
	} catch (const libconfig::FileIOException &fioex) {
		throw ublox::config_error("I/O error while reading file.");
	} catch (const libconfig::ParseException &pex) {
		std::stringstream ss;
		ss << "Parse error at " << pex.getFile() << ":" << pex.getLine()
				  << " - " << pex.getError() << std::endl;
		throw ublox::config_error(ss.str().c_str());
	}

	const libconfig::Setting& root = cfg.getRoot();
	int port;
	if (!root["cpu_temp"].lookupValue("cpu_temp_port", port)) {
		throw ublox::config_error("CPU temperature file not defined in config.");
	}
	spdlog::info("Config parsed.");

	try {
		context = zmq::context_t(1);
		socket = new zmq::socket_t(context, ZMQ_PUB);
		if (socket != nullptr) {
			socket->bind("tcp://*:" + std::to_string(port));
		} else {
			throw ublox::network_error("Error while creating socket.");
		}
	} catch (const zmq::error_t &e) {
		throw ublox::network_error(e.what());
	}
	spdlog::info("Socket opened at: {}", port);
}

Socket_handler::~Socket_handler() {
	socket->close();
	delete socket;
	context.close();
}
