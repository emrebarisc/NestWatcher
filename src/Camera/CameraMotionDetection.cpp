#include "CameraMotionDetection.h"
#include "Program.h"
#include "Managers/CameraManager.h"
#include "Managers/InputManager.h"

namespace
{
    inline int FastAbsDiff(int a, int b)
    {
        return a > b ? a - b : b - a;
    }
}

CameraMotionDetection::CameraMotionDetection() = default;

CameraMotionDetection::~CameraMotionDetection()
{
    if (motionDetectionThread_.joinable())
    {
        motionDetectionThread_.join();
    }
}

void CameraMotionDetection::Start()
{
    cameraManager_ = Program::GetInstance()->GetCameraManager();
    inputManager_ = Program::GetInstance()->GetInputManager();

    const int width = LOW_QUALITY_RESOLUTION_WIDTH;
    const int height = LOW_QUALITY_RESOLUTION_HEIGHT;
    const int totalPixels = width * height;

    grayCurr_.resize(totalPixels);
    blurBuffer_.resize(totalPixels);
    blurTemp_.resize(totalPixels);

    motionDetectionThread_ = std::thread(&CameraMotionDetection::DetectMotion, this);
}

void CameraMotionDetection::ConvertToGrayscale(const uint8_t* src, uint8_t* dst, int width, int height)
{
    const int total = width * height * 3;
    for (int i = 0, j = 0; i < total; i += 3, ++j)
    {
        dst[j] = static_cast<uint8_t>((src[i] * 77 + src[i + 1] * 150 + src[i + 2] * 29) >> 8);
    }
}

void CameraMotionDetection::ApplyGaussianBlur(uint8_t* src, uint8_t* dst, int width, int height)
{
    static const float kernel[5] = { 0.06136f, 0.24477f, 0.38774f, 0.24477f, 0.06136f };

    for (int y = 0; y < height; ++y)
    {
        int rowOffset = y * width;
        
        for (int x = 0; x < 2; ++x)
        {
            float sum = 0.0f;
            for (int k = -2; k <= 2; ++k) {
                int px = std::clamp(x + k, 0, width - 1);
                sum += src[rowOffset + px] * kernel[k + 2];
            }
            blurTemp_[rowOffset + x] = sum;
        }

        for (int x = 2; x < width - 2; ++x)
        {
            float sum = src[rowOffset + (x - 2)] * kernel[0] +
                        src[rowOffset + (x - 1)] * kernel[1] +
                        src[rowOffset + x]       * kernel[2] +
                        src[rowOffset + (x + 1)] * kernel[3] +
                        src[rowOffset + (x + 2)] * kernel[4];
            blurTemp_[rowOffset + x] = sum;
        }

        for (int x = width - 2; x < width; ++x)
        {
            float sum = 0.0f;
            for (int k = -2; k <= 2; ++k)
            {
                int px = std::clamp(x + k, 0, width - 1);
                sum += src[rowOffset + px] * kernel[k + 2];
            }
            blurTemp_[rowOffset + x] = sum;
        }
    }

    for (int y = 0; y < height; ++y)
    {
        int rowOffset = y * width;
        
        if (y >= 2 && y < height - 2)
        {
            for (int x = 0; x < width; ++x)
            {
                float sum = blurTemp_[(y - 2) * width + x] * kernel[0] +
                            blurTemp_[(y - 1) * width + x] * kernel[1] +
                            blurTemp_[rowOffset + x]       * kernel[2] +
                            blurTemp_[(y + 1) * width + x] * kernel[3] +
                            blurTemp_[(y + 2) * width + x] * kernel[4];
                dst[rowOffset + x] = static_cast<uint8_t>(sum);
            }
        } 
        else
        {
            for (int x = 0; x < width; ++x)
            {
                float sum = 0.0f;
                for (int k = -2; k <= 2; ++k)
                {
                    int py = std::clamp(y + k, 0, height - 1);
                    sum += blurTemp_[py * width + x] * kernel[k + 2];
                }
                dst[rowOffset + x] = static_cast<uint8_t>(sum);
            }
        }
    }
}

void CameraMotionDetection::DetectMotion()
{
    const int width = LOW_QUALITY_RESOLUTION_WIDTH;
    const int height = LOW_QUALITY_RESOLUTION_HEIGHT;
    const int totalPixels = width * height;

    while (true)
    {
        currentFrameData_ = cameraManager_->GetFrameDataArrayLowQualityGrayscale();

        if (!currentFrameData_)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        grayCurr_.assign(currentFrameData_.get(), currentFrameData_.get() + totalPixels);

        ApplyGaussianBlur(grayCurr_.data(), blurBuffer_.data(), width, height);

        if (blurredPrev_.empty())
        {
            blurredPrev_ = blurBuffer_;
            continue;
        }

        int changedPixels = 0;
        for (int i = 0; i < totalPixels; ++i)
        {
            int diff = FastAbsDiff(blurBuffer_[i], blurredPrev_[i]);
            if (PIXEL_CHANGE_THRESHOLD < diff)
            {
                ++changedPixels;
            }
        }

        if (MOTION_DETECTION_PERCENTAGE * totalPixels < changedPixels)
        {
            CommandMessage cmd;
            cmd.command = Command::TakeAPhotograph;
            inputManager_->ExecuteCommand(cmd);
        }

        blurredPrev_ = blurBuffer_;

        std::this_thread::sleep_for(std::chrono::milliseconds(125));
    }
}