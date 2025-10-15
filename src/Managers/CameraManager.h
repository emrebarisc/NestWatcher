#pragma once

#include "IManager.h"

#include <memory>
#include <vector>

#include "libcamera/libcamera.h"

class CameraMotionDetection;

constexpr int LOW_QUALITY_RESOLUTION_WIDTH = 320;
constexpr int LOW_QUALITY_RESOLUTION_HEIGHT = 240;
constexpr int LOW_QUALITY_COLOR_DEPTH = 3;
constexpr int LOW_QUALITY_RESOLUTION = LOW_QUALITY_RESOLUTION_WIDTH * LOW_QUALITY_RESOLUTION_HEIGHT;

constexpr int HIGH_QUALITY_RESOLUTION_WIDTH = 1920;
constexpr int HIGH_QUALITY_RESOLUTION_HEIGHT = 1080;
constexpr int HIGH_QUALITY_COLOR_DEPTH_FOR_NETWORK = 3;
constexpr int HIGH_QUALITY_RESOLUTION = HIGH_QUALITY_RESOLUTION_WIDTH * HIGH_QUALITY_RESOLUTION_HEIGHT;

enum class PixelFormat : uint8_t
{
	None = 0,

	R8 = 10,

	YUV420 = 30,
	RGB565,
	RGB888,
	
	XRGB8888 = 40,
	RGBA8888
};

class CameraManager : public IManager
{
	public:
		CameraManager();
		~CameraManager();

		void Init() override;
		void Start();

		std::shared_ptr<uint8_t[]> GetFrameDataArray();
		std::shared_ptr<uint8_t[]> GetFrameDataArrayLowQuality();
		std::shared_ptr<uint8_t[]> GetFrameDataArrayLowQualityGrayscale();

		void SetupLowQualityCamera();
		void SetupHighQualityCamera();

		int GetCameraWidth() const
		{
			return cameraWidth_;
		}

		int GetCameraHeight() const
		{
			return cameraHeight_;
		}

		int GetColorDepth() const
		{
			return colorDepth_;
		}

	protected:

	private:
		void RequestComplete(libcamera::Request* request);

		void SetupCamera();

		CameraMotionDetection* cameraMotionDetection_{ nullptr };

		std::shared_ptr<libcamera::Camera> libcameraCamera_{ nullptr };
		std::unique_ptr<libcamera::CameraManager> libcameraCameraManager_{ nullptr };
		libcamera::FrameBuffer* lastlyCapturedFrame_{ nullptr };
		libcamera::FrameBufferAllocator* frameBufferAllocator_{ nullptr }; 
		libcamera::Stream* libcameraCameraStream_{ nullptr };
		std::vector<std::unique_ptr<libcamera::Request>> libcameraRequests_{};

		std::mutex lastlyCapturedFrameMutex_;

		unsigned int cameraWidth_{ HIGH_QUALITY_RESOLUTION_WIDTH };
		unsigned int cameraHeight_{ HIGH_QUALITY_RESOLUTION_HEIGHT };
		unsigned int colorDepth_ { 4 };

		PixelFormat pixelFormat_{ PixelFormat::XRGB8888 };
		libcamera::PixelFormat libcameraPixelFormat_{ libcamera::formats::XRGB8888 };

		uint8_t isCameraRunning_ : 1;
};
