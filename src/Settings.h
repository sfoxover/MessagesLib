#pragma once
/*
    CSettings - class to load and retrieve various settings from json config file
*/
#include <vector>
#include <string>
#include <map>
#include <any>

class CSettings
{
private:
	CSettings();
	~CSettings();

private:
// Properties

	// Message publish endpoints
	std::string _publishUri;

    // Message subscribe endpoints
	std::string _subscribeUri;

	// Command server endpoint
	std::string _cmdServerUri;

	// Command server endpoint
	std::string _cmdClientUri;

    // Video settings
	bool _useSampleVideo;
	std::string _sampleVideoName;

    // Face detect settings
	bool _useFaceDetect;
	std::string _faceDetectMethod;

	// Topic settings
	std::string _VideoCamTopic;
	std::string _VideoSampleTopic;
	std::string _FaceDetectTopic;
	std::string _MotionSensorTopic;
	std::string _ProfilingTopic;

public:
// Methods
	static CSettings& Instance()
	{
		static CSettings instance;
		return instance;
	}

	// Initialize all settings
	bool Initialize(std::string jsonPath, std::wstring& error);

    // Get access methods
    std::string GetPublishUri();
    std::string GetSubscribeUri();
	std::string GetCmdServerUri();
	std::string GetCmdClientUri();
    bool GetUseSampleVideo();
    std::string GetSampleVideoName();
    
	// Get set for _useFaceDetect
	bool GetUseFaceDetect();
	void SetUseFaceDetect(bool value);
    std::string GetFaceDetectMethod();
	std::string GetVideoCamTopic();
	std::string GetVideoSampleTopic();
	std::string GetFaceDetectTopic();
	std::string GetMotionSensorTopic();
	std::string GetProfilingTopic();
};