#pragma once

class ImageData
{
public:
	ImageData(){}
	~ImageData()
	{
		delete[] row;
	}

	int rowIndex;
	int sectionIndex;
	uint8_t* row;
};
