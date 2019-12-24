#include "MessageHelper.h"
#include "helpers.h"
#include <iostream>
#include <iosfwd>

// Try and cast values to json document
Json::Value MessageHelper::AnyValueToJson(std::any object)
{
    Json::Value value;
    try
    {
        if (object.type() == typeid(bool))
        {
            value = std::any_cast<bool>(object);
        }
        else if (object.type() == typeid(short))
        {
            value = std::any_cast<short>(object);
        }
        else if (object.type() == typeid(int))
        {
            value = std::any_cast<int>(object);
        }
        else if (object.type() == typeid(const char *))
        {
            value = std::any_cast<const char *>(object);
        }
        else if (object.type() == typeid(std::string))
        {
            value = std::any_cast<std::string>(object);
        }
        else if (object.type() == typeid(const wchar_t*))
        {
            value = Helpers::WideToUtf8(std::any_cast<const wchar_t*>(object));
        }
        else if (object.type() == typeid(std::wstring))
        {
            value = Helpers::WideToUtf8(std::any_cast<std::wstring>(object));
        }
        else if (object.type() == typeid(int64_t))
        {
            value = std::any_cast<int64_t>(object);
        }
        else if (object.type() == typeid(uint64_t))
        {
            value = std::any_cast<uint64_t>(object);
        }
        else
        {
            std::cerr << "MessageHelper::AnyValueToJson unsupported std::any type: " << object.type().name() << std::endl;
            assert(false);
        }
    }
    catch (std::bad_any_cast &err)
    {
        std::cerr << "MessageHelper::AnyValueToJson bad_any_cast: " << err.what() << std::endl;
        assert(false);
    }
    return value;
}

// Convert json value to std::any object
std::any MessageHelper::JsonToAnyValue(Json::Value json)
{
    std::any value;
    try
    {
        if (json.isBool())
        {
            value = json.asBool();
        }
        else if (json.isInt())
        {
            value = json.asInt();
        }
        else if (json.isInt64())
        {
            value = json.asInt64();
        }
        else if (json.isString())
        {
            value = json.asString();
        }
        else if (json.isUInt())
        {
            value = json.asUInt();
        }
        else if (json.isUInt64())
        {
            value = json.asUInt64();
        }
        else if (json.isDouble())
        {
            value = json.asDouble();
        }
        else if (json.isDouble())
        {
            value = json.asDouble();
        }
        else if (json.isArray())
        {
            std::vector<std::any> array;
            std::for_each(json.begin(), json.end(), [&](auto item) {
                array.push_back(JsonToAnyValue(item));
            });
            value = array;
        }
        else
        {
            std::cerr << "MessageHelper::JsonToAnyValue unsupported std::any type: " << json.type() << std::endl;
            assert(false);
        }
    }
    catch (std::bad_any_cast &err)
    {
        std::cerr << "MessageHelper::JsonToAnyValue bad_any_cast: " << err.what() << std::endl;
        assert(false);
    }
    return value;
}

// Get std::string value from std::any object
std::string MessageHelper::StringFromAnyValue(std::any object)
{
    std::string result;
    CastAnyValue(object, result);
    return result;
}

// Load video settings into map
std::map<std::string, std::any> MessageHelper::LoadSettingsFromConfig(std::string configFilePath)
{
    std::map<std::string, std::any> settings;
    if (Helpers::FileExists(configFilePath))
    {
        // Read config file
        std::ifstream fs(configFilePath, std::ifstream::binary);
        Json::Value root;
        Json::CharReaderBuilder builder;
		Json::String err;
        bool bOK = Json::parseFromStream(builder, fs, &root, &err);
        fs.close();
        if (bOK)
        {
            LoadJsonIntoMap(root, settings);
        }
        else
        {
            std::cerr << "LoadSettingsFromConfig json error " << err.c_str() << std::endl;
            assert(bOK);
        }
    }
    return settings;
}

// Load json values into a key value map
void MessageHelper::LoadJsonIntoMap(const Json::Value json, std::map<std::string, std::any> &map)
{
    auto names = json.getMemberNames();
    std::for_each(names.begin(), names.end(), [&](auto name) {
        Json::Value item = json[name];
        if (item.type() == Json::ValueType::objectValue)
        {
            std::map<std::string, std::any> mapTemp;
            LoadJsonIntoMap(item, mapTemp);
            map[name] = mapTemp;
        }
        else
        {
            map[name] = MessageHelper::JsonToAnyValue(item);
        }
    });
}

// Helper to convert any array to std::string array
std::vector<std::string> MessageHelper::AnyArrayToStringArray(std::vector<std::any> object)
{
    std::vector<std::string> result;
    std::for_each(object.begin(), object.end(), [&](auto item) {
        assert(item.type() == typeid(std::string));
        if (item.type() == typeid(std::string))
        {
            auto szItem = std::any_cast<std::string>(item);
            result.push_back(szItem);
        }
    });
    return result;
}