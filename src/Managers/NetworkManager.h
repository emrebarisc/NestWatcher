#pragma once

#include "Managers/IManager.h"

#include <string>

#include <netinet/in.h>

#define SERVER_IP "192.168.31.154"

namespace std
{
	class thread;
}

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
	StopStream
};

class CommandMessage
{
	Command command;
	char commandMassage[64];
};

class NetworkManager : public IManager
{
	public:
		NetworkManager();
		~NetworkManager();

		virtual void Init() override;

		void StartListeningCommandAddress();

	protected:

	private:
		void StartListeningCommandAddressAsync();

		static constexpr int COMMAND_PORT{ 1000 };
		static constexpr int DATA_PORT{ 1001 };

		int commandSocket_{ 0 };
		int dataSocket_{ 0 };

		sockaddr_in commandAddress_{ 0 };
		sockaddr_in dataAddress_{ 0 };

		in_addr clientIP_{ 0 };

		std::thread* listenerThread_{ nullptr };
};
