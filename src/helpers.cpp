#include "helpers.h"
#ifdef _WIN32
	#include "windows.h"
	#include <atlbase.h>
	#include <comdef.h>
	#include <Wbemidl.h>
	#pragma comment(lib, "wbemuuid.lib")
#else
	#include <unistd.h>
#endif
#include <filesystem>
#include <codecvt>
#include <iostream>
#include <cassert>
#include <thread>

using namespace std::chrono_literals;

bool Helpers::FileExists(std::wstring path)
{
	bool exists = std::filesystem::exists(std::filesystem::path(path));
	return exists;
}

bool Helpers::FileExists(std::string path)
{
	bool exists = std::filesystem::exists(std::filesystem::path(path));
	return exists;
}

std::string Helpers::WideToUtf8(std::wstring value)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
	auto result = convert.to_bytes(value.c_str());
	return result;
}

std::wstring Helpers::Utf8ToWide(std::string value)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
	std::wstring result = convert.from_bytes(value.c_str());
	return result;
}

std::string Helpers::AppendPath(std::string path1, std::string path2)
{
	std::filesystem::path filePath = std::filesystem::path(path1) /= path2;
	return filePath.u8string();
}

std::string Helpers::AppendToRunPath(std::string path)
{
	char buffer[4096] = {};
	#ifdef _WIN32
		GetModuleFileNameA(NULL, buffer, 4096);
		std::wstring filePath = std::filesystem::path(buffer).parent_path();
		if(path.front() != '\\' && filePath.back() != '\\')
		{
			filePath += L"\\";
		}
		std::string result = Helpers::WideToUtf8(filePath) += path;
	#else
		auto size = readlink("/proc/self/exe", buffer, 4096);
		buffer[size] = '\0';
		std::string filePath(buffer);
		std::string result = filePath.substr(0, filePath.find_last_of('/'));
		if(path.front() != '/' && result.back() != '/')
		{
			result += "/";
		}
		result += path;
	#endif
	bool exists = Helpers::FileExists(result);
	assert(exists);
	return result;
}

// Split string into vector based on delimiter
std::vector<std::string> Helpers::SplitString(std::string value, char delimiter)
{
	std::vector<std::string> results;
	std::string token;
	std::istringstream tokenStream(value);
	while (std::getline(tokenStream, token, delimiter))
	{
		results.push_back(token);
	}
	return results;
}

// Get CPU Temperature
double GetCpuTemperatureWin32()
{
	double result = 0;
#ifdef _WIN32
	HRESULT ci = CoInitialize(NULL);
	if (SUCCEEDED(ci))
	{
		HRESULT hr = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
		CComPtr<IWbemLocator> pLocator;
		hr = CoCreateInstance(CLSID_WbemAdministrativeLocator, NULL, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&pLocator);
		if (SUCCEEDED(hr))
		{
			CComPtr<IWbemServices> pServices;
			CComBSTR ns = L"root\\cimv2";
			hr = pLocator->ConnectServer(ns, NULL, NULL, NULL, 0, NULL, NULL, &pServices);
			if (SUCCEEDED(hr))
			{
				CComBSTR query = L"SELECT * FROM Win32_PerfFormattedData_Counters_ThermalZoneInformation";
				CComBSTR wql = L"WQL";
				CComPtr<IEnumWbemClassObject> pEnum;
				hr = pServices->ExecQuery(wql, query, WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnum);
				if (SUCCEEDED(hr))
				{
					hr = pEnum->Reset();
					do
					{
						CComPtr<IWbemClassObject> pObject;
						ULONG returned = 0;
						hr = pEnum->Next(WBEM_INFINITE, 1, &pObject, &returned);
						if (hr == S_OK)
						{
							CComVariant value;
							HRESULT hrFound = pObject->Get(L"Temperature", 0, &value, NULL, NULL);
							if (SUCCEEDED(hrFound))
							{
								result = value.intVal * 9 / 5 - 459.67;
								break;
							}
						}
					} while (hr == S_OK);
				}
			}

		}
	}
	if (ci == S_OK)
	{
		CoUninitialize();
	}
#endif // _WIN32
	return result;
}

double GetCpuTemperatureLinux()
{
	double result = 0;
#ifndef _WIN32

#endif // _WIN32
	return result;
}

double Helpers::GetCpuTemperature()
{
	double result = 0;
#ifdef _WIN32
	result = GetCpuTemperatureWin32();
#else
	result = GetCpuTemperatureLinux();
#endif
	return result;
}