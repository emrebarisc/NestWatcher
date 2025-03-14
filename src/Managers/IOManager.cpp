#include "IOManager.h"

#include <iomanip>
#include <sys/mman.h>

#include <libcamera/libcamera.h>

#define STB_IMAGE_IMPLEMENTATION
#include "IO/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "IO/stb_image_write.h"

void IOManager::SaveFrameToPNG(libcamera::FrameBuffer *buffer, const std::string &filename, int width, int height)
{
    if (buffer->planes().empty()) {
        std::cerr << "No planes in buffer!" << std::endl;
        return;
    }

    int fd = buffer->planes()[0].fd.get();
    size_t length = buffer->planes()[0].length;
    void *data = mmap(nullptr, length, PROT_READ, MAP_SHARED, fd, 0);

    if (data == MAP_FAILED) {
        std::cerr << "Failed to map buffer memory!" << std::endl;
        return;
    }

    // Convert XRGB8888 to RGB (skip X byte)
    std::vector<uint8_t> rgbData(width * height * 3);
    uint32_t *pixelData = reinterpret_cast<uint32_t *>(data);

    for (int i = 0; i < width * height; ++i) {
        uint32_t pixel = pixelData[i];
        rgbData[i * 3] = (pixel >> 16) & 0xFF;   // Red
        rgbData[i * 3 + 1] = (pixel >> 8) & 0xFF; // Green
        rgbData[i * 3 + 2] = pixel & 0xFF;       // Blue
    }

    // Save as PNG
    if (!stbi_write_png(filename.c_str(), width, height, 3, rgbData.data(), width * 3)) {
        std::cerr << "Failed to write PNG file!" << std::endl;
    } else {
        std::cout << "Saved frame as " << filename << std::endl;
    }

    munmap(data, length);
}


