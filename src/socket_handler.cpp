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

cpu_temp_frame_t process_data(std::string data) {
	try {
		cpu_temp_frame_t cpu_temp_frame(std::stof(data));
	} catch (const std::exception& e) {
		spdlog::error(e.what());
	}

	return cpu_temp_frame;
}

void Socket_handler::send_data(cpu_temp_frame_t& cpu_temp_frame) {
	std::string topic = "cpu_temp";
	zmq::message_t msg(&cpu_temp_frame, sizeof(cpu_temp_frame));   
	
	try {
		socket->send(topic.begin(), topic.end(), ZMQ_SNDMORE);
		socket->send(msg);
	} catch (const zmq::error_t& e) {
		throw network_error(e.what());
	}
}

void publish(std::string data) {
	auto frame = process_data(data);
	try {
		send_data(frame);
	} catch(const network_error& e) {
		spdlog::error(e.what());
	}
}
