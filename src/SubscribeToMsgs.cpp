#include "defines.h"
#include "SubscribeToMsgs.h"
#include "MessageHelper.h"
#include "Settings.h"
#include <iosfwd>

CSubscribeToMsgs::CSubscribeToMsgs()
{
	SetExitingFlag(false);
}

CSubscribeToMsgs::~CSubscribeToMsgs()
{
	std::wstring error;
	bool bOK = Stop(error);
	assert(bOK);
}

// Get set for _exitingFlag
void CSubscribeToMsgs::GetExitingFlag(bool& value)
{
	_exitingFlagLock.lock();
	value = _exitingFlag;
	_exitingFlagLock.unlock();
}

void CSubscribeToMsgs::SetExitingFlag(bool value)
{
	_exitingFlagLock.lock();
	_exitingFlag = value;
	_exitingFlagLock.unlock();
}

// Start reading data from publisher
bool CSubscribeToMsgs::Start(std::wstring &error)
{
	// zeromq subscriber object
	auto uri = CSettings::Instance().GetSubscribeUri();
	if(uri.empty())
	{
		error = L"Error SubscriberEndpoint is missing from the json config file";
		return false;
	}
	auto context = std::make_unique<zmq::context_t>(1);
	auto subscriber = std::make_unique<zmq::socket_t>(*context, ZMQ_SUB);
	subscriber->connect(uri);

	// Subscrible to single server
	auto topic = CSettings::Instance().GetVideoCamTopic();
	subscriber->setsockopt(ZMQ_SUBSCRIBE, topic.c_str(), topic.size());

	_readSubThread = std::thread(&CSubscribeToMsgs::ReadThread, this, std::move(subscriber), std::move(context));
	return true;
}

// Stop video streaming
bool CSubscribeToMsgs::Stop(std::wstring &error)
{
	SetExitingFlag(true);
	_stopEvent.notify_all();

	if (_readSubThread.joinable())
		_readSubThread.join();

	return true;
}

// Message topics read thread
void CSubscribeToMsgs::ReadThread(CSubscribeToMsgs *pThis, std::unique_ptr<zmq::socket_t> subscriber, std::unique_ptr<zmq::context_t> context)
{
	long delay = 10;
	bool exiting = false;
	do
	{
		pThis->GetExitingFlag(exiting);
		if (!exiting)
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
			}
			std::unique_lock<std::mutex> lock(pThis->_stopEventLock);
			pThis->_stopEvent.wait_for(lock, std::chrono::milliseconds(delay));
		}
	} 
	while (!exiting);
}

// Get all messages in queue and empty queue
void CSubscribeToMsgs::GetCurrentMessageQueue(std::vector<CMessage>& results)
{
	_messageQueueLock.lock();
	results = std::move(_messageQueue);
	_messageQueueLock.unlock();
}
