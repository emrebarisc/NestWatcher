#include "CameraMotionDetection.h"
#include "Program.h"
#include "Managers/CameraManager.h"
#include "Managers/InputManager.h"

CameraMotionDetection::CameraMotionDetection() = default;

CameraMotionDetection::~CameraMotionDetection()
{
    if (motionDetectionThread_.joinable())
        motionDetectionThread_.join();
}

void CameraMotionDetection::Start()
{
    cameraManager_ = Program::GetInstance()->GetCameraManager();
    inputManager_ = Program::GetInstance()->GetInputManager();

    const int width = cameraManager_->GetCameraWidth();
    const int height = cameraManager_->GetCameraHeight();

    grayPrev_.resize(width * height);
    grayCurr_.resize(width * height);
    blurBuffer_.resize(width * height);

    motionDetectionThread_ = std::thread(&CameraMotionDetection::DetectMotion, this);
}

void CameraMotionDetection::ConvertToGrayscale(const uint8_t* src, uint8_t* dst, int width, int height)
{
    const int total = width * height * 3;
    for (int i = 0, j = 0; i < total; i += 3, ++j)
    {
        // Weighted average approximation (BT.601)
        dst[j] = static_cast<uint8_t>((src[i] * 0.299f) + (src[i + 1] * 0.587f) + (src[i + 2] * 0.114f));
    }
}

void CameraMotionDetection::ApplyGaussianBlur(uint8_t* src, uint8_t* dst, int width, int height)
{
    // 5x5 Gaussian kernel, separable form (σ ≈ 1.0)
    static const float kernel[5] = { 0.06136f, 0.24477f, 0.38774f, 0.24477f, 0.06136f };
    std::vector<float> temp(width * height);

    // Horizontal pass
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            float sum = 0.0f;
            for (int k = -2; k <= 2; ++k)
            {
                int px = std::clamp(x + k, 0, width - 1);
                sum += src[y * width + px] * kernel[k + 2];
            }
            temp[y * width + x] = sum;
        }
    }

    // Vertical pass
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            float sum = 0.0f;
            for (int k = -2; k <= 2; ++k)
            {
                int py = std::clamp(y + k, 0, height - 1);
                sum += temp[py * width + x] * kernel[k + 2];
            }
            dst[y * width + x] = static_cast<uint8_t>(sum);
        }
    }
}

void CameraMotionDetection::DetectMotion()
{
    const int width = cameraManager_->GetCameraWidth();
    const int height = cameraManager_->GetCameraHeight();
    const int totalPixels = width * height;

    while (true)
    {
        previousFrameData_ = currentFrameData_;
        currentFrameData_ = cameraManager_->GetFrameDataArray();

        if (!previousFrameData_ || !currentFrameData_)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        if(cameraManager_->GetColorDepth() == 1)
        {
            const int frameSize = width * height;
            grayCurr_.assign(currentFrameData_.get(), currentFrameData_.get() + frameSize);
            grayPrev_.assign(previousFrameData_.get(), previousFrameData_.get() + frameSize);
        }
        else
        {
            ConvertToGrayscale(currentFrameData_.get(), grayCurr_.data(), width, height);
            ConvertToGrayscale(previousFrameData_.get(), grayPrev_.data(), width, height);
        }

        ApplyGaussianBlur(grayCurr_.data(), blurBuffer_.data(), width, height);
        grayCurr_.swap(blurBuffer_);
        ApplyGaussianBlur(grayPrev_.data(), blurBuffer_.data(), width, height);
        grayPrev_.swap(blurBuffer_);

        int changedPixels = 0;
        for (int i = 0; i < totalPixels; ++i)
        {
            int diff = std::abs(grayCurr_[i] - grayPrev_[i]);
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

        std::this_thread::sleep_for(std::chrono::milliseconds(125));
    }
}
