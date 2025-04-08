#pragma once

#include <memory>

class CameraMotionDetection
{
public:
	CameraMotionDetection();
	~CameraMotionDetection();

protected:

private:
	std::shared_ptr<uint8_t[]> previousFrameData_{ nullptr };
	std::shared_ptr<uint8_t[]> currentFrameData_{ nullptr };
};
