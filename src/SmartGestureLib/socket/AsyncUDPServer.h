#pragma once

class AsyncUDPServer
{
public:
	AsyncUDPServer(asio::io_service& io_service);
	~AsyncUDPServer();

protected:
	/*Override function:
	This function will be called by do_receive(). 
	The received message can be retrieved by getUDPMsg()
	*/
	auto virtual handle_receive()	-> void;

	/*This function waits for an UDP receive and stores the message into this object.
	Retrieve message by using getUDPMsh()
	*/
	auto do_receive()				-> void;

	/*Retrieve the UDP Msg.*/
	auto getUDPMsg()				-> char*;

	/*Retrieve the IPv4 Address visible by the Local Area Network*/
	auto getIPv4Address()			->std::string;

	/*Terminate and shutdown UDP server*/
	auto shutdown_Server()			-> void;

private:
	std::size_t msg_length;
	char data_[1024];
	char newData_[1024];
	size_t m_bytes_received;
	asio::ip::udp::socket socket_;
	asio::ip::udp::endpoint sender_endpoint_;

};
