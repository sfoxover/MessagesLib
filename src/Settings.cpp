#include "Settings.h"
#include "helpers.h"
#include "MessageHelper.h"

CSettings::CSettings()
{
}

CSettings::~CSettings()
{
}

// Initialize all settings
bool CSettings::Initialize(std::string jsonPath, std::wstring &error)
{
    try
    {
        // Load json settings values
        auto settingsMap = MessageHelper::LoadSettingsFromConfig(jsonPath);
        
        // Load publisher endpoint
        CASTANY(settingsMap["PublisherEndpoint"], _publishUri);

        // Subscriber endpoints
        CASTANY(settingsMap["SubscriberEndpoint"], _subscribeUri);

        // Command server endpoint
        CASTANY(settingsMap["CmdServerEndpoint"], _cmdServerUri);

        // Command client endpoint
        CASTANY(settingsMap["CmdClientEndpoint"], _cmdClientUri);

        // Set face detection settings
        std::map<std::string, std::any> videoSettings;
        CASTANY(settingsMap["VideoSettings"], videoSettings);
        CASTANY(videoSettings["StreamSampleVideo"], _useSampleVideo);
        CASTANY(videoSettings["SampleVideoName"], _sampleVideoName);
        CASTANY(videoSettings["RunFaceDetection"], _useFaceDetect);
        CASTANY(videoSettings["FaceDetectionMethod"], _faceDetectMethod);

        // Topic settings
        CASTANY(settingsMap["VideoCamTopic"], _VideoCamTopic);
        CASTANY(settingsMap["VideoSampleTopic"], _VideoSampleTopic);
        CASTANY(settingsMap["FaceDetectTopic"], _FaceDetectTopic);
        CASTANY(settingsMap["MotionSensorTopic"], _MotionSensorTopic);
        CASTANY(settingsMap["ProfilingTopic"], _ProfilingTopic);

        return true;
    }
    catch (const std::exception &e)
    {
        error = L"CSettings::Initialize error " + Helpers::Utf8ToWide(e.what());
        std::wcerr << error << std::endl;
    }
    return false;
}

// Get access methods
std::string CSettings::GetPublishUri()
{
    return _publishUri;
}

std::string CSettings::GetSubscribeUri()
{
    return _subscribeUri;
}

std::string CSettings::GetCmdServerUri()
{
    return _cmdServerUri;
}

std::string CSettings::GetCmdClientUri()
{
    return _cmdClientUri;
}

bool CSettings::GetUseSampleVideo()
{
    return _useSampleVideo;
}

std::string CSettings::GetSampleVideoName()
{
    return _sampleVideoName;
}

bool CSettings::GetUseFaceDetect()
{
    return _useFaceDetect;
}

void CSettings::SetUseFaceDetect(bool value)
{
    _useFaceDetect = value;
}

std::string CSettings::GetFaceDetectMethod()
{
    return _faceDetectMethod;
}

std::string CSettings::GetVideoCamTopic()
{
    return _VideoCamTopic;
}

std::string CSettings::GetVideoSampleTopic()
{
    return _VideoSampleTopic;
}

std::string CSettings::GetFaceDetectTopic()
{
    return _FaceDetectTopic;
}

std::string CSettings::GetMotionSensorTopic()
{
    return _MotionSensorTopic;
}

std::string CSettings::GetProfilingTopic()
{
    return _ProfilingTopic;
}
