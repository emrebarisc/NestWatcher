#pragma once

#include "Managers/IManager.h"

#include <string>

#include <netinet/in.h>

#include "Command.h"

#define SERVER_IP "127.0.0.1"

namespace std
{
	class thread;
}

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
