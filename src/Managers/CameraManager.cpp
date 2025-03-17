#include "CameraManager.h"

#include <chrono>
#include <thread>

#include <libcamera/libcamera.h>

#include "Program.h"
#include "Managers/IOManager.h"

using namespace std::chrono_literals;

static std::shared_ptr<libcamera::Camera> libcameraCamera_;

CameraManager::CameraManager(){}
CameraManager::~CameraManager()
{
	libcameraCamera_->stop();
	frameBufferAllocator_->free(libcameraCameraStream_);
	delete frameBufferAllocator_;
	libcameraCamera_->release();
	libcameraCamera_.reset();
	libcameraCameraManager_->stop();
}

void CameraManager::RequestComplete(libcamera::Request* request)
{
	if(request->status() == libcamera::Request::RequestCancelled)
	{
		return;
	}

	const std::map<const libcamera::Stream*, libcamera::FrameBuffer*>& frameBuffers = request->buffers();

	for(auto frameBufferPair : frameBuffers)
	{
		libcamera::FrameBuffer* frameBuffer = frameBufferPair.second;
		const libcamera::FrameMetadata& frameMetadata = frameBuffer->metadata();
		
		Program::GetInstance()->GetIOManager()->SaveFrameToPNG(frameBuffer, "output.png", 1920, 1080);
	}

	request->reuse(libcamera::Request::ReuseBuffers);
	libcameraCamera_->queueRequest(request);
}

void CameraManager::Init()
{
	libcameraCameraManager_ = std::make_unique<libcamera::CameraManager>();
	libcameraCameraManager_->start();

	auto cameras = libcameraCameraManager_->cameras();
	if(cameras.empty())
	{
		std::cout << "No camera found!" << std::endl;
		return;
	}
	
	std::string cameraId = cameras[0]->id();

	libcameraCamera_ = libcameraCameraManager_->get(cameraId);
	libcameraCamera_->acquire();

	std::unique_ptr<libcamera::CameraConfiguration> cameraConfig = libcameraCamera_->generateConfiguration({ libcamera::StreamRole::Viewfinder });

	libcamera::StreamConfiguration& streamConfig = cameraConfig->at(0);
	std::cout << "Default viewfinder configuration is: " + streamConfig.toString() << std::endl;

	streamConfig.size.width = 1920;
	streamConfig.size.height = 1080;

	cameraConfig->validate();

	std::cout << "Validated viewfinder configuration is: " + streamConfig.toString() << std::endl;

	libcameraCamera_->configure(cameraConfig.get());
	
	frameBufferAllocator_ = new libcamera::FrameBufferAllocator(libcameraCamera_);

	for(libcamera::StreamConfiguration& streamConfiguration : *cameraConfig)
	{
		int ret = frameBufferAllocator_->allocate(streamConfiguration.stream());
		if(ret < 0)
		{
			std::cerr << "Can't allocate buffers" << std::endl;
			return;
		}

		size_t allocatedSize = frameBufferAllocator_->buffers(streamConfiguration.stream()).size();
		std::cout << "Allocated " << allocatedSize << " buffers for stream" << std::endl;
	}

	libcameraCameraStream_ = streamConfig.stream();
}

void CameraManager::Start()
{
	const std::vector<std::unique_ptr<libcamera::FrameBuffer>>& frameBuffers = frameBufferAllocator_->buffers(libcameraCameraStream_);
	
	for(unsigned int i = 0; i < frameBuffers.size(); ++i)
	{
		std::unique_ptr<libcamera::Request> request = libcameraCamera_->createRequest();
		if(!request)
		{
			std::cerr << "Can't create request" << std::endl;
			return;
		}

		const std::unique_ptr<libcamera::FrameBuffer>& frameBuffer = frameBuffers[i];
		int ret = request->addBuffer(libcameraCameraStream_, frameBuffer.get());
		if(ret < 0)
		{
			std::cerr << "Can't set buffer for request" << std::endl;
			return;
		}

		libcameraRequests_.push_back(std::move(request));
	}

	libcameraCamera_->requestCompleted.connect(this, &CameraManager::RequestComplete);
	
	std::this_thread::sleep_for(std::chrono::duration(3000ms));
	
	libcameraCamera_->start();
	for(std::unique_ptr<libcamera::Request>& request : libcameraRequests_)
	{
		libcameraCamera_->queueRequest(request.get());
	}
}
