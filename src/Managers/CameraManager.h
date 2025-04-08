#pragma once

#include "IManager.h"

#include <memory>
#include <vector>

#include "libcamera/libcamera.h"

class CameraMotionDetection;

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

		CameraMotionDetection* cameraMotionDetection_{ nullptr };

		std::shared_ptr<libcamera::Camera> libcameraCamera_{ nullptr };
		std::unique_ptr<libcamera::CameraManager> libcameraCameraManager_{ nullptr };
		libcamera::FrameBuffer* lastlyCapturedFrame_{ nullptr };
		libcamera::FrameBufferAllocator* frameBufferAllocator_{ nullptr }; 
		libcamera::Stream* libcameraCameraStream_{ nullptr };
		std::vector<std::unique_ptr<libcamera::Request>> libcameraRequests_;

		std::mutex lastlyCapturedFrameMutex_;

		int cameraWidth_{ 1920 };
		int cameraHeight_{ 1080 };
		int colorDepth_ { 4 };

		PixelFormat pixelFormat_{ PixelFormat::XRGB8888 };
		libcamera::PixelFormat libcameraPixelFormat_{ libcamera::formats::XRGB8888 };
};
