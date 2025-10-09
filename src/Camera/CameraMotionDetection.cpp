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

int GetGrayscaleDifference(const uint8_t* pixel1, const uint8_t* pixel2)
{
    int diffR = std::abs(pixel1[0] - pixel2[0]);
    int diffG = std::abs(pixel1[1] - pixel2[1]);
    int diffB = std::abs(pixel1[2] - pixel2[2]);
    return (diffR + diffG + diffB) / 3;
}

void CameraMotionDetection::DetectMotion()
{
    const int cameraWidth = cameraManager_->GetCameraWidth();
    const int cameraHeight = cameraManager_->GetCameraHeight();
    const int cameraResolution = cameraWidth * cameraHeight;
    const int bytesPerPixel = 3;
    const int rowStride = cameraWidth * bytesPerPixel;

    while (true)
    {
        previousFrameData_ = currentFrameData_;
        currentFrameData_ = cameraManager_->GetFrameDataArray();

        if (!previousFrameData_ || !currentFrameData_)
        {
            std::this_thread::yield();
            continue;
        }

        int totalChangedPixelCount = 0;

        for (int y = 0; y < cameraHeight; y += 2)
        {
            const int rowOffset = y * rowStride;

            for (int x = 0; x < rowStride; x += bytesPerPixel * 2)
            {
                const int index = rowOffset + x;

                int grayscaleDiff = GetGrayscaleDifference(
                    &currentFrameData_.get()[index],
                    &previousFrameData_.get()[index]);

                if (PIXEL_CHANGE_THRESHOLD < grayscaleDiff)
                {
					++totalChangedPixelCount;
				}
			}
        }

        totalChangedPixelCount *= 4;

        if (MOTION_DETECTION_PERCENTAGE * cameraResolution < totalChangedPixelCount)
        {
            CommandMessage commandMessage;
            commandMessage.command = Command::TakeAPhotograph;
            inputManager_->ExecuteCommand(commandMessage);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(125));
    }
}