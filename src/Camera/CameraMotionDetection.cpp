#include "CameraMotionDetection.h"

#include <chrono>

#include "Program.h"
#include "Managers/CameraManager.h"
#include "Managers/InputManager.h"

CameraMotionDetection::CameraMotionDetection()
{

}

CameraMotionDetection::~CameraMotionDetection()
{
	delete[] frameDifference_;
}

void CameraMotionDetection::Start()
{
	cameraManager_ = Program::GetInstance()->GetCameraManager();
	inputManager_ = Program::GetInstance()->GetInputManager();

	const int cameraWidth = cameraManager_->GetCameraWidth();
	const int cameraHeight = cameraManager_->GetCameraHeight();

	//frameDifference_ = new uint8_t[cameraWidth * cameraHeight * 3];
	motionDetectionThread_ = std::thread(&CameraMotionDetection::DetectMotion, this);
}

int GetGrayscaleDifference(uint8_t* frameOne, uint8_t* frameTwo)
{
	return (abs(frameOne[0] - frameTwo[0]) + abs(frameOne[1] - frameTwo[1]) + abs(frameOne[2] - frameTwo[2])) / 3;
}

void CameraMotionDetection::DetectMotion()
{
	while(true)
	{
		previousFrameData_ = currentFrameData_;
		currentFrameData_ = cameraManager_->GetFrameDataArray();

		if(!previousFrameData_ || !currentFrameData_)
		{
			std::this_thread::yield();
			continue;
		}

		const int cameraWidth = cameraManager_->GetCameraWidth();
		const int cameraHeight = cameraManager_->GetCameraHeight();
		const int cameraResolution = cameraWidth * cameraHeight;
		const int cameraWidthWithColorDepth = cameraWidth * 3;

		int totalChangedPixelCount = 0;

		for(int y = 0; y < cameraHeight; ++y)
		{
			for(int x = 0; x < cameraWidthWithColorDepth; x += 3)
			{
				const int index = y * cameraWidthWithColorDepth + x;

				//frameDifference_[index] = abs(currentFrameData_.get()[index] - previousFrameData_.get()[index]);
				//frameDifference_[index + 1] = abs(currentFrameData_.get()[index + 1] - previousFrameData_.get()[index + 1]);
				//frameDifference_[index + 2] = abs(currentFrameData_.get()[index + 2] - previousFrameData_.get()[index + 2]);
			
				int grayscaleDifference = GetGrayscaleDifference(&currentFrameData_.get()[0], &previousFrameData_.get()[0]);

				if(PIXEL_CHANGE_THRESHOLD < grayscaleDifference)
				{
					++totalChangedPixelCount;
				}
			}	
		}

		if((MOTION_DETECTION_PERCENTAGE * cameraResolution) < totalChangedPixelCount)
		{
			CommandMessage commandMessage;
			commandMessage.command = Command::TakeAPhotograph;

			inputManager_->ExecuteCommand(commandMessage);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}
}
