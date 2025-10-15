#pragma once

#include "IManager.h"

#include <memory>
#include <string>
#include <thread>

namespace libcamera
{
	class FrameBuffer;
}

class IOManager : public IManager
{
	public:
		IOManager() = default;
		IOManager(const IOManager& other) = default;
		~IOManager();

		IOManager& operator=(const IOManager& other) = default;

		void Init() override {}

		void SaveFrameJPG(const std::shared_ptr<uint8_t[]>& frameData, const std::string &filename, int width, int height, int channel);

	protected:

	private:
		void SaveFrameAsync(const std::string& fileName, int width, int height, int channel, const std::shared_ptr<uint8_t[]>&  data);
		
		std::thread* saveFrameThread_{ nullptr };
		std::mutex saveFrameMutex_;
};
