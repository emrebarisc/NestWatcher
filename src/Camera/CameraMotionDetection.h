#pragma once

#include <memory>
#include <thread>
#include <vector>
#include <cmath>
#include <chrono>
#include <algorithm>

class CameraManager;
class InputManager;

class CameraMotionDetection
{
public:
    CameraMotionDetection();
    ~CameraMotionDetection();

    void Start();

private:
    static constexpr float MOTION_DETECTION_PERCENTAGE = 0.03f;
    static constexpr int PIXEL_CHANGE_THRESHOLD = 30;

    void DetectMotion();

    void ApplyGaussianBlur(uint8_t* src, uint8_t* dst, int width, int height);
    void ConvertToGrayscale(const uint8_t* src, uint8_t* dst, int width, int height);

    CameraManager* cameraManager_{ nullptr };
    InputManager* inputManager_{ nullptr };

    std::shared_ptr<uint8_t[]> previousFrameData_{ nullptr };
    std::shared_ptr<uint8_t[]> currentFrameData_{ nullptr };

    std::vector<uint8_t> grayPrev_;
    std::vector<uint8_t> grayCurr_;
    std::vector<uint8_t> blurBuffer_;

    std::thread motionDetectionThread_;
};
