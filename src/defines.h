#pragma once

// Skip detection on n frames to increase fps
#define SKIP_FRAME_NUM  5

// Max length of a topic string
#define MAX_TOPIC_LENGTH 255

// Max publisher messages allowed to queue up before being dropped
#define MAX_PUB_MSG_QUEUE 10