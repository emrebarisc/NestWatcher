#include "IOManager.h"

#include <iomanip>
#include <sys/mman.h>

#include <libcamera/libcamera.h>

#include <thread>
#include <chrono>

#define STB_IMAGE_IMPLEMENTATION
#include "IO/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "IO/stb_image_write.h"

void IOManager::SaveFrameAsync(const std::string& fileName, int width, int height, void* data)
{
	saveFrameMutex_.lock();

	auto now = std::chrono::system_clock::now();
	auto msBefore = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

    	if (!stbi_write_png(fileName.c_str(), width, height, 3, data, width * 3))
	{
        	std::cerr << "Failed to write PNG file!" << std::endl;
    	}
	else
	{
		now = std::chrono::system_clock::now();
		auto msAfter = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        	std::cout << "Saved frame as " << fileName << " in " << (msAfter - msBefore) * 0.001f << " seconds."<< std::endl;
    	}
	
	munmap(data, width * height * 3);

	saveFrameMutex_.unlock();
}

void IOManager::SaveFrameToPNG(libcamera::FrameBuffer *buffer, const std::string &fileName, int width, int height)
{
    	if (buffer->planes().empty())
	{
        	std::cerr << "No planes in buffer!" << std::endl;
        	return;
    	}

    	int fd = buffer->planes()[0].fd.get();
    	size_t length = buffer->planes()[0].length;
    	void *data = mmap(nullptr, length, PROT_READ, MAP_SHARED, fd, 0);

	assert(length == width * height * 3);
 
	if (data == MAP_FAILED)
    	{
        	std::cerr << "Failed to map buffer memory!" << std::endl;
        	return;
    	}

	if(saveFrameThread_ && saveFrameThread_->joinable())
	{
		saveFrameThread_->join();
		delete saveFrameThread_;
		saveFrameThread_ = nullptr;
	}

	saveFrameThread_ = new std::thread(&IOManager::SaveFrameAsync, this, fileName, width, height, data);
}


