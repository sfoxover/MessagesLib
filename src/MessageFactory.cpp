#include "MessageFactory.h"

// Create message from key value pairs
CMessage MessageFactory::Create(std::string topic, int32_t type, std::map<std::string, std::any> items)
{
	CMessage msg;

	// Set topic and type
	msg.SetTopic(topic);
	msg.SetType(type);
	msg.SetMicroTime();

	msg._headerMap.insert(items.begin(), items.end());
	return msg;
}

// Set message values including data buffer
CMessage MessageFactory::Create(std::string topic, int type, std::vector<unsigned char>& buffer)
{
	CMessage msg;

	// Set topic and type
	msg.SetTopic(topic);
	msg.SetType(type);
	msg.SetMicroTime();

	// Copy to buffer
	msg.SetData(buffer);
	return msg;
}

// Create a message from a cv::Mat frame
CMessage MessageFactory::Create(std::string topic, int32_t type, cv::Mat frame, int fps)
{
	CMessage msg;

	// Set topic and type
	msg.SetTopic(topic);
	msg.SetType(type);
	msg.SetMicroTime();

	// Set cv::Mat specific parameters
	if (fps >= 0)
	{
		msg._headerMap["fps"] = fps;
	}
	msg._headerMap["width"] = (int)frame.cols;
	msg._headerMap["height"] = (int)frame.rows;
	msg._headerMap["step"] = (int)frame.step;

	// Use mat buffer to avoid cpu cost for conversion
	std::vector<uchar> videoBuffer(frame.datastart, frame.dataend);
	msg.SetData(videoBuffer);
	return msg;
}

