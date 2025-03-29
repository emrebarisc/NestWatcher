#include "IOManager.h"

#include <libcamera/libcamera.h>

#include <thread>
#include <chrono>

#define STB_IMAGE_IMPLEMENTATION
#include "IO/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "IO/stb_image_write.h"

IOManager::~IOManager()
{
	delete saveFrameThread_;
}

void IOManager::SaveFrameAsync(const std::string& fileName, int width, int height, const std::shared_ptr<uint8_t[]>& data)
{
	saveFrameMutex_.lock();

	auto now = std::chrono::system_clock::now();
	auto msBefore = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

    	if (!stbi_write_png(fileName.c_str(), width, height, 3, data.get(), width * 3))
	{
        	std::cerr << "Failed to write PNG file!" << std::endl;
    	}
	else
	{
		now = std::chrono::system_clock::now();
		auto msAfter = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        	std::cout << "Saved frame as " << fileName << " in " << (msAfter - msBefore) * 0.001f << " seconds."<< std::endl;
    	}

	saveFrameMutex_.unlock();
}

void IOManager::SaveFrameToPNG(const std::shared_ptr<uint8_t[]>& frameData, const std::string &fileName, int width, int height)
{
	if(frameData == nullptr)
	{
		return;
	}

	/*(saveFrameThread_ && saveFrameThread_->joinable())
	{
		saveFrameThread_->join();
		delete saveFrameThread_;
		saveFrameThread_ = nullptr;
	}

	saveFrameThread_ = new std::thread(&IOManager::SaveFrameAsync, this, fileName, width, height, frameData);
	*/
	std::make_shared<std::thread*>(new std::thread(&IOManager::SaveFrameAsync, this, fileName, width, height, frameData));
}


