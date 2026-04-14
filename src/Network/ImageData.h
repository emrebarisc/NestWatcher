#pragma once

#include <cstdint>

#pragma pack(push, 1)
class ImageData
{
public:
    uint32_t frameId{ 0 };
    uint16_t chunkIndex{ 0 };
    uint16_t totalChunks{ 0 };
    uint16_t dataSize{ 0 };
    uint8_t data[1024]; 
};
#pragma pack(pop)