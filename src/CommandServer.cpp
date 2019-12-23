#include "defines.h"
#include "CommandServer.h"
#include <future>
#include <Settings.h>

CommandServer::CommandServer()
{
}

CommandServer::~CommandServer()
{
	std::wstring error;
	bool bOK = Stop(error);
	assert(bOK);
}

// Start reading client requests on new thread
bool CommandServer::Start(std::wstring& error)
{
	// zeromq subscriber object
	auto uris = CSettings::Instance().GetCmdServerUris();
	if (uris.size() != 1)
	{
		std::wstringstream stError;
		stError << L"Error command server config requires a single endpoint to connect to. Detected " << uris.size() << L" endpoints.";
		error = stError.str();
		assert(uris.size() == 1);
		return false;
	}
	auto context = std::make_unique<zmq::context_t>(1);
	auto server = std::make_unique<zmq::socket_t>(*context, ZMQ_REP);
	server->bind(uris[0]);
	std::cout << "Command server listening on " << uris[0] << std::endl;

	_stopReadThreadSignal = std::make_unique<std::promise<void>>();
	auto futureObj = _stopReadThreadSignal->get_future();
	_readSubThread = std::thread(&CommandServer::ReadThread, this, std::move(futureObj), std::move(server), std::move(context));
	return true;
}

// Stop video streaming
bool CommandServer::Stop(std::wstring& error)
{
	if (_stopReadThreadSignal)
	{
		_stopReadThreadSignal->set_value();
		_stopReadThreadSignal.reset(nullptr);
	}
	if (_readSubThread.joinable())
		_readSubThread.join();

	return true;
}

// Message topics read thread
void CommandServer::ReadThread(CommandServer* pThis, std::future<void> futureObj, std::unique_ptr<zmq::socket_t> subscriber, std::unique_ptr<zmq::context_t> context)
{
	long delay = 10;
	std::future_status waitResult;
	do
	{
		zmq::message_t msg;
		auto result = subscriber->recv(msg, zmq::recv_flags::dontwait);
		if (result.has_value() && result.value() > 0)
		{
			// Read message command
			std::vector<unsigned char> buffer((const char*)msg.data(), (const char*)msg.data() + msg.size());
			CMessage msg;
			msg.DeserializeBufferToMessage(buffer);
			
			waitResult = futureObj.wait_for(std::chrono::milliseconds(1));
		}
		else
		{
			waitResult = futureObj.wait_for(std::chrono::milliseconds(delay));
		}
	} while (waitResult == std::future_status::timeout);
}
