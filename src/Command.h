#pragma once

enum class Command : unsigned char
{
	None = 0,
	Connect,
	Disconnect,

	StartCamera,
	StopCamera,

	SetResolution,
	SetFPS,
	SetPixelFormat,

	StartRecording,
	PauseRecording,
	StopRecording,

	TakeAPhotograph,

	StartStream,
	StopStream,

	Shutdown
};

class CommandMessage
{
public:
	Command command;
	char commandMessage[64];
};
