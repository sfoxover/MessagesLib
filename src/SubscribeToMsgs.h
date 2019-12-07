#pragma once
/*
	Read video frames with ZeroMQ
*/
#include <thread>
#include <future>
#include <chrono>
#include <zmq.hpp>
#include <opencv2/opencv.hpp>
#include "Message.h"

class CSubscribeToMsgs
{
public:
	CSubscribeToMsgs();
	~CSubscribeToMsgs();

private:
	// Properties

	// Exit thread signal
	std::unique_ptr<std::promise<void>> _stopReadThreadSignal;

	// Streaming thread
	std::thread _readSubThread;

	// Message queue
	std::vector<CMessage> _messageQueue;
	std::mutex _messageQueueLock;

public:
// Methods

	// Start streaming video from hardware camera
	bool Start(std::string connectEndpoint, std::wstring &error);

	// Stop reading video frames
	bool Stop(std::wstring &error);

	// Video reading thread
	static void ReadThread(CSubscribeToMsgs *pThis, std::future<void> futureObj, std::unique_ptr<zmq::socket_t> subscriber, std::unique_ptr<zmq::context_t> context);

	// Get all messages in queue and empty queue
	void GetCurrentMessageQueue(std::vector<CMessage> &results);
};