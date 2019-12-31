#pragma once
/*
	MessageFactory - create Message objects
*/
#include <any>
#include <map>
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>
#include <functional>
#include "Message.h"
#include "json/json.h"
#include <cassert>

class MessageFactory
{
public:
	// Create message from key value pairs
	static CMessage Create(std::string topic, CMessage::MessageType type, std::map<std::string, std::any> items);

	// Create message from data buffer
	static CMessage Create(std::string topic, CMessage::MessageType type, std::vector<unsigned char>& buffer);

	// Create a message from a cv::Mat frame
	static CMessage Create(std::string topic, CMessage::MessageType type, cv::Mat frame, int fps=-1);
};