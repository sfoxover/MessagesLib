#include "defines.h"
#include "SubscribeToMsgs.h"
#include <MessageHelper.h>
#include "Settings.h"
#include <iosfwd>

CSubscribeToMsgs::CSubscribeToMsgs()
{
}

CSubscribeToMsgs::~CSubscribeToMsgs()
{
	std::wstring error;
	bool bOK = Stop(error);
	assert(bOK);
}

// Start reading data from publisher
bool CSubscribeToMsgs::Start(std::string jsonConfigPath, std::wstring &error)
{
	// Load settings object
	bool bOK = true;
	bOK = CSettings::Instance().Initialize(jsonConfigPath, error);
	if(!bOK)
	{
		std::wcout << L"CSubscribeToMsgs settings initialize failed. Error, " << error << std::endl;
	}
	assert(bOK);

	// zeromq subscriber object
	auto uris = CSettings::Instance().GetSubscribeUris();
	if(uris.size() != 1)
	{
		std::wstringstream stError;
		stError << L"Error subscription config only supports a single endpoint to connect to. Detected " << uris.size() << L" endpoints.";
		error = stError.str();
		assert(uris.size() == 1);
		return false;
	}
	auto context = std::make_unique<zmq::context_t>(1);
	auto subscriber = std::make_unique<zmq::socket_t>(*context, ZMQ_SUB);
	subscriber->connect(uris[0]);

	// Subscrible to single server
	auto topic = CSettings::Instance().GetVideoCamTopic();
	subscriber->setsockopt(ZMQ_SUBSCRIBE, topic.c_str(), topic.size());

	_stopReadThreadSignal = std::make_unique<std::promise<void>>();
	auto futureObj = _stopReadThreadSignal->get_future();
	_readSubThread = std::thread(&CSubscribeToMsgs::ReadThread, this, std::move(futureObj), std::move(subscriber), std::move(context));
	return true;
}

// Stop video streaming
bool CSubscribeToMsgs::Stop(std::wstring &error)
{
	if(_stopReadThreadSignal)
	{
		_stopReadThreadSignal->set_value();
		_stopReadThreadSignal.reset(nullptr);
	}
	if (_readSubThread.joinable())
		_readSubThread.join();

	return true;
}

// Message topics read thread
void CSubscribeToMsgs::ReadThread(CSubscribeToMsgs *pThis, std::future<void> futureObj, std::unique_ptr<zmq::socket_t> subscriber, std::unique_ptr<zmq::context_t> context)
{
	long delay = 10;
	std::future_status waitResult;
	do
	{
		zmq::message_t msg;
		auto result = subscriber->recv(msg, zmq::recv_flags::dontwait);
		if (result.has_value() && result.value() > 0)
		{
			// Add message to queue
			std::vector<unsigned char> buffer((const char*)msg.data(), (const char*)msg.data() + msg.size());
			CMessage msg;
			msg.DeserializeBufferToMessage(buffer);
			pThis->_messageQueueLock.lock();
			pThis->_messageQueue.push_back(msg);
			pThis->_messageQueueLock.unlock();
			waitResult = futureObj.wait_for(std::chrono::milliseconds(1));
		}
		else
		{
			waitResult = futureObj.wait_for(std::chrono::milliseconds(delay));
		}
	} while (waitResult == std::future_status::timeout);
}

// Get all messages in queue and empty queue
void CSubscribeToMsgs::GetCurrentMessageQueue(std::vector<CMessage>& results)
{
	_messageQueueLock.lock();
	results = std::move(_messageQueue);
	_messageQueueLock.unlock();
}
