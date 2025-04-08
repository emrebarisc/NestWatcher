#pragma once

#include <memory>
#include <thread>

class CameraManager;
class InputManager;

class CameraMotionDetection
{
public:
	CameraMotionDetection();
	~CameraMotionDetection();

	void Start();

protected:

private:
	static constexpr float MOTION_DETECTION_PERCENTAGE = 0.05f;
	static constexpr int PIXEL_CHANGE_THRESHOLD = 50;

	void DetectMotion();

	CameraManager* cameraManager_{ nullptr };
	InputManager* inputManager_{ nullptr };

	uint8_t* frameDifference_{ nullptr };

	std::shared_ptr<uint8_t[]> previousFrameData_{ nullptr };
	std::shared_ptr<uint8_t[]> currentFrameData_{ nullptr };

	std::thread motionDetectionThread_;
};
