#include <chrono>
#include <iostream>
#include <iomanip>
#include <string>
#include <memory>
#include <vector>
#include <thread>

#include <libcamera/libcamera.h>

using namespace std::chrono_literals;

static std::shared_ptr<libcamera::Camera> camera;

static void requestComplete(libcamera::Request* request)
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

		std::cout << " seq: " << std::setw(6) << std::setfill('0') << frameMetadata.sequence << " bytesused: ";

		unsigned int nplane = 0;
		for(const libcamera::FrameMetadata::Plane& plane : frameMetadata.planes())
		{
			std::cout << plane.bytesused;
			if(++nplane < frameMetadata.planes().size())
			{
				std::cout << "/";
			}

			std::cout << std::endl;
		}
	}

	request->reuse(libcamera::Request::ReuseBuffers);
	camera->queueRequest(request);
}

int main(void)
{	
	std::unique_ptr<libcamera::CameraManager> cameraManager = std::make_unique<libcamera::CameraManager>();
	cameraManager->start();

	auto cameras = cameraManager->cameras();
	if(cameras.empty())
	{
		std::cout << "No camera found!" << std::endl;
		cameraManager->stop();
		return EXIT_FAILURE;
	}
	
	std::string cameraId = cameras[0]->id();

	camera = cameraManager->get(cameraId);
	camera->acquire();

	std::unique_ptr<libcamera::CameraConfiguration> cameraConfig = camera->generateConfiguration({ libcamera::StreamRole::Viewfinder });

	libcamera::StreamConfiguration& streamConfig = cameraConfig->at(0);
	std::cout << "Default viewfinder configuration is: " + streamConfig.toString() << std::endl;

	streamConfig.size.width = 1920;
	streamConfig.size.height = 1080;

	cameraConfig->validate();

	std::cout << "Validated viewfinder configuration is: " + streamConfig.toString() << std::endl;

	camera->configure(cameraConfig.get());
	
	libcamera::FrameBufferAllocator* frameBufferAllocator = new libcamera::FrameBufferAllocator(camera);

	for(libcamera::StreamConfiguration& streamConfiguration : *cameraConfig)
	{
		int ret = frameBufferAllocator->allocate(streamConfiguration.stream());
		if(ret < 0)
		{
			std::cerr << "Can't allocate buffers" << std::endl;
			return -ENOMEM;
		}

		size_t allocatedSize = frameBufferAllocator->buffers(streamConfiguration.stream()).size();
		std::cout << "Allocated " << allocatedSize << " buffers for stream" << std::endl;
	}

	libcamera::Stream* cameraStream = streamConfig.stream();
	const std::vector<std::unique_ptr<libcamera::FrameBuffer>>& frameBuffers = frameBufferAllocator->buffers(cameraStream);
	std::vector<std::unique_ptr<libcamera::Request>> requests;

	for(unsigned int i = 0; i < frameBuffers.size(); ++i)
	{
		std::unique_ptr<libcamera::Request> request = camera->createRequest();
		if(!request)
		{
			std::cerr << "Can't create request" << std::endl;
			return -ENOMEM;
		}

		const std::unique_ptr<libcamera::FrameBuffer>& frameBuffer = frameBuffers[i];
		int ret = request->addBuffer(cameraStream, frameBuffer.get());
		if(ret < 0)
		{
			std::cerr << "Can't set buffer for request" << std::endl;
			return ret;
		}

		requests.push_back(std::move(request));
	}

	camera->requestCompleted.connect(requestComplete);
	
	std::this_thread::sleep_for(std::chrono::duration(3000ms));

	camera->start();
	for(std::unique_ptr<libcamera::Request>& request : requests)
	{
		camera->queueRequest(request.get());
	}

	camera->stop();
	frameBufferAllocator->free(cameraStream);
	delete frameBufferAllocator;
	camera->release();
	camera.reset();
	cameraManager->stop();

	return 0;
}
