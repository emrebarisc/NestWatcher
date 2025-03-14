#pragma once

#include "IManager.h"

#include <memory>

namespace libcamera
{
	class Camera;
	class CameraManager;
	class FrameBufferAllocator;
	class Request;
	class Stream;
}

class CameraManager : public IManager
{
	public:
		CameraManager();
		~CameraManager();

		void Init() override;
		void Tick();

	protected:

	private:
		void RequestComplete(libcamera::Request* request);

		std::shared_ptr<libcamera::Camera> libcameraCamera_{ nullptr };
		std::unique_ptr<libcamera::CameraManager> libcameraCameraManager_{ nullptr };
		libcamera::FrameBufferAllocator* frameBufferAllocator_{ nullptr }; 
		libcamera::Stream* libcameraCameraStream_{ nullptr };
};
