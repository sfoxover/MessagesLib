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

	// Publish endpoints
	std::vector<std::string> _publishUris;

    // Subscribe endpoints
	std::vector<std::string> _subscribeUris;

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
	std::string _MotionSensor;

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
    std::vector<std::string> GetPublishUris();
    std::vector<std::string> GetSubscribeUris();
    bool GetUseSampleVideo();
    std::string GetSampleVideoName();
    bool GetUseFaceDetect();
    std::string GetFaceDetectMethod();
	std::string GetVideoCamTopic();
	std::string GetVideoSampleTopic();
	std::string GetFaceDetectTopic();
	std::string GetMotionSensorTopic();
};