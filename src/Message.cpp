#include "Message.h"
#include "MessageHelper.h"
#include <chrono>
#include <cassert>

CMessage::CMessage()
{
	SetMicroTime();
	_headerMap["version"] = MESSAGE_VERSION;
}

CMessage::~CMessage()
{
}

void CMessage::SetMicroTime()
{
	_headerMap["time_stamp"] = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

CMessage &CMessage::operator=(const CMessage &value)
{
	_data = value._data;
	_headerMap = value._headerMap;
	return *this;
}

bool CMessage::operator==(const CMessage &value)
{
	if (_data.size() != value._data.size())
		return false;
	if (_data.empty())
		return true;

	// Convert header map to json to test
	Json::Value json1, json2;
	SerializeHeaderMapToJson(_headerMap, json1);
	SerializeHeaderMapToJson(value._headerMap, json2);
	if (json1 != json2)
		return false;
	bool equal = std::equal(_data.begin(), _data.end(), value._data.begin());
	return equal;
}

bool CMessage::operator!=(const CMessage &value)
{
	bool equal = !(*this == value);
	return !equal;
}

// Get Set a header key value pair in _headerMap
bool CMessage::GetHeaderMapValue(std::string key, std::any &value)
{
	bool found = _headerMap.find(key) != _headerMap.end();
	if (found)
	{
		value = _headerMap[key];
	}
	return found;
}

void CMessage::SetHeaderMapValue(std::string key, const std::any value)
{
	_headerMap[key] = value;
}

// Get Set for _data
void CMessage::GetData(std::vector<unsigned char> &value)
{
	value = _data;
}

void CMessage::SetData(std::vector<unsigned char> &value, bool move)
{
	_headerMap["data_size"] = value.size();
	if (move)
	{
		_data = std::move(value);
	}
	else
	{
		_data = value;
	}
}

// Get Set for _type
CMessage::MessageType CMessage::GetType()
{
	MessageType type = MessageType::Unknown;
	auto object = _headerMap["type"];
	if (object.has_value() && object.type() == typeid(int32_t))
	{
		type = (MessageType)std::any_cast<int32_t>(object);
	}
	else
	{
		std::cerr << "CMessage::GetType unsupported std::any type: " << object.type().name() << std::endl;
	}
	return type;
}

void CMessage::SetType(MessageType value)
{
	_headerMap["type"] = (int32_t)value;
}

// Get Set for _topic
void CMessage::GetTopic(std::string &value)
{
	auto object = _headerMap["topic"];
	if (object.has_value())
	{
		value = MessageHelper::StringFromAnyValue(object);
	}
}

void CMessage::SetTopic(std::string value)
{
	_headerMap["topic"] = value;
}

// Get Set for _microTimeStamp
void CMessage::GetMicroTimeStamp(int64_t &value)
{
	auto object = _headerMap["time_stamp"];
	if (object.has_value() && object.type() == typeid(int64_t))
	{
		value = std::any_cast<int64_t>(object);
	}
}

void CMessage::SetMicroTimeStamp(int64_t value)
{
	_headerMap["time_stamp"] = value;
}

// Create message from key value pairs
void CMessage::CreateMessageFromJson(std::string topic, MessageType type, std::map<std::string, std::any> items)
{
	// Set topic and type
	SetTopic(topic);
	SetType(type);
	SetMicroTime();

	_headerMap.insert(items.begin(), items.end());
}

// Set message values including data buffer
void CMessage::CreateMessageFromBuffer(std::string topic, MessageType type, std::vector<unsigned char> &buffer)
{
	// Set topic and type
	SetTopic(topic);
	SetType(type);
	SetMicroTime();

	// Copy to buffer
	SetData(buffer);
}

// Create a message from a cv::Mat frame
void CMessage::CreateMessageFromMatFrame(std::string topic, cv::Mat frame, int fps)
{
	// Set topic and type
	SetTopic(topic);
	SetType(OpenCVMatFrame);
	SetMicroTime();

	// Set cv::Mat specific parameters
	_headerMap["fps"] = fps;
	_headerMap["width"] = (int)frame.cols;
	_headerMap["height"] = (int)frame.rows;
	_headerMap["step"] = (int)frame.step;

	// Use mat buffer to avoid cpu cost for conversion
	std::vector<uchar> videoBuffer(frame.datastart, frame.dataend);
	SetData(videoBuffer);
}

// Create a message from a cv::Mat frame with face detection
void CMessage::CreateMessageFromFaceDetectedMatFrame(std::string topic, cv::Mat frame)
{
	// Set topic and type
	SetTopic(topic);
	SetType(FaceDetection);
	SetMicroTime();

	// Set cv::Mat specific parameters
	_headerMap["width"] = (int)frame.cols;
	_headerMap["height"] = (int)frame.rows;
	_headerMap["step"] = (int)frame.step;

	// Use mat buffer to avoid cpu cost for conversion
	std::vector<uchar> videoBuffer(frame.datastart, frame.dataend);
	SetData(videoBuffer);
}

// Deserialize buffer into message properties topic + magic marker + message type + micro seconds + data
void CMessage::DeserializeBufferToMessage(std::vector<unsigned char> &buffer)
{
	SetType(Unknown);

	// Search for start marker after topic
	auto findStart = std::vector<unsigned char>(MESSAGE_MARKER_START, MESSAGE_MARKER_START + sizeof(MESSAGE_MARKER_START));
	auto itMax = buffer.size() > MAX_TOPIC_LENGTH ? buffer.begin() + MAX_TOPIC_LENGTH : buffer.end();
	auto posStart = std::search(buffer.begin(), itMax, findStart.begin(), findStart.end());

	// We found start marker so look for end marker
	if (posStart != itMax)
	{
		auto findEnd = std::vector<unsigned char>(MESSAGE_MARKER_END, MESSAGE_MARKER_END + sizeof(MESSAGE_MARKER_END));
		auto posEnd = std::search(buffer.begin(), buffer.end(), findEnd.begin(), findEnd.end());
		if (posEnd != buffer.end())
		{
			// Load json values into header map
			auto it = posStart + sizeof(MESSAGE_MARKER_START);
			std::string szJson(it, posEnd);
			Json::Value root;
			Json::CharReaderBuilder builder;
			const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
			JSONCPP_STRING err;
			if (reader->parse(szJson.c_str(), szJson.c_str() + szJson.size(), &root, &err))
			{
				// Load all json values into our header map
				MessageHelper::LoadJsonIntoMap(root, _headerMap);
			}

			// Anything left over is the data buffer
			std::vector<unsigned char> data(posEnd + sizeof(MESSAGE_MARKER_END), buffer.end());
			SetData(data);

#ifdef _DEBUG
			// Test topic string value
			std::string szTopic(buffer.begin(), posStart);
			std::string szTopicJson;
			GetTopic(szTopicJson);
			assert(!szTopic.empty() && szTopic == szTopicJson);
#endif // DEBUG
		}
	}
}

// Serialize message properties topic + magic marker + message type + micro seconds + message to buffer
void CMessage::SerializeMessageToBuffer(std::vector<unsigned char> &buffer)
{
	// Write message topic
	std::string topic;
	GetTopic(topic);
	assert(!topic.empty());
	std::string strMessage = topic;

	// Add header start marker
	strMessage += std::string((const char *)MESSAGE_MARKER_START, sizeof(MESSAGE_MARKER_START));

	// Add header json values
	Json::Value root;
	SerializeHeaderMapToJson(_headerMap, root);
	Json::StreamWriterBuilder builder;
	const std::string json = Json::writeString(builder, root);
	strMessage += json;

	// Add header end marker
	strMessage += std::string((const char *)MESSAGE_MARKER_END, sizeof(MESSAGE_MARKER_END));

	// Convert header to vector
	auto tempData = std::vector<unsigned char>(strMessage.c_str(), strMessage.c_str() + strMessage.size());

	// Append message buffer data
	tempData.insert(tempData.end(), _data.begin(), _data.end());
	buffer = std::move(tempData);
}

// Convert header map to json
void CMessage::SerializeHeaderMapToJson(const std::map<std::string, std::any> map, Json::Value &json)
{
	std::for_each(map.begin(), map.end(), [&](auto item) {
		if (item.second.type() == typeid(std::map<std::string, std::any>))
		{
			Json::Value doc;
			SerializeHeaderMapToJson(std::any_cast<std::map<std::string, std::any>>(item.second), doc);
			json[item.first] = doc;
		}
		else
		{
			json[item.first] = MessageHelper::AnyValueToJson(item.second);
		}
	});
}
