#pragma once
/*
	Read video frames with ZeroMQ
*/
#include <thread>
#include <chrono>
#include <condition_variable>
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

	// Wait event will signal if exiting
	std::condition_variable _stopEvent;
	std::mutex _stopEventLock;

	// Exit thread flag
	bool _exitingFlag;
	std::mutex _exitingFlagLock;

	// Streaming thread
	std::thread _readSubThread;

	// Message queue
	std::vector<CMessage> _messageQueue;
	std::mutex _messageQueueLock;

public:
// Methods

	// Get set for _exitingFlag
	void GetExitingFlag(bool& value);
	void SetExitingFlag(bool value);

	// Start streaming video from hardware camera
	bool Start(std::wstring &error);

	// Stop reading video frames
	bool Stop(std::wstring &error);

	// Video reading thread
	static void ReadThread(CSubscribeToMsgs *pThis, std::unique_ptr<zmq::socket_t> subscriber, std::unique_ptr<zmq::context_t> context);

	// Get all messages in queue and empty queue
	void GetCurrentMessageQueue(std::vector<CMessage> &results);
};