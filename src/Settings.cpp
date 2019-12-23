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
        std::vector<std::any> anyUris;
        
        // Load publisher endpoints from json config file
        std::map<std::string, std::any> publisher;
        CASTANY(settingsMap["Publisher"], publisher);
        CASTANY(publisher["Endpoints"], anyUris);
        _publishUris = MessageHelper::AnyArrayToStringArray(anyUris);

        // Subscriber endpoints
        std::map<std::string, std::any> subscriber;
        CASTANY(settingsMap["Subscriber"], subscriber);
        CASTANY(subscriber["Endpoints"], anyUris);
        _subscribeUris = MessageHelper::AnyArrayToStringArray(anyUris);

        // Command server endpoints
        std::map<std::string, std::any> server;
        CASTANY(settingsMap["CmdServer"], server);
        CASTANY(server["Endpoints"], anyUris);
        _cmdServerUris = MessageHelper::AnyArrayToStringArray(anyUris);

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
        CASTANY(settingsMap["MotionSensorTopic"], _MotionSensor);

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
std::vector<std::string> CSettings::GetPublishUris()
{
    return _publishUris;
}

std::vector<std::string> CSettings::GetSubscribeUris()
{
    return _subscribeUris;
}

std::vector<std::string> CSettings::GetCmdServerUris()
{
    return _cmdServerUris;
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
    return _MotionSensor;
}
