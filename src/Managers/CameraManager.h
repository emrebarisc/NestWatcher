#pragma once

#include "IManager.h"

#include <memory>
#include <vector>

#include "libcamera/libcamera.h"

/*
namespace libcamera
{
	class Camera;
	class CameraManager;
	class Frame;
	class FrameBufferAllocator;
	class Request;
	class Stream;
}
*/

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

	protected:

	private:
		void RequestComplete(libcamera::Request* request);

		std::shared_ptr<libcamera::Camera> libcameraCamera_{ nullptr };
		std::unique_ptr<libcamera::CameraManager> libcameraCameraManager_{ nullptr };
		libcamera::FrameBuffer* lastlyCapturedFrame_{ nullptr };
		libcamera::FrameBufferAllocator* frameBufferAllocator_{ nullptr }; 
		libcamera::Stream* libcameraCameraStream_{ nullptr };
		std::vector<std::unique_ptr<libcamera::Request>> libcameraRequests_;

		std::mutex lastlyCapturedFrameMutex_;

		int cameraWidth_{ 1920 };
		int cameraHeight_{ 1080 };

		libcamera::PixelFormat pixelFormat_{ libcamera::formats::XRGB8888 };
};
