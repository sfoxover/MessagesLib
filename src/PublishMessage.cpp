#include "defines.h"
#include "PublishMessage.h"

CPublishMessage::CPublishMessage()
{
}

CPublishMessage::~CPublishMessage()
{
}

// Initialize publisher and bind to endpoint e.g. "tcp://*:5563"
bool CPublishMessage::Initialize(std::string endpoint, std::wstring &error)
{
	auto context = std::make_shared<zmq::context_t>(1);
	auto socket = std::make_shared<zmq::socket_t>(*context, ZMQ_PUB);
	socket->bind(endpoint);
	_contexts.push_back(context);
	_connections.push_back(socket);
	std::cout << "Publisher listening on " << endpoint << std::endl;

	return true;
}

// Send message buffer with topic
bool CPublishMessage::SendMessageData(CMessage &msg, std::wstring &error)
{
	// Load message topic and data
	std::vector<unsigned char> buffer;
	msg.SerializeMessageToBuffer(buffer);

	// Send to all endpoints
	bool bOK = true;
	std::for_each(_connections.begin(), _connections.end(), [&](auto socket) {		
		// Check we are connected
		if (!socket->connected())
		{
			bOK = false;
			error = L"Send message failed, publisher is disconnected.";
		}
		else
		{
			// Send data buffer
			zmq::message_t frame(&buffer[0], buffer.size());
			auto result = socket->send(frame, zmq::send_flags::dontwait);
			bOK = result.has_value() && result.value() == buffer.size() && bOK;
			assert(bOK);
		}
	});
	return bOK;
}
