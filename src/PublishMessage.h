#pragma once
/*
	Publish a message with ZeroMQ
*/

#include <zmq.hpp>
#include "Message.h"

class CPublishMessage
{
private:
	CPublishMessage();
	~CPublishMessage();

private:
// Properties
	// zeromq publisher object
	std::vector<std::shared_ptr<zmq::socket_t>> _connections;
	std::vector<std::shared_ptr<zmq::context_t>> _contexts;

public:
// Methods
	static CPublishMessage& Instance()
	{
		static CPublishMessage instance;
		return instance;
	}

	// Initialize publisher and bind to endpoint e.g. "tcp://*:5563"
	bool Initialize(std::string endpoint, std::wstring& error);

	// Send message packet with topic
	bool SendMessageData(CMessage& msg, std::wstring& error);

};