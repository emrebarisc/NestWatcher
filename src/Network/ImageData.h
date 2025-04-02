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
	uint8_t* row;
};
