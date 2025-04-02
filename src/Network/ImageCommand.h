#pragma once

enum class ImageCommand : uint8_t
{
	None = 0,
	FrameTransmissionStarted,
	FrameTransmissionEnded
};

class ImageCommandMessage
{
public:
	ImageCommand command;
	char commandMessage[64];
};
