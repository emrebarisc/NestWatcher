#pragma once

#include <cstdint>

#pragma pack(push, 1)
constexpr int MAX_CHUNK_SIZE = 1400;

struct ImageData {
    uint32_t frameId;
    int totalChunks;
    int chunkIndex;
    int dataSize;
    uint8_t data[MAX_CHUNK_SIZE];
};

#pragma pack(pop)