#pragma once

#include "IManager.h"

#include <string>

namespace libcamera
{
	class FrameBuffer;
}


class IOManager : public IManager
{
	public:
		IOManager() = default;
		IOManager(const IOManager& other) = default;
		~IOManager() = default;

		IOManager& operator=(const IOManager& other) = default;

		void Init() override {}

		void SaveFrameToPNG(libcamera::FrameBuffer *buffer, const std::string &filename, int width, int height);

	protected:

	private:

};
