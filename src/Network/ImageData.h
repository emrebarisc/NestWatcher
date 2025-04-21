#pragma once

#pragma pack(push, 1)
class alignas(sizeof(uint8_t*)) ImageData
{
public:
	ImageData(){}
	~ImageData(){}
	int rowIndex{ 0 };
	int sectionIndex{ 0 };
	uint8_t row[1280];
};
#pragma pack(pop)

