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
	_zeroMqContext = std::make_shared<zmq::context_t>(1);
	_zeroMqSocket = std::make_shared<zmq::socket_t>(*_zeroMqContext, ZMQ_PUB);
	_zeroMqSocket->bind(endpoint);
	std::cout << "Publisher listening on " << endpoint << std::endl;

	return true;
}

// Send message buffer with topic
bool CPublishMessage::SendMessageData(CMessage &msg, std::wstring &error)
{
	// Load message topic and data
	std::vector<unsigned char> buffer;
	msg.SerializeMessageToBuffer(buffer);

	// Check we are connected
	bool bOK = true;
	if (!_zeroMqSocket || !_zeroMqSocket->connected())
	{
		bOK = false;
		error = L"Send message failed, publisher is disconnected.";
	}
	else
	{
		// Send data buffer
		zmq::message_t frame(&buffer[0], buffer.size());
		auto result = _zeroMqSocket->send(frame, zmq::send_flags::dontwait);
		bOK = result.has_value() && result.value() == buffer.size() && bOK;
		assert(bOK);
	}
	return bOK;
}
