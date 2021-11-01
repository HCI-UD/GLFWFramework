#include "asio.hpp"
#include "Common.h"
#include "AsyncUDPServer.h"



AsyncUDPServer::AsyncUDPServer(asio::io_service& io_service):
socket_(io_service, asio::ip::udp::endpoint(asio::ip::udp::v4(), 14321))
{
}

AsyncUDPServer::~AsyncUDPServer()
{

}

auto AsyncUDPServer::do_receive() -> void
{
	socket_.async_receive_from(
		asio::buffer(data_, 1024), sender_endpoint_,
		[this](std::error_code ec, std::size_t bytes_received)
	{
		if (!ec && bytes_received > 0)
		{
			msg_length = bytes_received;
			handle_receive();
		}
		else
		{
			do_receive();
		}
	});
}

auto AsyncUDPServer::handle_receive() -> void
{
}


auto AsyncUDPServer::getUDPMsg() -> char*
{
	char cpyChar[1024];
	strncpy(cpyChar, data_, msg_length);
	cpyChar[msg_length] = '\0';
	return cpyChar;
}

auto AsyncUDPServer::getIPv4Address() -> std::string
{
	asio::ip::udp::resolver resolver(socket_.get_io_service());
	asio::ip::udp::resolver::query query(asio::ip::udp::v4(), asio::ip::host_name(), "");
	asio::ip::udp::resolver::iterator it = resolver.resolve(query);
	asio::ip::udp::endpoint endpoint = *it;
	return endpoint.address().to_string();
}

auto AsyncUDPServer::shutdown_Server() -> void
{
	try {
		socket_.shutdown(asio::socket_base::shutdown_both);
		socket_.close();
	}
	catch (std::exception &err) {
		std::cout << err.what() << std::endl;
	}
}
