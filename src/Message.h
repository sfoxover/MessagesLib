#pragma once
/*
	CMessage - message container
*/
#include <vector>
#include <string> 
#include <map>
#include <any>
#include <algorithm>
#include "json/json.h"
#include "defines.h"
#include <opencv2/opencv.hpp>

// Message container version
#define MESSAGE_VERSION "1.0"

// Message header start marker sequence
const unsigned char MESSAGE_MARKER_START[] = { 0xD1, 0xFF, 0xD2, 0xFE };

// Message header end marker sequence
const unsigned char MESSAGE_MARKER_END[] = { 0xD1, 0xFF, 0xD3, 0xFA };

class MessageFactory;

class CMessage
{
public:
	// Suported message types
	enum MessageType
	{
		Unknown,
		OpenCVMatFrame,
		FaceDetection,
		Audio,
		MotionSensor,
		ServerCommand,
		ProfilingData,
		Other
	};

	CMessage();
	~CMessage();

protected:
// Properties

	// A map of any types that gets converted to and from json
	std::map<std::string, std::any> _headerMap;
	
	// Message payload
	std::vector<unsigned char> _data;	

// Methods
	void SetMicroTime();

public: 
// Methods

	// Overloaded operators
	CMessage& operator=(const CMessage& value);
	bool operator==(const CMessage& value);
	bool operator!=(const CMessage& value);

	// Get Set a header key value pair in _headerMap 
	bool GetHeaderMapValue(std::string key, std::any& value);
	void SetHeaderMapValue(std::string key, const std::any value);

	// Get Set for _data buffer
	void GetData(std::vector<unsigned char>& value);
	void SetData(std::vector<unsigned char>& value, bool move=true);

	// Get Set for _type
	MessageType GetType();
	void SetType(MessageType value);

	// Get Set for _topic
	void GetTopic(std::string& value);
	void SetTopic(std::string value);

	// Get Set for _microTimeStamp
	void GetMicroTimeStamp(int64_t& value);
	void SetMicroTimeStamp(int64_t value);

	// Deserialize buffer into message properties topic + magic marker + message type + micro seconds + data
	void DeserializeBufferToMessage(std::vector<unsigned char>& buffer);

	// Serialize message properties topic + magic marker + message type + micro seconds + message to buffer
	void SerializeMessageToBuffer(std::vector<unsigned char>& buffer);

	// Convert header map to json
	static void SerializeHeaderMapToJson(const std::map<std::string, std::any> map, Json::Value& json);

	friend class MessageFactory;
};