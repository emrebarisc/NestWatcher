#pragma once

#include "Managers/IManager.h"

#include <atomic>
#include <string>

#include <netinet/in.h>

#define SERVER_IP "127.0.0.1"

namespace std
{
	class thread;
}

enum class NetworkMode
{
    LocalNetwork,
    AccessPoint
};

class NetworkManager : public IManager
{
	public:
		NetworkManager();
		~NetworkManager();

		void Init() override {}
		void Init(NetworkMode mode = NetworkMode::LocalNetwork);

		void StartListeningCommandAddress();
		void TransmitCameraImage();

	protected:

	private:
		void StartListeningCommandAddressAsync();
		void TransmitCameraImageAsync();
		void TransmitCameraImageLowQualityAsync();

		static constexpr int COMMAND_PORT{ 1000 };
		static constexpr int DATA_PORT{ 1001 };
		static constexpr int CAMERA_IMAGE_COMMAND_PORT{ 1002 };

		int commandSocket_{ 0 };
		int dataSocket_{ 0 };
		int cameraImageCommandSenderSocket_{ 0 };
	
		sockaddr_in commandAddress_{ 0 };
		sockaddr_in dataAddress_{ 0 };
		sockaddr_in cameraImageCommandSenderAddress_{ 0 };

		in_addr clientIP_{ 0 };

		std::thread* listenerThread_{ nullptr };
		std::thread* imageTransmitterThread_{ nullptr };

		std::atomic_bool transmittingCameraImage_{ ATOMIC_VAR_INIT(false) };
};
