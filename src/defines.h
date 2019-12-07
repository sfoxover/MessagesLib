#pragma once

// Publisher endpoint
#define VIDEO_PUB_END_POINT "tcp://*:5563"

// Subscriber endpoint
#define VIDEO_SUB_END_POINT "tcp://192.168.2.116:5563"
//#define VIDEO_SUB_END_POINT "tcp://127.0.0.1:5563"

// Publish test video
#define PUBLISH_VIDEO_MP4

// Do facial detection on publisher
//#define DO_FACE_DETECTION_ON_PUBLISHER

// Skip detection on n frames to increase fps
#define SKIP_FRAME_NUM  5

// Run motion sensor
#ifndef _WIN32
    //#define USE_MOTION_SENSOR
#endif // _WIN32

// Max length of a topic string
#define MAX_TOPIC_LENGTH 255
