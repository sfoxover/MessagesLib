#pragma once
/*
	Static helper functions for Json serialization
*/
#include <any>
#include <map>
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>
#include <functional>
#include <Message.h>
#include "json/json.h"
#include <cassert>

// MACRO shortcuts for std::any casting
#define CASTANY(o, r) MessageHelper::CastAnyValue(o, r)

class MessageHelper
{
public:
	// Try and cast values to json document
	static Json::Value AnyValueToJson(std::any object);

	// Convert json value to std::any object
	static std::any JsonToAnyValue(Json::Value json);

	// Get std::string value from std::any object
	static std::string StringFromAnyValue(std::any object);

	// Load settings into key value map
	static std::map<std::string, std::any> LoadSettingsFromConfig(std::string configFilePath);

	// Load json values into a key value map
	static void LoadJsonIntoMap(const Json::Value json, std::map<std::string, std::any> &map);

	// Cast an any value into its native object
	template <typename T>
	static void CastAnyValue(std::any object, T &result)
	{
		if (object.type() == typeid(T))
		{
			result = std::any_cast<T>(object);
		}
		else
		{
			std::stringstream stErr;
			stErr << "MessageHelper::CastAnyValue unsupported std::any type: " << object.type().name();
			std::cerr << stErr.str() << std::endl;
			throw std::runtime_error(stErr.str());
		}
	}

	// Helper to convert any array to std::string array
	static std::vector<std::string> AnyArrayToStringArray(std::vector<std::any> object);
};
