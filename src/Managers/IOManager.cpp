#include "IOManager.h"

#include <libcamera/libcamera.h>

#include <thread>
#include <chrono>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdio>

#define STB_IMAGE_IMPLEMENTATION
#include "IO/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "IO/stb_image_write.h"

#include "Core.h"

bool fileExists(const std::string& path) 
{
    std::ifstream file(path);
    return file.good();
}

std::string incrementFilename(const std::string& filename) 
{
    std::size_t dotPos = filename.find_last_of('.');
    std::size_t dashPos = filename.find_last_of('-');

    std::string namePart;
    std::string extPart = (dotPos != std::string::npos) ? filename.substr(dotPos) : "";

    if (dotPos == std::string::npos) dotPos = filename.length();  // If no extension

    if (dashPos != std::string::npos && dashPos < dotPos) {
        std::string numberPart = filename.substr(dashPos + 1, dotPos - dashPos - 1);
        bool isNumber = !numberPart.empty() && std::all_of(numberPart.begin(), numberPart.end(), ::isdigit);

        if (isNumber) {
            int number = std::stoi(numberPart);
            number++;

            namePart = filename.substr(0, dashPos);
            return namePart + "-" + std::to_string(number) + extPart;
        }
    }

    // If no valid "-number" part found
    namePart = filename.substr(0, dotPos);
    return namePart + "-1" + extPart;
}


IOManager::~IOManager()
{
	delete saveFrameThread_;
}

void IOManager::SaveFrameAsync(const std::string& fileName, int width, int height, const std::shared_ptr<uint8_t[]>& data)
{
	saveFrameMutex_.lock();

	std::string path = std::string(SAVE_PATH) + fileName;

	if (fileExists(path))
	{
		std::string newPath = incrementFilename(path);
		std::rename(path.c_str(), newPath.c_str());
	}

	auto now = std::chrono::system_clock::now();
	auto msBefore = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

    	if (!stbi_write_jpg(path.c_str(), width, height, 3, data.get(), 100))
	{
        	std::cerr << "Failed to save image!" << std::endl;
    	}
	else
	{
		now = std::chrono::system_clock::now();
		auto msAfter = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        	//std::cout << "Saved frame to " << path << " in " << (msAfter - msBefore) * 0.001f << " seconds."<< std::endl;
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


